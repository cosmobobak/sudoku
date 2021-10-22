#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <string>

enum class RangeType {
    ROW,
    COL,
    BOX,
    GLOBAL,
};

template <RangeType ROW_COL_BOX_GLOBAL>
class Iterator2D {
   private:
    using matrix = std::array<std::array<int, 9>, 9>;
    using matrix_pointer = matrix*;

    size_t col, row;
    matrix_pointer const target;

   public:
    static constexpr auto END = 9;
    static constexpr auto GLOBAL_END = 81;

    static constexpr auto IS_ROW = ROW_COL_BOX_GLOBAL == RangeType::ROW;
    static constexpr auto IS_COL = ROW_COL_BOX_GLOBAL == RangeType::COL;
    static constexpr auto IS_BOX = ROW_COL_BOX_GLOBAL == RangeType::BOX;
    static constexpr auto IS_GLOBAL = ROW_COL_BOX_GLOBAL == RangeType::GLOBAL;

    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = int;
    using pointer = value_type*;
    using reference = value_type&;

    Iterator2D(matrix_pointer t, int n = 0) : target(t) {
        if constexpr (IS_ROW) {
            this->col = 0;
            this->row = n / 9;
        } else if (IS_COL) {
            this->col = n % 9;
            this->row = 0;
        } else if (IS_BOX) {
            this->col = ((n % 9) / 3) * 3;
            this->row = ((n / 9) / 3) * 3;
        } else {
            this->col = n % 9;
            this->row = n / 9;
        }
    }

    Iterator2D(matrix_pointer const t, int x, int y) : target(t) {
        this->col = x;
        this->row = y;
    }

    Iterator2D(Iterator2D const&) = default;
    Iterator2D& operator=(Iterator2D other) {
        col = other.col;
        row = other.row;
        return *this;
    }

    // consider specialising to return pointers if we get a ROW.
    static constexpr auto begin(matrix_pointer const target, int n = 0) {
        return Iterator2D(target, n);
    }

    // consider specialising to return pointers if we get a ROW.
    static constexpr auto begin(matrix& target_reference, int n = 0) {
        return Iterator2D(&target_reference, n);
    }

    // consider specialising to return pointers if we get a ROW.
    static constexpr auto end(matrix_pointer const target, int n = 0) {
        if constexpr (IS_ROW) {
            return Iterator2D(target, END, n / 9);
        } else if (IS_COL) {
            return Iterator2D(target, n % 9, END);
        } else if (IS_BOX) {
            return Iterator2D(target, ((n % 9) / 3) * 3, ((n / 9) / 3) * 3 + 3);
        } else {
            return Iterator2D(target, 0, END);
        }
    }

    // consider specialising to return pointers if we get a ROW.
    static constexpr auto end(matrix& target_reference, int n = 0) {
        if constexpr (IS_ROW) {
            return Iterator2D(&target_reference, END, n / 9);
        } else if (IS_COL) {
            return Iterator2D(&target_reference, n % 9, END);
        } else if (IS_BOX) {
            return Iterator2D(&target_reference, ((n % 9) / 3) * 3, ((n / 9) / 3) * 3 + 3);
        } else {
            return Iterator2D(&target_reference, 0, END);
        }
    }

    inline auto operator*() -> int& {
        return (*target)[row][col];
    }

    operator int() const {
        return col + row * 9;
    }

    // Prefix increment
    inline Iterator2D& operator++() {
        // std::cout << "I am an iterator at x: " << x << " y: " << y << " and my progress is " << progress << "\n";
        if constexpr (IS_ROW) {
            ++col;
        } else if (IS_COL) {
            ++row;
        } else if (IS_BOX) {
            // this hack saves us ~2ms of computation for a hard sudoku.
            constexpr uint64_t c = 1 + UINT64_C(0xffffffffffffffff) / 3;
            if (((uint32_t)col + 1) * c <= c - 1) {
                ++row;
                col -= 2;
            } else {
                ++col;
            }
        } else {
            if (col == 8) {
                col = 0;
                ++row;
            } else {
                ++col;
            }
        }

        return *this;
    }

    // Postfix increment
    inline Iterator2D operator++(int) {
        Iterator2D tmp = *this;
        ++(*this);
        return tmp;
    }

    // Prefix decrement
    inline Iterator2D& operator--() {
        // std::cout << "I am an iterator at x: " << x << " y: " << y << " and my progress is " << progress << "\n";
        if constexpr (IS_ROW) {
            --col;
        } else if (IS_COL) {
            --row;
        } else if (IS_BOX) {
            // this hack saves us ~2ms of computation for a hard sudoku.
            constexpr uint64_t c = 1 + 0xffffffffffffffffull / 3;
            if (((uint32_t)col) * c <= c - 1) {
                --row;
                col += 2;
            } else {
                --col;
            }
        } else {
            if (col == 0) {
                col = 8;
                --row;
            } else {
                --col;
            }
        }

        return *this;
    }

    // Postfix decrement
    inline Iterator2D operator--(int) {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    friend bool operator==(const Iterator2D& a, const Iterator2D& b) {
        if constexpr (IS_ROW) {
            return a.col == b.col;
        } else if (IS_COL || IS_BOX) {
            return a.row == b.row;
        } else {
            return a.row == b.row && a.col == b.col;
        }
    }

    friend bool operator!=(const Iterator2D& a, const Iterator2D& b) {
        // std::cout << a.x << " " << a.y << " != " << b.x << " " << b.y << " ?\n";
        if constexpr (IS_ROW) {
            return a.col != b.col;
        } else if (IS_COL || IS_BOX) {
            return a.row != b.row;
        } else {
            return a.row != b.row || a.col != b.col;
        }
    }
};

struct SudokuRange {
    using enum RangeType;

    std::array<std::array<int, 9>, 9>* target;

    SudokuRange(std::array<std::array<int, 9>, 9>* target) : target(target) {}

    auto begin() const -> Iterator2D<GLOBAL> {
        return Iterator2D<GLOBAL>(target);
    }

    auto end() const -> Iterator2D<GLOBAL> {
        return Iterator2D<GLOBAL>(target, Iterator2D<GLOBAL>::GLOBAL_END);
    }
};

auto make_range(std::array<std::array<int, 9>, 9>& target) {
    return SudokuRange(&target);
}