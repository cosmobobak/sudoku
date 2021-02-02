#include <array>
#include <cassert>
#include <chrono>
#include <iostream>
#include <string>

constexpr auto UNASSIGNED = 0;
constexpr std::array<char, 10> symbols = {
    '.',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
};

class SudokuBoard {
   public:
    std::array<std::array<int, 9>, 9> state = {0};

    SudokuBoard(std::string &in) {
        int x = 0;
        for (char &c : in) {
            if (c != '-') {
                state[x / 9][x % 9] = c - '0';  // converts char -> int
            }
            x++;
        }
    }

    void show() {
        for (auto &row : state) {
            for (auto &num : row) {
                std::cout << symbols[num] << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    auto first_unassigned() -> int {
        int x = 0;
        while (x < 81) {
            if (state[x / 9][x % 9] == 0) {
                return x;
            }
            x++;
        }
        return x;
    }

    auto rowview(int x) -> std::array<int, 9> {
        std::array<int, 9> out;
        for (int i = 0; i < 9; i++) {
            out[i] = state[x / 9][i];
        }
        return out;
    }

    auto colview(int x) -> std::array<int, 9> {
        std::array<int, 9> out;
        for (int i = 0; i < 9; i++) {
            out[i] = state[i][x % 9];
        }
        return out;
    }

    auto boxview(int x) -> std::array<int, 9> {
        int rstart = ((x / 9) / 3) * 3;
        int cstart = ((x % 9) / 3) * 3;
        std::array<int, 9> out;
        for (int row = rstart; row < 3 + rstart; row++) {
            for (int col = cstart; col < 3 + cstart; col++) {
                out[(row - rstart) * 3 + (col - cstart)] = state[row][col];
            }
        }
        return out;
    }

    auto legal(int x, int num) {
        auto v1 = rowview(x);
        auto v2 = colview(x);
        auto v3 = boxview(x);
        return std::all_of(
                   v1.begin(),
                   v1.end(),
                   [num](int n) { return n != num; }) &&
               std::all_of(
                   v2.begin(),
                   v2.end(),
                   [num](int n) { return n != num; }) &&
               std::all_of(
                   v3.begin(),
                   v3.end(),
                   [num](int n) { return n != num; });
    }

    auto solve() -> bool {
        int x = first_unassigned();
        // If there is no unassigned location, we are done
        if (x == 81)
            return true;  // success!
        // consider digits 1 to 9
        for (int num = 1; num <= 9; num++) {
            // if looks promising
            if (legal(x, num)) {
                // make tentative assignment
                state[x / 9][x % 9] = num;
                // return, if success, yay!
                if (solve())
                    return true;
                // failure, unmake & try again
                state[x / 9][x % 9] = UNASSIGNED;
            }
        }
        return false;  // this triggers backtracking
    }
};

auto main(int argc, char *argv[]) -> int {
    std::string in = std::string(argv[1]);
    SudokuBoard b = SudokuBoard(in);
    b.show();
    auto start = std::chrono::steady_clock::now();
    b.solve();
    std::cout << "solved in " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << "ms\n";
    b.show();
    return 0;
}
