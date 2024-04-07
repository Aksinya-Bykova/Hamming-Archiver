#include <fstream>
#include <iostream>
#include "archive.h"

std::vector<std::string> Archive::get_list() {
    if (file_list.empty()) {
        throw std::invalid_argument("Can't get empty list!");
    }

    std::vector<std::string> res;

    for (const auto& el: file_list) {
        res.push_back(el.first);
        std::cout << el.first << "\n";
    }

    return res;
}

void Archive::add_file(const std::string& file_name) {
    File current_file(file_name);
    merge_archive_files(current_file);
    file_list[file_name] = current_file;
}

void Archive::delete_file(const std::string& file_name) {
    if (auto iter = file_list.find(file_name); iter != file_list.end()) {
        erase_archive_files(iter->first);
        file_list.erase(iter->first);
        return;
    }

    throw std::invalid_argument("Can't delete not existing file!");
}

void Archive::merge_archive(Archive other) {
    file_list.merge(other.file_list);
    merge_archive_files(other);
}

void Archive::create_archive_file(const std::string& directory_path) {
    set_full_archive_path(directory_path);

    for (auto& el: file_list) {
        el.second.set_file(full_archive_path);
    }
}

std::string get_extract_path(const std::string& path) {
    std::size_t found = path.find('.');
    if (found != std::string::npos) {
        return path.substr(0, found - 1) + "_1" + path.substr(found, path.size() - 1);
    }

    return path + "_1";
}

void Archive::extract_file(const std::string& directory_path) {
    size_t begin = 0;

    for (auto& el: file_list) {
        std::string extract_path = get_extract_path(get_extract_path(el.first));

        size_t end = get_file_borders(el.first).second;
        el.second.get_file(full_archive_path, begin, end, extract_path);
        begin = end;
    }
}

void Archive::extract_file(const std::vector<std::string>& files,
                           const std::string& directory_path) {
    size_t begin = 0;

    for (const auto& file_name: files) {
        if (auto iter = file_list.find(file_name); iter != file_list.end()) {
            std::string extract_path = directory_path + "/" + iter->first;

            size_t end = get_file_borders(iter->first).second;
            iter->second.get_file(full_archive_path, begin, end, extract_path);
            begin = end;
        } else {
            throw std::invalid_argument("Can't delete not existing file!");
        }
    }
}

std::pair<size_t, size_t> Archive::get_file_borders(const std::string& file_name) {
    bool not_found = true;
    size_t begin = 0;
    size_t end = 0;

    for (const auto& el: file_list) {
        end += el.second.get_size();
        if (el.first == file_name) {
            not_found = false;
            break;
        }
        begin += el.second.get_size();
    }

    if (not_found) {
        throw std::invalid_argument("Can't get not existing file!");
    }

    return std::make_pair(begin, end);
}

void Archive::merge_archive_files(File& other) const {
    other.set_file(full_archive_path);
}

void Archive::merge_archive_files(Archive& other) const {
    for (auto& el: other.file_list) {
        el.second.set_file(full_archive_path);
    }
}

void Archive::set_short_archive_path(const std::string& archive_name) {
    short_archive_path = archive_name;
}

void Archive::set_full_archive_path(const std::string& directory_path) {
    full_archive_path = directory_path + "/" + short_archive_path;
}

void Archive::erase_archive_files(const std::string& file_name) {
    std::string temp_file = file_name + "_temp";

    set_temp_file(file_name, temp_file);
    std::remove(full_archive_path.c_str());

    rewrite_archive_file(temp_file);
    std::remove(temp_file.c_str());
}

void Archive::rewrite_archive_file(const std::string& temp_file) const {
    std::ifstream in(temp_file, std::ifstream::binary);

    if (!in.is_open()) {
        throw std::invalid_argument("Failed! Can't open input file!");
    }

    std::ofstream out(full_archive_path, std::ios::app);

    if (!out.is_open()) {
        throw std::invalid_argument("Path does not exist!");
    }

    char ch;

    while (in.good()) {
        in >> ch;
        out << ch;
    }

    in.close();
    out.close();
}

void Archive::set_temp_file(const std::string& file_name, const std::string& temp_file) {
    std::ifstream in(full_archive_path, std::ifstream::binary);

    if (!in.is_open()) {
        throw std::invalid_argument("Failed! Can't open input file!");
    }

    std::ofstream temp_out(temp_file, std::ios::app);

    if (!temp_out.is_open()) {
        throw std::invalid_argument("Path does not exist!");
    }

    size_t begin = get_file_borders(file_name).first;
    size_t end = get_file_borders(file_name).second;
    char ch;

    for (size_t i = 0; i < begin && in.good(); ++i) {
        in >> ch;
        temp_out << ch;
    }

    for (size_t i = begin; i < end && in.good(); ++i) {
        in >> ch;
    }

    for (size_t i = end; in.good() && in >> ch; ++i) {
        temp_out << ch;
    }

    in.close();
    temp_out.close();
}
