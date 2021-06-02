#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "sudoku.hpp"

int main() {
    // Create an input filestream
    std::ifstream sudokus("test_set.txt");
    std::ifstream answers("answer_set.txt");

    // Make sure the file is open
    if (!sudokus.is_open()) throw std::runtime_error("Could not open file");

    SudokuBoard driver;

    std::string line;
    std::string answer;
    // Read data, line by line
    while (std::getline(sudokus, line)) {
        std::getline(answers, answer);
        line.pop_back();
        answer.pop_back();

        driver.set_state(line);
        driver.solve_dfs();

        if (answer.compare(driver.to_string()) == 0) {
            std::cout << line << " PASS\n";
        } else {
            std::cout << "\n"
                      << line << "\n";
            std::cout << answer << "\n";
            std::cout << driver.to_string() << "\n";
            std::cerr << "FAIL\n";
        }
    }
    return 0;
}
