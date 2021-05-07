#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <string>

template <int ROW_COL_BOX>
class Iterator2D {
   private:
    const std::array<std::array<int, 9>, 9>& target;
    int x, y;

   public:
    static constexpr auto ROW = 0;
    static constexpr auto COL = 1;
    static constexpr auto BOX = 2;
    static constexpr auto END = 9;

    static constexpr auto IS_ROW = ROW_COL_BOX == ROW;
    static constexpr auto IS_COL = ROW_COL_BOX == COL;
    static constexpr auto IS_BOX = ROW_COL_BOX == BOX;

    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = int;
    using pointer = int*;    // or also value_type*
    using reference = int&;  // or also value_type&

    Iterator2D(const std::array<std::array<int, 9>, 9>& t, int n) : target(t) {
        if constexpr (IS_ROW) {
            this->x = 0;
            this->y = n / 9;
        } else if (IS_COL) {
            this->x = n % 9;
            this->y = 0;
        } else {
            this->x = ((n % 9) / 3) * 3;
            this->y = ((n / 9) / 3) * 3;
        }
    }

    Iterator2D(const std::array<std::array<int, 9>, 9>& t, int x, int y, int progress) : target(t) {
        this->x = x;
        this->y = y;
    }

    static inline auto begin(const std::array<std::array<int, 9>, 9>& target, int n) {
        return Iterator2D(target, n);
    }

    static inline auto end(const std::array<std::array<int, 9>, 9>& target, int n) {
        if constexpr (IS_ROW) {
            return Iterator2D(target, END, n / 9, END);
        } else if (IS_COL) {
            return Iterator2D(target, n % 9, END, END);
        } else {
            return Iterator2D(target, ((n % 9) / 3) * 3, ((n / 9) / 3) * 3 + 3, END);
        }
    }

    inline auto operator*() const -> const int& {
        return target[y][x];
    }

    // Prefix increment
    inline Iterator2D& operator++() {
        // std::cout << "I am an iterator at x: " << x << " y: " << y << " and my progress is " << progress << "\n";
        if constexpr (IS_ROW) {
            ++x;
        } else if (IS_COL) {
            ++y;
        } else {
            // this hack saves us ~2ms of computation for a hard sudoku.
            constexpr uint64_t c = 1 + UINT64_C(0xffffffffffffffff) / 3;
            if (((uint32_t)x + 1) * c <= c - 1) {
                ++y;
                x -= 2;
            } else {
                ++x;
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

    friend bool operator==(const Iterator2D& a, const Iterator2D& b) { 
        if constexpr (IS_ROW) {
            return a.x == b.x;
        } else if (IS_COL) {
            return a.y == b.y;
        } else {
            return a.y == b.y; 
        }
    };

    friend bool operator!=(const Iterator2D& a, const Iterator2D& b) {
        // std::cout << a.x << " " << a.y << " != " << b.x << " " << b.y << " ?\n";
        if constexpr (IS_ROW) {
            return a.x != b.x;
        }
        else if (IS_COL) {
            return a.y != b.y;
        }
        else {
            return a.y != b.y;
        }
    };
};