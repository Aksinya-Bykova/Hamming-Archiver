#include <iostream>
#include <cstring>
#include "parser.h"

const char* kUtilityName = "hamarc";

void parse(int argc, const char** argv, const std::string& directory_path) {
    Argument current_arg;

    if (strcmp(argv[1], kUtilityName) != 0) {
        throw std::invalid_argument("Incorrect utility name!");
    }

    for (int i = 2; i < argc; ++i) {
        current_arg.set_token(argv[i]);
    }

    current_arg.operate_archiver(directory_path);
}

void Argument::set_token(const char* str) {
    for (const auto& iter: token_type) {
        if (strcmp(iter.first, str) == 0) {
            commands[iter.second] = true;
            return;
        }
    }

    if (!is_file_flag(str)) {
        set_name(str);
    }
}

Argument::Argument() {
    token_type["-c"] = "create";
    token_type["--create"] = "create";
    token_type["-f"] = "file";
    token_type["-l"] = "list";
    token_type["--list"] = "list";
    token_type["-x"] = "extract";
    token_type["--extract"] = "extract";
    token_type["-a"] = "append";
    token_type["--append"] = "append";
    token_type["-d"] = "delete";
    token_type["--delete"] = "delete";
    token_type["-A"] = "concatenate";
    token_type["--concatenate"] = "concatenate";

    commands["create"] = false;
    commands["file"] = false;
    commands["list"] = false;
    commands["extract"] = false;
    commands["append"] = false;
    commands["delete"] = false;
    commands["concatenate"] = false;
}

bool Argument::is_file_flag(const char* str) {
    const char* begin = "--file=";
    size_t begin_size = 0;

    for (size_t i = 0; begin[i] != '\0'; ++i) {
        if (str[i] != begin[i]) {
            return false;
        }
        ++begin_size;
    }

    std::string name;

    for (size_t i = begin_size; str[i] != '\0'; ++i) {
        name.push_back(str[i]);
    }

    Archive archive;
    archive.set_short_archive_path(name);
    archives.push_back(archive);

    return true;
}

void Argument::operate_archiver(const std::string& directory_path) {
    if (commands["create"]) {
        for (auto ar: archives) {
            ar.create_archive_file(directory_path);
        }
    }
    if (commands["concatenate"]) {
        for (size_t i = 1; i < archives.size(); ++i) {
            archives[i].merge_archive(archives[i - 1]);
        }
    }
    for (auto ar: archives) {
        ar.set_full_archive_path(directory_path);
        if (commands["append"]) {
            for (const auto& el: file_names) {
                ar.add_file(el);
            }
            file_names.clear();
        }
        if (commands["extract"]) {
            if (file_names.empty()) {
                ar.extract_file(directory_path);
            } else {
                ar.extract_file(file_names, directory_path);
            }
        }
        if (commands["delete"]) {
            for (const auto& el: file_names) {
                ar.delete_file(el);
            }
        }
        if (commands["list"]) {
            ar.get_list();
        }
    }
}

void Argument::set_name(const char* name) {
    if (commands["file"] || commands["concatenate"]) {
        Archive archive;
        archive.set_short_archive_path(name);
        archives.push_back(archive);
        commands["file"] = false;
        commands["concatenate"] = false;
    } else {
        file_names.push_back(name);
    }
}
