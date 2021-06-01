# The Iridium Sudoku Solver
 A C++ sudoku solver. 
 
 In order to use:
 
 1. Compile with -std=c++2a -Ofast
 2. Pass a single command-line argument to input initial board state, using '-' for empty spaces, reading Right-to-Left, Top-to-Bottom. Strings may be terminated early, e.g. ---4----2---5 is a valid string.

Example use: 
```
$ git clone https://github.com/cosmobobak/sudoku
$ cd sudoku
$ g++ -Ofast -std=c++2a main.cpp -o sudoku_solver
$ ./sudoku_solver -6------59----1----1--7---26-2--4-3-----5--4--4--1------7--2--62---9-7--3------1-
Your sudoku:
┌───────┬───────┬───────┐
│ . 6 . │ . . . │ . . 5 │
│ 9 . . │ . . 1 │ . . . │
│ . 1 . │ . 7 . │ . . 2 │
├───────┼───────┼───────┤
│ 6 . 2 │ . . 4 │ . 3 . │
│ . . . │ . 5 . │ . 4 . │
│ . 4 . │ . 1 . │ . . . │
├───────┼───────┼───────┤
│ . . 7 │ . . 2 │ . . 6 │
│ 2 . . │ . 9 . │ 7 . . │
│ 3 . . │ . . . │ . 1 . │
└───────┴───────┴───────┘

Your solved sudoku:
┌───────┬───────┬───────┐
│ 7 6 3 │ 4 2 9 │ 1 8 5 │
│ 9 2 8 │ 5 6 1 │ 3 7 4 │
│ 4 1 5 │ 3 7 8 │ 9 6 2 │
├───────┼───────┼───────┤
│ 6 7 2 │ 9 8 4 │ 5 3 1 │
│ 8 3 1 │ 2 5 7 │ 6 4 9 │
│ 5 4 9 │ 6 1 3 │ 8 2 7 │
├───────┼───────┼───────┤
│ 1 5 7 │ 8 3 2 │ 4 9 6 │
│ 2 8 4 │ 1 9 6 │ 7 5 3 │
│ 3 9 6 │ 7 4 5 │ 2 1 8 │
└───────┴───────┴───────┘

solved in 2917 microseconds!
```
