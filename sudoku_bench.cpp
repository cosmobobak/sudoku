#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "sudoku.hpp"

int main() {
    // Create an input filestream
    std::ifstream sudokus("sudokus.txt");

    // Make sure the file is open
    if (!sudokus.is_open()) throw std::runtime_error("Could not open file");

    SudokuBoard driver;

    std::string line;
    // Read data, line by line
    while (std::getline(sudokus, line)) {
        driver.set_state(line);
        *(line.end() - 1) = ' ';

        auto start = std::chrono::system_clock::now();
        driver.solve_dfs();
        auto end = std::chrono::system_clock::now();

        auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        std::cout << line << "solved in " << std::right << std::setw(5) << time << " microseconds." << std::endl;
    }
    return 0;
}
