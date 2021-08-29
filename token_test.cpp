#include <array>
#include <iostream>
#include <string>
#include <vector>
#include <utility>

#include "sudoku.hpp"

int main() {
    std::vector<std::pair<std::string, bool>> strings = {
        std::make_pair("asdjbj", false),
        std::make_pair("---------", true),
        std::make_pair("-1-2-3-4-", true),
        std::make_pair("-1-2-3-4-5-6-7-8-9-", true),
        std::make_pair("-1-2-3-4-5a-6-7-8-9-", false),
        std::make_pair("-1-2-3-4-5-6-7-8-9-a-", false),
        std::make_pair("", true),
        std::make_pair("123456789", true),
    };

    for (auto [str, expected] : strings) {
        auto result = SudokuBoard::is_string_valid(str);
        if (result != expected) {
            printf("FAIL: %s, Expected: %d, Got: %d\n", str.c_str(), expected, result);
        } else {
            printf("PASS: %s\n", str.c_str());
        }
    }
    return 0;
}
