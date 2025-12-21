#include "cpp/jetecu/Map.h"

#include <algorithm>

namespace jetecu {

Map1D::Map1D(std::initializer_list<std::pair<float, float>> points)
{
    map_ = {};
    int i = 0;
    for (auto &p : points) {
        if (i >= MAP_MAX_POINTS) break;
        map_.x[i] = p.first;
        map_.y[i] = p.second;
        i++;
    }
    map_.count = i;
}

float Map1D::lookup(float x) const
{
    return ecu_map1d_lookup(&map_, x);
}

Map2D::Map2D(std::initializer_list<float> row_keys,
             std::initializer_list<float> col_keys,
             std::initializer_list<std::initializer_list<float>> values)
{
    map_ = {};
    int r = 0;
    for (auto k : row_keys) {
        if (r >= MAP2D_MAX_ROWS) break;
        map_.row_keys[r++] = k;
    }
    map_.rows = r;

    int c = 0;
    for (auto k : col_keys) {
        if (c >= MAP2D_MAX_COLS) break;
        map_.col_keys[c++] = k;
    }
    map_.cols = c;

    int ri = 0;
    for (auto &row : values) {
        if (ri >= map_.rows) break;
        int ci = 0;
        for (auto v : row) {
            if (ci >= map_.cols) break;
            map_.values[ri][ci++] = v;
        }
        ri++;
    }
}

float Map2D::lookup(float row, float col) const
{
    return ecu_map2d_lookup(&map_, row, col);
}

} // namespace jetecu
