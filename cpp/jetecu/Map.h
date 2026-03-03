#pragma once

#ifndef JETECU_MAP_H
#define JETECU_MAP_H

#ifdef __cplusplus
extern "C" {
#endif
#include "core/ecu_map.h"
#ifdef __cplusplus
}
#endif

#include <initializer_list>
#include <iterator>
#include <utility>
#include <cassert>

namespace jetecu {

/** @brief A single breakpoint in a 1-D map. */
struct Point {
    float x;
    float y;
};

/**
 * @brief Iterator over Map1D breakpoints.
 *
 * Yields Point values by constructing them from the underlying arrays.
 */
class PointIterator {
public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = Point;
    using difference_type = std::ptrdiff_t;
    using pointer = const Point *;
    using reference = Point;

    PointIterator(const float *xp, const float *yp) : xp_(xp), yp_(yp) {}

    Point operator*() const { return {*xp_, *yp_}; }
    PointIterator &operator++() { ++xp_; ++yp_; return *this; }
    PointIterator operator++(int) { auto tmp = *this; ++xp_; ++yp_; return tmp; }
    PointIterator &operator--() { --xp_; --yp_; return *this; }
    PointIterator operator--(int) { auto tmp = *this; --xp_; --yp_; return tmp; }
    PointIterator operator+(difference_type n) const { return {xp_ + n, yp_ + n}; }
    PointIterator operator-(difference_type n) const { return {xp_ - n, yp_ - n}; }
    difference_type operator-(const PointIterator &o) const { return xp_ - o.xp_; }
    bool operator==(const PointIterator &o) const { return xp_ == o.xp_; }
    bool operator!=(const PointIterator &o) const { return xp_ != o.xp_; }
    bool operator<(const PointIterator &o) const { return xp_ < o.xp_; }

private:
    const float *xp_;
    const float *yp_;
};

/** @brief Range adapter for iterating Map1D breakpoints. */
class PointRange {
public:
    PointRange(const float *x, const float *y, int count)
        : x_(x), y_(y), count_(count) {}

    PointIterator begin() const { return PointIterator(x_, y_); }
    PointIterator end() const { return PointIterator(x_ + count_, y_ + count_); }
    int size() const { return count_; }

private:
    const float *x_;
    const float *y_;
    int count_;
};

class Map1D {
public:
    Map1D(std::initializer_list<std::pair<float, float>> points);

    float lookup(float x) const;

    /** @brief Alias for lookup(). */
    float operator()(float x) const { return lookup(x); }

    /** @brief Number of breakpoints. */
    int size() const { return map_.count; }

    /** @brief Get breakpoint at index i. */
    Point point(int i) const;

    /** @brief Range of breakpoints for range-based for. */
    PointRange points() const;

    const ecu_map1d_t &raw() const { return map_; }

private:
    ecu_map1d_t map_;
};

class Map2D {
public:
    Map2D(std::initializer_list<float> row_keys,
          std::initializer_list<float> col_keys,
          std::initializer_list<std::initializer_list<float>> values);

    float lookup(float row, float col) const;

    /** @brief Alias for lookup(). */
    float operator()(float row, float col) const { return lookup(row, col); }

    /** @brief Number of row keys. */
    int rows() const { return map_.rows; }

    /** @brief Number of column keys. */
    int cols() const { return map_.cols; }

    const ecu_map2d_t &raw() const { return map_; }

private:
    ecu_map2d_t map_;
};

} // namespace jetecu

#endif // JETECU_MAP_H
