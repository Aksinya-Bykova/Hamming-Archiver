#include <fstream>
#include <iostream>
#include "file.h"

size_t File::get_size() const {
    return coded_file_size;
}

File::File(const std::string& file_name) : coded_file_size(0), file_name(file_name) {}

/**
 * Coding binary file with Hamming algorithm
 * Binary file is separated by 10-bytes blocks
 * (But last block can be less)
 * [ ][ ][1][ ][2][3][4][ ][5][6][7][8][9][10]
   |1| 2| 3| 4| 5| 6| 7| 8| 9|10|11|12|13| 14|
 * Then every 12-bits are coded with Hamming
 *
 * If last block is less than 10 (for example 3),
 * for this mini-blocks are smaller than 16 (for 3 it's 6)
 */
void File::set_file(const std::string& archive_path) {
    std::ifstream in(file_name, std::ifstream::binary);

    if (!in.is_open()) {
        throw std::invalid_argument("Failed! Can't open input file!");
    }

    while (in.good()) {
        char* buffer = new char[10];
        in.read(buffer, 10);

        std::vector<char> v;

        for (size_t i = 0; i < in.gcount(); ++i) {
            v.push_back(buffer[i]);
        }

        delete[] buffer;

        if (!v.empty()) {
            BinaryBlock block(v.size());
            block.make_hamming(v);
            coded_file_size += block.get_actual_cbs_size();
            block.print_coded(archive_path);
        }
    }

    in.close();
}

void File::get_file(const std::string& archive_path, size_t begin, size_t end, const std::string& extract_path) {
    std::ifstream in(archive_path, std::ifstream::binary);
    if (!in.is_open()) {
        throw std::invalid_argument("Failed! Can't open input file!");
    }

    if (begin != 0) {
        read_before(begin, in);
    }

    size_t len = end - begin;
    size_t count = 0;

    while (in.good()) {
        char* buffer = new char[14];
        in.read(buffer, 14);
        std::vector<char> v;

        for (size_t i = 0; i < in.gcount() && (count != len); ++i) {
            v.push_back(buffer[i]);
            ++count;
        }

        delete[] buffer;

        if (!v.empty()) {
            BinaryBlock block(10);
            block.decode_hamming(v);
            block.print_decoded(extract_path);
        }
    }

    if (count != len) {
        throw std::invalid_argument("Invalid file!");
    }

    in.close();
}

void File::read_before(size_t begin, std::ifstream& in) {
    size_t len = 0;

    while (in.good() && (len < begin)) {
        ++len;
    }

    if (len != begin - 1) {
        throw std::invalid_argument("Failed! Failed to read input file!");
    }
}

std::bitset<4> BinaryBlock::get_vector_matrix(size_t pos) {
    std::bitset<4> nums = 0b0000;

    for (size_t i = 0; i < get_actual_cbs_size(); ++i) {
        nums[0] = nums[0] ^ (r0[get_actual_cbs_size() - i - 1] & coded_bit_string[get_actual_cbs_size() * pos + i]);
    }

    for (size_t i = 0; i < get_actual_cbs_size(); ++i) {
        nums[1] = nums[1] ^ (r1[get_actual_cbs_size() - i - 1] & coded_bit_string[get_actual_cbs_size() * pos + i]);
    }

    for (size_t i = 0; i < get_actual_cbs_size(); ++i) {
        nums[2] = nums[2] ^ (r2[get_actual_cbs_size() - i - 1] & coded_bit_string[get_actual_cbs_size() * pos + i]);
    }

    for (size_t i = 0; i < get_actual_cbs_size(); ++i) {
        nums[3] = nums[3] ^ (r3[get_actual_cbs_size() - i - 1] & coded_bit_string[get_actual_cbs_size() * pos + i]);
    }

    return nums;
}

void BinaryBlock::make_hamming(const std::vector<char>& v) {
    set_bit_string(v);
    set_coded_bit_string();

    for (size_t pos = 0; pos < 8; ++pos) {
        std::bitset<4> r_nums = get_vector_matrix(pos);

        size_t bin_pos = 1;
        size_t count = 0;

        for (size_t i = 0; i < get_actual_cbs_size(); ++i) {
            if (i == bin_pos - 1) {
                coded_bit_string[get_actual_cbs_size() * pos + i] = r_nums[count];
                ++count;
                bin_pos *= 2;
            }
        }
    }
}

void BinaryBlock::invariant() const {
    if (block_size > 10 || block_size <= 0) {
        throw std::invalid_argument("Incorrect size of block!");
    }
}

void BinaryBlock::set_bit_string(const std::vector<char>& v) {
    invariant();

    if (v.size() != block_size) {
        throw std::invalid_argument("Incorrect size of block in vector!");
    }

    for (size_t i = 0; i < v.size(); ++i) {
        for (size_t j = 0; j < 8; ++j) {
            bit_string[i * 8 + j] = (v[i] >> (8 - j - 1)) % 2;
        }
    }
}

size_t BinaryBlock::get_control_size() const {
    invariant();

    if (block_size >= 5 && block_size <= 10) {
        return 4;
    }
    if (block_size >= 2 && block_size <= 4) {
        return 3;
    }

    return 2;
}

void BinaryBlock::set_coded_bit_string() {
    for (size_t pos = 0; pos < 8; ++pos) {
        size_t count = 0;

        for (size_t i = 0; i < get_actual_cbs_size(); ++i) {
            if (i == 0 || i == 1 || i == 3 || i == 7) {
                coded_bit_string[pos * get_actual_cbs_size() + i] = false;
            } else {
                coded_bit_string[pos * get_actual_cbs_size() + i] =
                        bit_string[pos * block_size + count];
                ++count;
            }
        }
    }
}

void BinaryBlock::print_coded(const std::string& archive_path) {
    std::ofstream out(archive_path, std::ios::app);

    if (!out.is_open()) {
        throw std::invalid_argument("Path does not exist!");
    }

    for (int i = get_actual_cbs_size() - 1; i >= 0; --i) {
        std::bitset<8> current_byte;

        for (int pos = 7; pos >= 0; --pos) {
            current_byte[pos] = coded_bit_string[i * 8 + pos];
        }
        out << static_cast<char> (current_byte.to_ulong());
    }

    out.close();
}

size_t BinaryBlock::get_actual_cbs_size() const {
    return block_size + get_control_size();
}

void BinaryBlock::decode_hamming(const std::vector<char>& v) {
    set_coded_bit_string(v);

    for (size_t pos = 0; pos < 8; ++pos) {
        std::bitset<4> s_nums = get_vector_matrix(pos);

        if (s_nums.to_ulong() > get_actual_cbs_size()) {
            throw std::invalid_argument("Error! Can't fix file!");
        }

        if (s_nums.to_ulong() != 0) {
            fix_bit_string(pos, s_nums.to_ulong());
        }
    }
}

void BinaryBlock::fix_bit_string(size_t pos, size_t here_pos) {
    size_t actual_pos = pos * 14 + here_pos;
    coded_bit_string[actual_pos - 1] = !(coded_bit_string[actual_pos - 1]);
}

void BinaryBlock::set_coded_bit_string(const std::vector<char>& v) {
    for (size_t i = 0; i < get_actual_cbs_size(); ++i) {
        for (size_t pos = 0; pos < 8; ++pos) {
            coded_bit_string[i * 8 + pos] = (v[get_actual_cbs_size() - i - 1] >> (pos)) % 2;
        }
    }
}

void BinaryBlock::print_decoded(const std::string& extract_path) {
    set_bit_string();

    std::ofstream out(extract_path, std::ios::app);
    if (!out.is_open()) {
        throw std::invalid_argument("Path does not exist!");
    }

    for (size_t i = 0; i < block_size; ++i) {
        std::bitset<8> current_byte;

        for (size_t pos = 0; pos < 8; ++pos) {
            current_byte[pos] = bit_string[i * 8 + (8 - pos - 1)];
        }
        out << static_cast<char> (current_byte.to_ulong());
    }

    out.close();
}

void BinaryBlock::set_bit_string() {
    for (size_t pos = 0; pos < 8; ++pos) {
        size_t count = 0;

        for (size_t i = 0; i < get_actual_cbs_size(); ++i) {
            if (!(i == 0 || i == 1 || i == 3 || i == 7)) {
                bit_string[pos * block_size + count] =
                        coded_bit_string[pos * get_actual_cbs_size() + i];
                ++count;
            }
        }
    }
}
