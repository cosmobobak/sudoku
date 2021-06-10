#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "sudoku.hpp"

int main(int argc, char* argv[]) {
    int max_sudokus_processed = 100;
    if (argc > 1) {
        max_sudokus_processed = atoi(argv[1]);
    }

    assert(max_sudokus_processed > 0);
    
    // Create an input filestream
    std::ifstream sudokus("benchmark_set.txt");

    // Make sure the file is open
    if (!sudokus.is_open()) throw std::runtime_error("Could not open file");

    SudokuBoard driver;

    std::string easiest_sudoku, hardest_sudoku;
    long long min_time = std::numeric_limits<long long>::max(), max_time = 0;

    std::string line;
    // Read data, line by line
    int count = 0;
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

        std::cout << std::right << std::setprecision(2) << ((double)count / (double)max_sudokus_processed * 100.0) << "% done.\r";
        std::cout << line << " solved in " << std::right << std::setw(6) << time << "μs." << std::endl;
        if (count++ == max_sudokus_processed) break;
    }
    std::cout << std::endl;
    std::cout << "hardest sudoku " << hardest_sudoku << " took " << std::right << std::setw(6) << max_time << "μs." << std::endl;
    std::cout << "easiest sudoku " << easiest_sudoku << " took " << std::right << std::setw(6) << min_time << "μs." << std::endl;
    return 0;
}
