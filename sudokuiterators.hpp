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

    int x, y;
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
            this->x = 0;
            this->y = n / 9;
        } else if (IS_COL) {
            this->x = n % 9;
            this->y = 0;
        } else if (IS_BOX) {
            this->x = ((n % 9) / 3) * 3;
            this->y = ((n / 9) / 3) * 3;
        } else {
            this->x = n % 9;
            this->y = n / 9;
        }
    }

    Iterator2D(matrix_pointer const t, int x, int y) : target(t) {
        this->x = x;
        this->y = y;
    }

    Iterator2D(Iterator2D const&) = default;
    Iterator2D& operator=(Iterator2D other) {
        x = other.x;
        y = other.y;
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

    auto operator*() -> int& {
        return (*target)[y][x];
    }

    operator int() const {
        return x + y * 9;
    }

    // Prefix increment
    inline Iterator2D& operator++() {
        // std::cout << "I am an iterator at x: " << x << " y: " << y << " and my progress is " << progress << "\n";
        if constexpr (IS_ROW) {
            ++x;
        } else if (IS_COL) {
            ++y;
        } else if (IS_BOX) {
            // this hack saves us ~2ms of computation for a hard sudoku.
            constexpr uint64_t c = 1 + UINT64_C(0xffffffffffffffff) / 3;
            if (((uint32_t)x + 1) * c <= c - 1) {
                ++y;
                x -= 2;
            } else {
                ++x;
            }
        } else {
            if (x == 8) {
                x = 0;
                ++y;
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

    // Prefix decrement
    inline Iterator2D& operator--() {
        // std::cout << "I am an iterator at x: " << x << " y: " << y << " and my progress is " << progress << "\n";
        if constexpr (IS_ROW) {
            --x;
        } else if (IS_COL) {
            --y;
        } else if (IS_BOX) {
            // this hack saves us ~2ms of computation for a hard sudoku.
            constexpr uint64_t c = 1 + 0xffffffffffffffffull / 3;
            if (((uint32_t)x) * c <= c - 1) {
                --y;
                x += 2;
            } else {
                --x;
            }
        } else {
            if (x == 0) {
                x = 8;
                --y;
            } else {
                --x;
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
            return a.x == b.x;
        } else if (IS_COL || IS_BOX) {
            return a.y == b.y;
        } else {
            return a.y == b.y && a.x == b.x;
        }
    }

    friend bool operator!=(const Iterator2D& a, const Iterator2D& b) {
        // std::cout << a.x << " " << a.y << " != " << b.x << " " << b.y << " ?\n";
        if constexpr (IS_ROW) {
            return a.x != b.x;
        } else if (IS_COL || IS_BOX) {
            return a.y != b.y;
        } else {
            return a.y != b.y || a.x != b.x;
        }
    }
};

template <typename T>
class ColIterator {
    T* target;

   public:
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = value_type*;
    using reference = value_type&;

    ColIterator(T* target) : target(target) {}

    auto operator*() -> reference {
        return *target;
    }

    // Prefix increment
    inline ColIterator& operator++() {
        target += 9;
        return *this;
    }

    // Postfix increment
    inline ColIterator operator++(int) {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    // Prefix decrement
    inline ColIterator& operator--() {
        target -= 9;
        return *this;
    }

    // Postfix decrement
    inline ColIterator operator--(int) {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    // addition operator
    inline ColIterator& operator+=(difference_type n) {
        target += n;
        return *this;
    }

    // subtraction operator
    inline ColIterator& operator-=(difference_type n) {
        target -= n;
        return *this;
    }

    // offset dereference operator (array access operator)
    inline reference operator[](difference_type n) {
        return *(target + n);
    }

    friend bool operator==(ColIterator a, ColIterator b) {
        return a.target == b.target;
    }

    friend bool operator!=(ColIterator a, ColIterator b) {
        return a.target != b.target;
    }

    // inequality comparisons
    friend bool operator<(ColIterator a, ColIterator b) {
        return a.target < b.target;
    }

    friend bool operator>(ColIterator a, ColIterator b) {
        return a.target > b.target;
    }

    friend bool operator<=(ColIterator a, ColIterator b) {
        return a.target <= b.target;
    }

    friend bool operator>=(ColIterator a, ColIterator b) {
        return a.target >= b.target;
    }

    friend ColIterator operator+(ColIterator a, difference_type n) {
        a.target += n * 9;
        return a;
    }

    friend ColIterator operator+(difference_type n, ColIterator a) {
        a.target += n * 9;
        return a;
    }

    friend ColIterator operator-(ColIterator a, difference_type n) {
        a.target -= n * 9;
        return a;
    }

    friend size_t operator-(ColIterator a, ColIterator b) {
        return (a.target - b.target) / 9;
    }
};

template <typename T>
class ConstColIterator {
    const T* target;

   public:
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = value_type*;
    using reference = value_type&;

    ConstColIterator(const T* target) : target(target) {}

    auto operator*() -> T {
        return *target;
    }

    // Prefix increment
    inline ConstColIterator& operator++() {
        target += 9;
        return *this;
    }

    // Postfix increment
    inline ConstColIterator operator++(int) {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    // Prefix decrement
    inline ConstColIterator& operator--() {
        target -= 9;
        return *this;
    }

    // Postfix decrement
    inline ConstColIterator operator--(int) {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    // addition operator
    inline ConstColIterator& operator+=(difference_type n) {
        target += n;
        return *this;
    }

    // subtraction operator
    inline ConstColIterator& operator-=(difference_type n) {
        target -= n;
        return *this;
    }

    // offset dereference operator (array access operator)
    inline T operator[](difference_type n) {
        return *(target + n);
    }

    friend bool operator==(ConstColIterator a, ConstColIterator b) {
        return a.target == b.target;
    }

    friend bool operator!=(ConstColIterator a, ConstColIterator b) {
        return a.target != b.target;
    }

    // inequality comparisons
    friend bool operator<(ConstColIterator a, ConstColIterator b) {
        return a.target < b.target;
    }

    friend bool operator>(ConstColIterator a, ConstColIterator b) {
        return a.target > b.target;
    }

    friend bool operator<=(ConstColIterator a, ConstColIterator b) {
        return a.target <= b.target;
    }

    friend bool operator>=(ConstColIterator a, ConstColIterator b) {
        return a.target >= b.target;
    }

    friend ConstColIterator operator+(ConstColIterator a, difference_type n) {
        a.target += n * 9;
        return a;
    }

    friend ConstColIterator operator+(difference_type n, ConstColIterator a) {
        a.target += n * 9;
        return a;
    }

    friend ConstColIterator operator-(ConstColIterator a, difference_type n) {
        a.target -= n * 9;
        return a;
    }

    friend size_t operator-(ConstColIterator a, ConstColIterator b) {
        return (a.target - b.target) / 9;
    }
};

class BoxIterator {
    int* target;
    uint8_t progress;

    public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = int;
    using pointer = value_type*;
    using reference = value_type&;

    BoxIterator(int* target) : target(target), progress(0) {}

    auto operator*() -> int& {
        return *target;
    }

    // Prefix increment
    inline BoxIterator& operator++() {
        if (progress == 2) {
            target += 7;
            progress = 0;
        } else {
            ++progress;
            ++target;
        }
        return *this;
    }

    // Prefix decrement
    inline BoxIterator& operator--() {
        if (progress == 0) {
            target -= 7;
            progress = 2;
        } else {
            --progress;
            --target;
        }
        return *this;
    }

    // Postfix increment
    inline BoxIterator operator++(int) {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    // Postfix decrement
    inline BoxIterator operator--(int) {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    friend bool operator==(BoxIterator a, BoxIterator b) {
        return a.target == b.target;
    }

    friend bool operator!=(BoxIterator a, BoxIterator b) {
        return a.target != b.target;
    }
};

class ConstBoxIterator {
    const int* target;
    uint8_t progress;

   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = int;
    using pointer = value_type*;
    using reference = value_type&;

    ConstBoxIterator(const int* target) : target(target), progress(0) {}

    auto operator*() -> int {
        return *target;
    }

    // Prefix increment
    inline ConstBoxIterator& operator++() {
        if (progress == 2) {
            target += 7;
            progress = 0;
        } else {
            ++progress;
            ++target;
        }
        return *this;
    }

    // Prefix decrement
    inline ConstBoxIterator& operator--() {
        if (progress == 0) {
            target -= 7;
            progress = 2;
        } else {
            --progress;
            --target;
        }
        return *this;
    }

    // Postfix increment
    inline ConstBoxIterator operator++(int) {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    // Postfix decrement
    inline ConstBoxIterator operator--(int) {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    friend bool operator==(ConstBoxIterator a, ConstBoxIterator b) {
        return a.target == b.target;
    }

    friend bool operator!=(ConstBoxIterator a, ConstBoxIterator b) {
        return a.target != b.target;
    }
};

class BaseBoard {
    std::array<int, 81> data = {0};

   public:
    BaseBoard() = default;
    BaseBoard(const BaseBoard&) = default;
    BaseBoard(BaseBoard&&) = default;
    BaseBoard& operator=(const BaseBoard&) = default;
    BaseBoard& operator=(BaseBoard&&) = default;
    ~BaseBoard() = default;

    auto to_size(int* ptr) const -> size_t {
        return ptr - data.data();
    }

    static constexpr auto whichrow(size_t idx) -> int {
        return idx / 9;
    }

    static constexpr auto whichcol(size_t idx) -> int {
        return idx % 9;
    }

    static constexpr auto whichbox(size_t idx) -> int {
        return (whichrow(idx) / 3) * 3 + whichcol(idx) / 3;
    }

    inline auto begin() {
        return data.begin();
    }

    inline auto end() {
        return data.end();
    }

    inline auto row_begin(int* ptr) {
        auto n = whichrow(to_size(ptr));
        return data.data() + 9 * n;
    }

    inline auto row_begin(int* ptr) const -> const int* {
        auto n = whichrow(to_size(ptr));
        return data.data() + 9 * n;
    }

    inline auto row_end(int* ptr) {
        auto n = whichrow(to_size(ptr));
        return data.data() + 9 * (n + 1);
    }

    inline auto row_end(int* ptr) const -> const int* {
        auto n = whichrow(to_size(ptr));
        return data.data() + 9 * (n + 1);
    }

    inline auto col_begin(int* ptr) {
        auto n = whichcol(to_size(ptr));
        return ColIterator(data.data() + n);
    }

    inline auto col_begin(int* ptr) const {
        auto n = whichcol(to_size(ptr));
        return ConstColIterator(data.data() + n);
    }

    inline auto col_end(int* ptr) {
        auto n = whichcol(to_size(ptr));
        // valid starting range is 0 -> 8
        // valid ending range is 81 -> 89
        return ColIterator(data.data() + n + 81);
    }

    inline auto col_end(int* ptr) const {
        auto n = whichcol(to_size(ptr));
        // valid starting range is 0 -> 8
        // valid ending range is 81 -> 89
        return ConstColIterator(data.data() + n + 81);
    }

    inline auto box_begin(int* ptr) {
        static constexpr std::array entry_points = {
            0, 3, 6, 27, 30, 33, 54, 57, 60,
        };
        auto n = whichbox(to_size(ptr));
        auto entry_point = entry_points[n];
        return BoxIterator(data.data() + entry_point);
    }

    inline auto box_begin(int* ptr) const {
        static constexpr std::array entry_points = {
            0, 3, 6, 27, 30, 33, 54, 57, 60,
        };
        auto n = whichbox(to_size(ptr));
        auto entry_point = entry_points[n];
        return ConstBoxIterator(data.data() + entry_point);
    }

    inline auto box_end(int* ptr) {
        static constexpr std::array exit_points = {
            27, 30, 33, 54, 57, 60, 81, 84, 87,
        };
        // possible faster searching the array
        auto n = whichbox(to_size(ptr));
        auto exit_point = exit_points[n];
        return BoxIterator(data.data() + exit_point);
    }

    inline auto box_end(int* ptr) const {
        static constexpr std::array exit_points = {
            27, 30, 33, 54, 57, 60, 81, 84, 87,
        };
        // possible faster searching the array
        auto n = whichbox(to_size(ptr));
        auto exit_point = exit_points[n];
        return ConstBoxIterator(data.data() + exit_point);
    }

    // array access operator
    inline int& operator[](size_t n) {
        return data[n];
    }

    // const array access operator
    inline int operator[](size_t n) const {
        return data[n];
    }
};