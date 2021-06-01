#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "dlxnode.hpp"
#include "sudokuiterators.hpp"

class SudokuBoard {
    static constexpr auto UNASSIGNED = 0;
    static constexpr auto ROW = 0;
    static constexpr auto COL = 1;
    static constexpr auto BOX = 2;
    static constexpr auto GLOBAL = 3;
    static constexpr std::array<char, 10> symbols = {'.', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    static constexpr std::array<char, 10> valid_tokens = {'-', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

    std::array<std::array<int, 9>, 9> state = {0};

   public:
    SudokuBoard() {
        clear();
    }

    SudokuBoard(const std::string &in) {
        set_state(in);
    }

    void clear() {
        std::fill(
            Iterator2D<GLOBAL>::begin(state),
            Iterator2D<GLOBAL>::end(state),
            0);
    }

    void set_state(const std::string& in) {
        clear();
        auto slot = Iterator2D<GLOBAL>::begin(state);
        auto end = Iterator2D<GLOBAL>::end(state);

        for (const auto& c : in) {
            *slot = c != '-' ? c - '0' : 0;  // converts char -> int

            if (slot++ == end) break;
        }
    }

    auto to_string() {
        std::string out;
        out.resize(81);
        int idx = 0;
        std::for_each(
            Iterator2D<GLOBAL>::begin(state),
            Iterator2D<GLOBAL>::end(state),
            [&out, &idx](int val) { 
                out[idx++] = val ? '0' + val : '-'; 
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
        std::reverse(Iterator2D<GLOBAL>::begin(state), Iterator2D<GLOBAL>::end(state));
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

    auto legal(const Iterator2D<GLOBAL> &test_position, int num) -> bool {
        return std::all_of(
                   Iterator2D<ROW>::begin(state, test_position),
                   Iterator2D<ROW>::end(state, test_position),
                   [num](int n) { return n != num; }) &&
               std::all_of(
                   Iterator2D<COL>::begin(state, test_position),
                   Iterator2D<COL>::end(state, test_position),
                   [num](int n) { return n != num; }) &&
               std::all_of(
                   Iterator2D<BOX>::begin(state, test_position),
                   Iterator2D<BOX>::end(state, test_position),
                   [num](int n) { return n != num; });
    }

    auto search_dfs(Iterator2D<GLOBAL> last_zero_pos) -> bool {
        auto end_pos = Iterator2D<GLOBAL>::end(state);
        auto zero_pos = std::find(last_zero_pos, end_pos, 0);

        // If there is no unassigned location, we are done
        if (zero_pos == end_pos) {
            return true;  // success!
        }

        for (int num = 1; num <= 9; num++) {
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
        auto start = Iterator2D<GLOBAL>::begin(state);
        auto middle = Iterator2D<GLOBAL>::begin(state);
        std::advance(middle, 41);
        auto end = Iterator2D<GLOBAL>::end(state);

        auto topcount = std::count_if(start, middle, [](int x) { return x != 0; });
        auto bottomcount = std::count_if(middle, end, [](int x) { return x != 0; });

        bool transposed = false;
        if (bottomcount > topcount) {
            transpose();
            transposed = true;
        }
        auto result = search_dfs(Iterator2D<GLOBAL>::begin(state));
        if (transposed) {
            transpose();
        }
        return result;
    }

    // auto dlx_search(DLX::Column* h, int k, list s) {
    //     if (h->r == h) {
    //         // print_solution( s );
    //         return;
    //     }

    //     auto c = choose_column_object(h);
    //     auto r = D[c];
    //     while (r != c) {
    //         s = s + [r];
    //         auto j = R[r];
    //         while (j != r) {
    //             cover(C[j]);
    //             j = R[j];
    //         }
    //         dlx_search(h, k + 1, s);
    //         // Pop data object
    //         r = sk;
    //         c = C[r];
    //         j = L[r];
    //         while (j != r) {
    //             uncover(C[j]);
    //             j = L[j];
    //         }
    //         r = D[r];
    //     }
    //     uncover(c);
    //     return;
    // }

    // auto solve_dlx() {
    //     auto size = 1;
    //     // ALL THE NODES GO IN HERE
    //     auto nodes = std::vector<DLX::Node>(size);

    // }
};