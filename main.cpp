#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <string>

#include "sudoku.hpp"

// easy: "----345----89---3-3----27892-4--6815----4----8765--4-27523----6-1---79----942----"
// medium: "---4-6-9------3--545-----866-2-74--1----9----9--56-7-871-----643--6------6-9-2---"
// hard: "9-3--42--4-65-------28----------5--4-67-4-92-1--9----------87-------94-3--83--6-1"
// evil: "--9------384---5------4-3-----1--27-2--3-4--5-48--6-----6-1------7---629-----5---"
// adversarial: "--------------3-85--1-2-------5-7-----4---1---9-------5------73--2-1--------4---9"

auto main(int argc, char *argv[]) -> int {
    // check if we haven't been given any arguments
    if (argc <= 1) {
        std::cout << "no input string provided.\n";
        return 0;
    }
    // create a std::string from which the board is initialised
    std::string in = std::string(argv[1]);
    // verify that all the characters in the string are valid, exit early if not
    if (!SudokuBoard::is_string_valid(in)) {
        std::cout << "input string invalid (you may only use digits and dashes in your input).\n";
        return 0;
    }
    // object is created.
    SudokuBoard b = SudokuBoard(in);

    // show the user their initial board, to confirm to
    // them that they have entered the correct CLI string
    std::cout << "Your sudoku:";
    b.show();

    // check if the given sudoku is legal as-is, exit early if not
    if (b.current_state_invalid()) {
        std::cout << "input sudoku invalid (given problem has repeated digits in rows, columns, or squares).\n";
        return 0;
    }

    // a timer that tracks how long we take to solve the problem
    auto start = std::chrono::system_clock::now();

    // solve() both mutates the board to a solved state,
    // and returns a flag that indicates if it was successful
    bool success = b.solve_dfs();

    // if the solve was unsuccessful, then the given sudoku was bad, and we exit early
    if (!success) {
        std::cout << "overconstrained sudoku (there is no pattern of digits that can validly fill the given sudoku).\n";
        return 0;
    }

    // the time taken to solve
    auto end = std::chrono::system_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // show the solved sudoku and exit
    std::cout << "\nYour solved sudoku:";
    b.show();
    std::cout << "\nsolved in " << time << "μs!\n";
    return 0;
}
