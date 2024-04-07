#include <gtest/gtest.h>
#include <fstream>

#include "../lib/parser.h"

void make_test_file(const std::string& file_path) {
    std::ofstream out(file_path, std::ios::app);
    if (!out.is_open()) {
        throw std::invalid_argument("Path does not exist!");
    }

    std::vector<char> arr = {'a' - 'a', 'b' - 'a', 'c' - 'a', 'd' - 'a', 'e' - 'a',
                             'f' - 'a', 'g' - 'a', 'h' - 'a', 'i' - 'a', 'j' - 'a'};

    for (size_t i = 0; i < 10; ++i) {
        out << arr[i];
    }

    out.close();
}

void set_three_files() {
    std::string file1_path = "/home/xunya/CLionProjects/labwork6/tests/FILE1";
    make_test_file(file1_path);
    File file1(file1_path);


    std::string file2_path = "/home/xunya/CLionProjects/labwork6/tests/FILE2";
    make_test_file(file2_path);
    File file2(file2_path);


    std::string file3_path = "/home/xunya/CLionProjects/labwork6/tests/FILE3";
    make_test_file(file3_path);
    File file3(file3_path);
}

void clear_file(const std::string& file_name) {
    std::ofstream ofs;
    ofs.open(file_name, std::ofstream::out | std::ofstream::trunc);
    ofs.close();
}

TEST(ParseSuit, ParseFirstSample) {
//    hamarc --create --file=ARCHIVE FILE1 FILE2 FILE3
    std::string directory_path = "/home/xunya/CLionProjects/labwork6/tests";
    Argument arg;

    arg.set_token("--create");
    arg.set_token("--file=ARCHIVE");

    set_three_files();
    clear_file("/home/xunya/CLionProjects/labwork6/tests/ARCHIVE");

    arg.set_token("/home/xunya/CLionProjects/labwork6/tests/FILE1");
    arg.set_token("/home/xunya/CLionProjects/labwork6/tests/FILE2");
    arg.set_token("/home/xunya/CLionProjects/labwork6/tests/FILE3");

    arg.operate_archiver(directory_path);
}

TEST(ParseSuit, ParsePracticeSample) {
    std::string directory_path = "/home/xunya/CLionProjects/labwork6/tests";
    Argument arg;

    clear_file("/home/xunya/CLionProjects/labwork6/tests/ARCHIVE");

    arg.set_token("--create");
    arg.set_token("-f");
    arg.set_token("ARCHIVE");

    arg.set_token("--append");
    arg.set_token("/home/xunya/CLionProjects/labwork6/tests/Screenshot-from-2024-01-04-20-00-18.png");

    arg.set_token("-x");

    arg.operate_archiver(directory_path);

    ASSERT_EQ(arg.archives.size(), 1);
}

TEST(ParseSuit, ParseSecondSample) {
//    hamarc -l -f ARCHIVE
    std::string directory_path = "/home/xunya/CLionProjects/labwork6/tests";
    Argument arg;

    clear_file("/home/xunya/CLionProjects/labwork6/tests/ARCHIVE");

    arg.set_token("--create");
    arg.set_token("-l");
    arg.set_token("-f");
    arg.set_token("ARCHIVE");

    set_three_files();

    arg.set_token("--append");
    arg.set_token("/home/xunya/CLionProjects/labwork6/tests/FILE1");
    arg.set_token("/home/xunya/CLionProjects/labwork6/tests/FILE2");
    arg.set_token("/home/xunya/CLionProjects/labwork6/tests/FILE3");

    arg.operate_archiver(directory_path);
}

TEST(ParseSuit, ParseThirdSample) {
//hamarc --concantenate ARCHIVE1 ARCHIVE2 -f ARCHIVE3
    std::string directory_path = "/home/xunya/CLionProjects/labwork6/tests";

    Argument arg;

    arg.set_token("--concatenate");
    arg.set_token("ARCHIVE1");
    arg.set_token("ARCHIVE2");
    arg.set_token("-f");
    arg.set_token("ARCHIVE3");

    arg.operate_archiver(directory_path);
}

TEST(ArchiveSuit, ArchiveName) {
    std::string directory_path = "/home/xunya/CLionProjects/labwork6/tests";

    Archive archive;
    archive.set_short_archive_path("FILE1");
    archive.set_full_archive_path(directory_path);

    ASSERT_EQ(archive.short_archive_path, "FILE1");
    ASSERT_EQ(archive.full_archive_path, "/home/xunya/CLionProjects/labwork6/tests/FILE1");
}

TEST(ParserSuit, TokenFirstSample) {
    Argument arg;

    arg.set_token("--create");
    arg.set_token("--file=ARCHIVE");
    arg.set_token("--extract");

    arg.set_token("FILE1");
    arg.set_token("FILE2");
    arg.set_token("FILE3");

    ASSERT_EQ(arg.commands["create"], true);
    ASSERT_EQ(arg.commands["delete"], false);
    ASSERT_EQ(arg.commands["extract"], true);

    ASSERT_EQ(arg.archives.size(), 1);
    ASSERT_EQ(arg.archives[0].short_archive_path, "ARCHIVE");

    ASSERT_EQ(arg.file_names[0], "FILE1");
    ASSERT_EQ(arg.file_names[1], "FILE2");
    ASSERT_EQ(arg.file_names[2], "FILE3");
}

TEST(BinaryBlockSuit, PrintCoded) {
    BinaryBlock block;
    block.block_size = 10;

    ASSERT_EQ(block.get_control_size(), 4);
    ASSERT_EQ(block.get_actual_cbs_size(), 14);

    std::vector<char> arr = {'a' - 'a', 'b' - 'a', 'c' - 'a', 'd' - 'a', 'e' - 'a',
                             'f' - 'a', 'g' - 'a', 'h' - 'a', 'i' - 'a', 'j' - 'a'};

    std::string archive_path = "/home/xunya/CLionProjects/labwork6/tests/ARCHIVE";
    clear_file(archive_path);

    block.make_hamming(arr);

    block.print_coded(archive_path);
    std::string str1 = block.coded_bit_string.to_string();

    BinaryBlock block_copy;
    block_copy.block_size = 10;

    std::ifstream in(archive_path, std::ifstream::binary);

    if (!in.is_open()) {
        throw std::invalid_argument("Failed! Can't open input file!");
    }

    std::vector<char> v;

    while (in.good()) {
        char* buffer = new char[10];
        in.read(buffer, 10);

        for (size_t i = 0; i < in.gcount(); ++i) {
            v.push_back(buffer[i]);
        }

        delete[] buffer;
    }

    in.close();

    block_copy.decode_hamming(v);

    std::string str2 = block_copy.coded_bit_string.to_string();

    ASSERT_EQ(str1, str2);
}

TEST(FileSuit, FileSetDecoded) {
    // block,
    BinaryBlock block;
    block.block_size = 10;
    std::vector<char> arr = {'a' - 'a', 'b' - 'a', 'c' - 'a', 'd' - 'a', 'e' - 'a',
                             'f' - 'a', 'g' - 'a', 'h' - 'a', 'i' - 'a', 'j' - 'a'};

    // print coded block to archive,
    std::string archive_path = "/home/xunya/CLionProjects/labwork6/tests/ARCHIVE";
    clear_file(archive_path);
    block.make_hamming(arr);
    block.print_coded(archive_path);

    // get block_copy by reading archive,
    BinaryBlock block_copy;
    block_copy.block_size = 10;

    std::ifstream in(archive_path, std::ifstream::binary);
    if (!in.is_open()) {
        throw std::invalid_argument("Failed! Can't open input file!");
    }

    std::vector<char> v_coded;

    while (in.good()) {
        char* buffer = new char[10];
        in.read(buffer, 10);

        for (size_t i = 0; i < in.gcount(); ++i) {
            v_coded.push_back(buffer[i]);
        }

        delete[] buffer;
    }

    in.close();

    // set decoded block_copy to file2,
    block_copy.decode_hamming(v_coded);

    std::string extract_path = "/home/xunya/CLionProjects/labwork6/tests/FILE2";
    clear_file(extract_path);
    block_copy.print_decoded(extract_path);

    // read file2 and equality with block data
    // very correct: 10010000000100001110000001100000101000000010000011000000010000001000000000000000
    std::ifstream in_coded(extract_path, std::ifstream::binary);

    if (!in_coded.is_open()) {
        throw std::invalid_argument("Failed! Can't open input file!");
    }

    std::vector<char> v_decoded;

    while (in_coded.good()) {
        char* buffer = new char[10];
        in_coded.read(buffer, 10);

        for (size_t i = 0; i < in_coded.gcount(); ++i) {
            v_decoded.push_back(buffer[i]);
        }

        delete[] buffer;
    }

    in_coded.close();

    for (size_t i = 0; i < v_decoded.size(); ++i) {
        ASSERT_EQ(arr[i], v_decoded[i]);
    }
}

void get_file_chars(const std::string file_path, std::vector<char>& v) {
    std::ifstream in(file_path, std::ifstream::binary);
    if (!in.is_open()) {
        throw std::invalid_argument("Failed! Can't open input file!");
    }

    while (in.good()) {
        char* buffer = new char[10];
        in.read(buffer, 10);

        for (size_t i = 0; i < in.gcount(); ++i) {
            v.push_back(buffer[i]);
        }

        delete[] buffer;
    }

    in.close();
}

TEST(FileSuit, FileSetDecodedPlus) {
    // First argument
    std::string file1_path = "/home/xunya/CLionProjects/labwork6/tests/FILE1";
    clear_file(file1_path);
    make_test_file(file1_path);
    File file1(file1_path);

    std::vector<char> file1_chars;
    get_file_chars(file1_path, file1_chars);

    char temp_char = 'a' - 'a';
    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(temp_char + i, file1_chars[i]);
    }

    // Second argument
    std::string archive_path = "/home/xunya/CLionProjects/labwork6/tests/ARCHIVE";
    std::string extract_file1_path = "/home/xunya/CLionProjects/labwork6/tests/EXTRACT_FILE1";
    clear_file(archive_path);
    clear_file(extract_file1_path);

    file1.set_file(archive_path);
    file1.get_file(archive_path, 0, 14, extract_file1_path);

    std::vector<char> extract_file_chars;
    get_file_chars(extract_file1_path, extract_file_chars);

    // Check equality
    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(extract_file_chars[i], file1_chars[i]);
    }
}

TEST(ArchiveSuit, AddFile) {
    std::string file1_path = "/home/xunya/CLionProjects/labwork6/tests/FILE1";
    clear_file(file1_path);

    Archive archive;
    archive.add_file(file1_path);

    ASSERT_EQ(archive.file_list[file1_path].file_name, file1_path);
}

TEST(ArchiveSuit, GetList) {
    std::string file1_path = "/home/xunya/CLionProjects/labwork6/tests/FILE1";
    clear_file(file1_path);

    std::string file2_path = "/home/xunya/CLionProjects/labwork6/tests/FILE2";
    clear_file(file2_path);

    std::string file3_path = "/home/xunya/CLionProjects/labwork6/tests/FILE3";
    clear_file(file3_path);

    Archive archive;
    archive.add_file(file1_path);
    archive.add_file(file2_path);
    archive.add_file(file3_path);

    std::vector<std::string> copy = archive.get_list();

    ASSERT_EQ(copy[0], file1_path);
    ASSERT_EQ(copy[1], file2_path);
    ASSERT_EQ(copy[2], file3_path);
}

TEST(ArchiveSuit, DeleteFile) {
    std::string file1_path = "/home/xunya/CLionProjects/labwork6/tests/FILE1";
    clear_file(file1_path);

    std::string file2_path = "/home/xunya/CLionProjects/labwork6/tests/FILE2";
    clear_file(file2_path);

    std::string file3_path = "/home/xunya/CLionProjects/labwork6/tests/FILE3";
    clear_file(file3_path);

    std::string archive_path = "/home/xunya/CLionProjects/labwork6/tests/ARCHIVE";
    clear_file(archive_path);
    Archive archive;
    archive.set_short_archive_path("ARCHIVE");
    archive.set_full_archive_path("/home/xunya/CLionProjects/labwork6/tests");

    archive.add_file(file1_path);
    archive.add_file(file2_path);
    archive.add_file(file3_path);
    archive.delete_file(file3_path);

    std::vector<std::string> copy = archive.get_list();

    ASSERT_EQ(copy.size(), 2);
    ASSERT_EQ(copy[0], file1_path);
    ASSERT_EQ(copy[1], file2_path);
}

void make_failure(const std::string& file_path) {
    std::vector<char> v;
    get_file_chars(file_path, v);

    char copy = v[0];
    v[0] = ((copy >> 1) << 1) + !(copy & 1);

    clear_file(file_path);

    std::ofstream out(file_path, std::ios::app);

    if (!out.is_open()) {
        throw std::invalid_argument("Path does not exist!");
    }

    for (auto el: v) {
        out << el;
    }

    out.close();
}

TEST(FileSuit, CheckWithFailures) {
    // First argument
    std::string file1_path = "/home/xunya/CLionProjects/labwork6/tests/FILE1";
    clear_file(file1_path);
    make_test_file(file1_path);
    File file1(file1_path);

    std::vector<char> file1_chars;
    get_file_chars(file1_path, file1_chars);

    char temp_char = 'a' - 'a';
    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(temp_char + i, file1_chars[i]);
    }

    // Second argument
    std::string archive_path = "/home/xunya/CLionProjects/labwork6/tests/ARCHIVE";
    std::string extract_file1_path = "/home/xunya/CLionProjects/labwork6/tests/EXTRACT_FILE1";
    clear_file(archive_path);
    clear_file(extract_file1_path);

    file1.set_file(archive_path);
    make_failure(archive_path);
    file1.get_file(archive_path, 0, 14, extract_file1_path);

    std::vector<char> extract_file_chars;
    get_file_chars(extract_file1_path, extract_file_chars);

    // Check equality
    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(extract_file_chars[i], file1_chars[i]);
    }
}
