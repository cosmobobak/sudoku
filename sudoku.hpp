#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <string>
#include <sstream>
#include <cassert>

#include "sudokuiterators.hpp"

class SudokuBoard {
    static constexpr auto UNASSIGNED = 0;
    static constexpr auto ROW = 0;
    static constexpr auto COL = 1;
    static constexpr auto BOX = 2;
    static constexpr std::array<char, 10> symbols = {'.', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    static constexpr std::array<char, 10> valid_tokens = {'-', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

    std::array<std::array<int, 9>, 9> state = {0};

   public:
    SudokuBoard(std::string &in) {
        int x = 0;
        for (char &c : in) {
            if (c != '-') {
                state[x / 9][x % 9] = c - '0';  // converts char -> int
            }
            x++;
        }
    }

    static auto is_string_valid(std::string &str) {
        // checks for strings of the form "2736-13-12---346"
        return std::all_of(
            str.begin(),
            str.end(),
            [](char c) { return std::any_of(
                             valid_tokens.begin(),
                             valid_tokens.end(),
                             [c](char token) { return token == c; }); });
    }

    void show() const {
        std::stringstream sb;
        std::string divider = "├───────┼───────┼───────┤\n";
        std::string top = "┌───────┬───────┬───────┐\n";
        std::string bottom = "└───────┴───────┴───────┘\n";
        sb << "\n";
        sb << top;
        for (size_t y = 0; y < 9; y++) {
            sb << "│ ";
            for (size_t x = 0; x < 9; x++) {
                sb << symbols[state[y][x]] << " ";
                if (x % 3 == 2 && x != 8) sb << "│ ";
            }
            sb << "│\n";
            if (y % 3 == 2 && y != 8) sb << divider;
        }
        sb << bottom;

        std::cout << sb.str();
    }

    auto first_unassigned() const -> int {
        int x = 0;
        while (x < 81) {
            if (state[x / 9][x % 9] == 0) {
                return x;
            }
            x++;
        }
        return x;
    }

    auto get_num_at_position(int x) const -> int {
        return state[x / 9][x % 9];
    }

    auto current_state_invalid() -> bool {
        int n;
        for (int i = 0; i < 81; i++) {
            n = state[i / 9][i % 9];
            if (n) {
                state[i / 9][i % 9] = 0;
                if (!legal(i, n)) {
                    return true;
                }
                state[i / 9][i % 9] = n;
            }
        }
        return false;
    }

    auto legal(int x, int num) const -> bool {
        return std::all_of(
                   Iterator2D<ROW>::begin(state, x),
                   Iterator2D<ROW>::end(state, x),
                   [num](int n) { return n != num; }) &&
               std::all_of(
                   Iterator2D<COL>::begin(state, x),
                   Iterator2D<COL>::end(state, x),
                   [num](int n) { return n != num; }) &&
               std::all_of(
                   Iterator2D<BOX>::begin(state, x),
                   Iterator2D<BOX>::end(state, x),
                   [num](int n) { return n != num; });
    }

    auto solve() -> bool {
        int x = first_unassigned();
        // If there is no unassigned location, we are done
        if (x == 81) {
            return true;  // success!
        }
        for (int num = 1; num <= 9; num++) {
            if (legal(x, num)) {
                state[x / 9][x % 9] = num;
                if (solve()) {
                    return true;
                }
                state[x / 9][x % 9] = UNASSIGNED;
            }
        }
        return false;  // this triggers backtracking
    }
};