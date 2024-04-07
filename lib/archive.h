#ifndef LABWORK6_ARCHIVE_H
#define LABWORK6_ARCHIVE_H

#include <map>
#include <string>
#include <vector>
#include "file.h"

/**
 * Archive is a binary file structured as shown below:
 * coded binary file 1
 * coded binary file 2
 * coded binary file 3
 * ...
 */

class Archive {
//private:
public:
    std::map<std::string, File> file_list;
    std::string full_archive_path;
    std::string short_archive_path;

    void set_full_archive_path(const std::string& directory_path);

    std::pair<size_t, size_t> get_file_borders(const std::string& file_name);

    void merge_archive_files(Archive& other) const;

    void merge_archive_files(File& other) const;

    void erase_archive_files(const std::string& file_name);

public:
    Archive() = default;

    void set_short_archive_path(const std::string& archive_name);

    void create_archive_file(const std::string& directory_path);

    void add_file(const std::string& file_name);

    void delete_file(const std::string& file_name);

    std::vector<std::string> get_list();

    void extract_file(const std::string& directory_path);

    void extract_file(const std::vector<std::string>& files,
                      const std::string& directory_path);

    void merge_archive(Archive other);

    void set_temp_file(const std::string& file_name, const std::string& temp_file);

    void rewrite_archive_file(const std::string& temp_file) const;
};

#endif //LABWORK6_ARCHIVE_H
