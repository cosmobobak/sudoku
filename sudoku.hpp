#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <ranges>

#include "dlxnode.hpp"
#include "sudokuiterators.hpp"

using enum RangeType;

class SudokuBoard {
    static constexpr auto UNASSIGNED = 0;
    static constexpr std::array<char, 10> symbols = {'.', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    static constexpr std::array<char, 10> valid_tokens = {'-', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

    std::array<std::array<int, 9>, 9> state = {0};

   public:
    SudokuBoard() {
        clear();
    }

    template <typename CharContainer>
    SudokuBoard(const CharContainer& in) {
        set_state(in);
    }

    // begin iterator
    auto begin() -> Iterator2D<GLOBAL> {
        return Iterator2D<GLOBAL>::begin(state);
    }

    // end iterator
    auto end() -> Iterator2D<GLOBAL> {
        return Iterator2D<GLOBAL>::end(state);
    }

    void clear() {
        std::fill(begin(), end(), 0);
    }

    template <typename CharContainer>
    void set_state(const CharContainer& in) {
        auto chars = std::views::take(in, 81);
        std::transform(
            chars.begin(),
            chars.end(), 
            begin(),
            [](char c) { return c != '-' ? c - '0' : 0; });
    }

    auto to_string() -> std::string {
        std::string out;
        std::transform(begin(), end(), std::back_inserter(out), [](int val) {
            return val ? '0' + val : '-';
        });
        return out;
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
        static const std::string divider = "├───────┼───────┼───────┤\n";
        static const std::string top = "┌───────┬───────┬───────┐\n";
        static const std::string bottom = "└───────┴───────┴───────┘\n";
        static const std::string bar = "│ ";
        static const std::string ascii_divider = "|-------|-------|-------|\n";
        static const std::string ascii_top = "|-------|-------|-------|\n";
        static const std::string ascii_bottom = "|-------|-------|-------|\n";
        static const std::string ascii_bar = "| ";

        std::stringstream sb;

        sb << "\n";
        sb << top;
        for (size_t y = 0; y < 9; y++) {
            sb << bar;
            for (size_t x = 0; x < 9; x++) {
                sb << symbols[state[y][x]] << " ";
                if (x % 3 == 2 && x != 8) sb << bar;
            }
            sb << bar << "\n";
            if (y % 3 == 2 && y != 8) sb << divider;
        }
        sb << bottom;

        std::cout << sb.str();
    }

    auto transpose() {
        std::reverse(begin(), end());
    }

    auto get_num_at_position(int x) const -> int {
        return state[x / 9][x % 9];
    }

    auto current_state_invalid() -> bool {
        for (
            auto val = Iterator2D<GLOBAL>::begin(state),
                 end = Iterator2D<GLOBAL>::end(state);
            val != end;
            ++val) {
            // assign n to the contents of the current index
            auto n = *val;
            // if n is nonzero, remove it, and check if having the original value would be legal.
            if (n) {
                *val = 0;
                if (!legal(val, n)) {
                    // clean our mess up before we leave
                    *val = n;
                    return true;
                }
                // reset position
                *val = n;
            }
        }
        return false;
    }

    auto legal(const Iterator2D<GLOBAL> &test_idx, int num) -> bool {
        return std::all_of(
                   Iterator2D<ROW>::begin(state, test_idx),
                   Iterator2D<ROW>::end(state, test_idx),
                   [num](int n) { return n != num; }) &&
               std::all_of(
                   Iterator2D<COL>::begin(state, test_idx),
                   Iterator2D<COL>::end(state, test_idx),
                   [num](int n) { return n != num; }) &&
               std::all_of(
                   Iterator2D<BOX>::begin(state, test_idx),
                   Iterator2D<BOX>::end(state, test_idx),
                   [num](int n) { return n != num; });
    }

    auto search_dfs(Iterator2D<GLOBAL> last_zero_pos) -> bool {
        auto end_pos = end();
        auto zero_pos = std::find(last_zero_pos, end_pos, 0);

        // If there is no unassigned location, we are done
        if (zero_pos == end_pos) {
            return true;  // success!
        }

        for (int num = 1; num <= 9; ++num) {
            if (legal(zero_pos, num)) {
                *zero_pos = num;
                if (search_dfs(zero_pos)) {
                    return true;
                }
                *zero_pos = UNASSIGNED;
            }
        }
        return false;  // this triggers backtracking
    }

    auto solve_dfs() -> bool {
        auto start_it = begin();
        auto middle_it = begin();
        std::advance(middle_it, 41);
        auto end_it = end();

        auto t_count = std::count_if(start_it, middle_it, std::identity{});
        auto b_count = std::count_if(middle_it, end_it, std::identity{});

        bool transposed = b_count > t_count;
        if (transposed) {
            transpose();
        }
        auto result = search_dfs(begin());
        if (transposed) {
            transpose();
        }
        return result;
    }

    auto fill_trivial_solutions() -> bool {
        // apply simple logical fill-ins of squares
        // i.e. if a square can only have one number, fill it with that number.
        // this is a preprocessing step to reduce the search space.
        auto start = begin();
        auto sent = end();
        auto change_made = false;
        for (auto it = start; it != sent; ++it) {
            if (*it) {
                continue;
            }
            int count = 0;
            int num;
            for (int attempt = 1; attempt <= 9 && count < 2; ++attempt) {
                if (legal(it, attempt)) {
                    ++count;
                    num = attempt;
                }
            }
            if (count == 1) {
                *it = num;
                change_made = true;
            }
        }
        // return whether we mutated the board at all
        return change_made;
    }

    auto solve_preproc_dfs() -> bool {
        while (fill_trivial_solutions());

        // drop into dfs
        return solve_dfs();
    }
};