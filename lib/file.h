#ifndef LABWORK6_FILE_H
#define LABWORK6_FILE_H

#include <bitset>
#include <string>
#include <vector>
#include <ostream>

class BinaryBlock {
public:
    size_t block_size{}; // in bytes

    const std::bitset<14> r0 = 0b10101010101010;
    const std::bitset<14> r1 = 0b01100110011001;
    const std::bitset<14> r2 = 0b00011110000111;
    const std::bitset<14> r3 = 0b00000001111111;

    std::bitset<10 * 8> bit_string;

    std::bitset<14 * 8> coded_bit_string;

    void invariant() const;

    void set_bit_string();

    void set_bit_string(const std::vector<char>& v);

    void set_coded_bit_string();

    void set_coded_bit_string(const std::vector<char>& v);

    void fix_bit_string(size_t pos, size_t here_pos);

    size_t get_control_size() const;

    std::bitset<4> get_vector_matrix(size_t pos);

    BinaryBlock() = default;

    explicit BinaryBlock(size_t block_size) : block_size(block_size) {}

    void make_hamming(const std::vector<char>& v);

    void decode_hamming(const std::vector<char>& v);

    void print_coded(const std::string& archive_path);

    void print_decoded(const std::string& extract_path);

    size_t get_actual_cbs_size() const;
};

class File {
public:
    size_t coded_file_size = 0;

    std::string file_name;

    static void read_before(size_t begin, std::ifstream& in);

    File() = default;

    explicit File(const std::string& file_name);

    void set_file(const std::string& archive_path);

    static void get_file(const std::string& archive_path, size_t begin, size_t end,
                         const std::string& extract_path);

    [[nodiscard]] size_t get_size() const;
};

#endif //LABWORK6_FILE_H
