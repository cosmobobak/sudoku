#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "sudoku.hpp"
#include "fastfile.hpp"

const char* BENCHMARK_FILENAME = "benchmark_set.txt";
constexpr auto SUDOKU_LINE_LEN = 81;

int main(int argc, char* argv[]) {
    int max_sudokus_processed = fast_count_lines(BENCHMARK_FILENAME) - 1;
    
    if (argc > 1) {
        max_sudokus_processed = atoi(argv[1]);
    }

    assert(max_sudokus_processed > 0);

    // Create an input filestream
    std::ifstream sudokus(BENCHMARK_FILENAME);

    // Make sure the file is open
    if (!sudokus.is_open()) throw std::runtime_error("Could not open file");

    SudokuBoard driver;

    std::string easiest_sudoku, hardest_sudoku;
    long long min_time = std::numeric_limits<long long>::max(), max_time = 0;

    int count = 0;
    std::string line;

    // time the whole execution
    auto global_start = std::chrono::system_clock::now();

    // Read data, line by line
    while (std::getline(sudokus, line)) {
        printf("%d out of %d\r", count, max_sudokus_processed);
        fflush(stdout);

        std::replace(line.begin(), line.end(), '.', '-');
        driver.set_state(line);

        auto start = std::chrono::system_clock::now();
        driver.solve_preproc_dfs();
        auto end = std::chrono::system_clock::now();

        auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        if (time > max_time) {
            max_time = time;
            hardest_sudoku = line.substr(0, SUDOKU_LINE_LEN);
        }

        if (time < min_time) {
            min_time = time;
            easiest_sudoku = line.substr(0, SUDOKU_LINE_LEN);
        }

        // std::cout << std::right << std::setprecision(2) << ((double)count / (double)max_sudokus_processed * 100.0) << "% done.\r";
        // std::cout << line << " solved in " << std::right << std::setw(6) << time << "μs." << std::endl;
        if (count == max_sudokus_processed) break;
        count++;
    }
    printf("%d out of %d\n", count, max_sudokus_processed);

    auto global_end = std::chrono::system_clock::now();

    auto global_time = std::chrono::duration_cast<std::chrono::microseconds>(global_end - global_start).count();

    std::cout << std::endl
              << "hardest sudoku "
              << hardest_sudoku << " took "
              << std::right << std::setw(6) << max_time << "μs." << std::endl
              << "easiest sudoku "
              << easiest_sudoku << " took "
              << std::right << std::setw(6) << min_time << "μs." << std::endl
              << "total time: " 
              << std::right << std::setw(6) << global_time << "μs." << std::endl;
    return 0;
}
