
default:
	@echo "options for make are build, test, bench, and graph_bench"

build:
	g++-11 -std=c++2a -Ofast -Wall -Wextra -Werror -Wpedantic main.cpp -o main

test:
	g++-11 -std=c++2a -Ofast -Wall -Wextra -Werror -Wpedantic sudoku_test.cpp -o test
	./test
	rm -f test
	g++-11 -std=c++2a -Ofast -Wall -Wextra -Werror -Wpedantic token_test.cpp -o test
	./test
	rm -f test

bench:
	g++-11 -std=c++2a -Ofast -Wall -Wextra -Werror -Wpedantic sudoku_bench.cpp -o bench
	./bench 10000

graph_bench:
	g++-11 -std=c++2a -pg -Wall -Wextra -Werror -Wpedantic sudoku_bench.cpp -o graph_bench
	./graph_bench 20
	gprof ./graph_bench | gprof2dot -s | dot -Tpng -o graph_bench.png

clean:
	rm -f main
	rm -f test
	rm -f bench
	rm -f graph_bench
	rm -f gmon.out
	rm -f graph_bench.png
