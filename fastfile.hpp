#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <cstddef>

static constexpr auto BLOCK_SIZE = 1024 * 1024;

auto fileread(std::istream& is, char* data, size_t size) -> uint32_t {
    is.read(data, size);
    return is.gcount();
}

auto countlines(const char* data, int size) -> uint32_t {
    return std::count(data, data + size, '\n');
}

auto fast_count_lines(const char* file_name) -> int {
    std::vector<char> buff(BLOCK_SIZE);
    std::ifstream file_stream(file_name);
    int n = 0;
    int cc;
    while ((cc = fileread(file_stream, buff.data(), buff.size()))) {
        n += countlines(buff.data(), cc);
    }
    return n;
}