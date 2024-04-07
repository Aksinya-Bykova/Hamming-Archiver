#ifndef LABWORK6_PARSER_H
#define LABWORK6_PARSER_H

#include <map>
#include <string>
#include "archive.h"

typedef bool flag;

class Argument {
public:
    std::map<std::string, bool> commands;

    std::vector<Archive> archives;

    std::vector<std::string> file_names;

    std::map<const char*, std::string> token_type;

    bool is_file_flag(const char* str);

    void set_name(const char* name);

    Argument();

    void set_token(const char* str);

    void operate_archiver(const std::string& directory_path);
};

void parse(int argc, const char** argv, const std::string& directory_path);

#endif //LABWORK6_PARSER_H
