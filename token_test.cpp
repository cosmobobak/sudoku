#include <array>
#include <iostream>
#include <string>
#include <vector>
#include <utility>

#include "sudoku.hpp"

int main() {
    std::vector<std::pair<std::string, bool>> strings = {
        { "asdjbj", false },
        { "---------", true },
        { "-1-2-3-4-", true },
        { "-1-2-3-4-5-6-7-8-9-", true },
        { "-1-2-3-4-5a-6-7-8-9-", false },
        { "-1-2-3-4-5-6-7-8-9-a-", false },
        { "", true },
        { "123456789", true },
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
