#include <iostream>
#include "../lib/parser.h"

// hamarc --create -f ARCHIVE --append /home/xunya/CLionProjects/labwork6/tests/Screenshot-from-2024-01-04-20-00-18.png -x
// hamarc --create -f ARCHIVE --append /home/xunya/CLionProjects/labwork6/tests/text-file.txt -x

int main(int argc, const char** argv) {
    std::string directory_path = "/home/xunya/CLionProjects/labwork6/tests";

    parse(argc, argv, directory_path);

    return 0;
}
