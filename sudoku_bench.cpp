#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "sudoku.hpp"

int main() {
    // Create an input filestream
    std::ifstream sudokus("benchmark_set.txt");

    // Make sure the file is open
    if (!sudokus.is_open()) throw std::runtime_error("Could not open file");

    SudokuBoard driver;

    std::string easiest_sudoku, hardest_sudoku;
    long long min_time = std::numeric_limits<long long>::max(), max_time = 0;

    std::string line;
    // Read data, line by line
    while (std::getline(sudokus, line)) {
        line.resize(81);
        std::for_each(line.begin(), line.end(), [](auto& c){ 
            if (c == '.') {
                c = '-';
            }
        });
        driver.set_state(line);

        auto start = std::chrono::system_clock::now();
        driver.solve_dfs();
        auto end = std::chrono::system_clock::now();

        auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        if (time > max_time) {
            max_time = time;
            hardest_sudoku = line;
        }

        if (time < min_time) {
            min_time = time;
            easiest_sudoku = line;
        }

        std::cout << line << " solved in " << std::right << std::setw(6) << time << "μs." << std::endl;
    }
    std::cout << std::endl;
    std::cout << "hardest sudoku " << hardest_sudoku << " took " << std::right << std::setw(6) << max_time << "μs." << std::endl;
    std::cout << "easiest sudoku " << easiest_sudoku << " took " << std::right << std::setw(6) << min_time << "μs." << std::endl;
    return 0;
}
