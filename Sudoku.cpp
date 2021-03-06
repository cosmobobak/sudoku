#include <array>
#include <chrono>
#include <iostream>
#include <string>

constexpr auto UNASSIGNED = 0;
constexpr std::array<char, 10> symbols = {'.', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
constexpr std::array<char, 10> valid_tokens = {'-', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

class SudokuBoard {
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

    auto get_num_at_position(int x) -> int {
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

    auto legal(int x, int num) -> bool {
        auto row = rowview(x);
        auto col = colview(x);
        auto box = boxview(x);
        return std::all_of(
                   row.begin(),
                   row.end(),
                   [num](int n) { return n != num; }) &&
               std::all_of(
                   col.begin(),
                   col.end(),
                   [num](int n) { return n != num; }) &&
               std::all_of(
                   box.begin(),
                   box.end(),
                   [num](int n) { return n != num; });
    }

    auto solve() -> bool {
        int x = first_unassigned();
        // If there is no unassigned location, we are done
        if (x == 81)
            return true;  // success!
        for (int num = 1; num <= 9; num++) {
            if (legal(x, num)) {
                state[x / 9][x % 9] = num;
                if (solve())
                    return true;
                state[x / 9][x % 9] = UNASSIGNED;
            }
        }
        return false;  // this triggers backtracking
    }
};

auto main(int argc, char *argv[]) -> int {
    // check if we haven't been given any arguments
    if (argc <= 1) {
        std::cout << "no input string provided.";
        return 0;
    }
    // create a std::string from which the board is initialised
    std::string in = std::string(argv[1]);
    // verify that all the characters in the string are valid, exit early if not
    if (std::any_of(
        in.begin(), 
        in.end(), 
        [](char c){ return std::none_of(
            valid_tokens.begin(),
            valid_tokens.end(),
            [c](char token){ return token == c; }); })) {
        std::cout << "input string invalid (you may only use digits and dashes in your input).";
        return 0;
    }
    // class is created
    SudokuBoard b = SudokuBoard(in);

    // show the user their initial board, to confirm to 
    // them that they have entered the correct CLI string
    std::cout << "\nYour sudoku:\n";
    b.show();

    // check if the given sudoku is legal as-is, exit early if not
    if (b.current_state_invalid()) {
        std::cout << "input sudoku invalid (given problem has repeated digits in rows, columns, or squares).";
        return 0;
    }

    // a timer that tracks how long we take to solve the problem
    auto start = std::chrono::system_clock::now();

    // solve() both mutates the board to a solved state,
    // and returns a flag that indicates if it was successful
    bool success = b.solve();

    // if the solve was unsuccessful, then the given sudoku was bad, and we exit early
    if (!success) {
        std::cout << "overconstrained sudoku (there is no pattern of digits that can validly fill the given sudoku).";
        return 0;
    }

    // the time taken to solve
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();

    // show the solved sudoku and exit
    std::cout << "Your solved sudoku:\n";
    b.show();
    std::cout << "\nsolved in " << time << "ms!\n";
    return 0;
}
