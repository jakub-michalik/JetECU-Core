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
#include <utility>
#include <cassert>

namespace jetecu {

class Map1D {
public:
    Map1D(std::initializer_list<std::pair<float, float>> points);

    float lookup(float x) const;

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

    const ecu_map2d_t &raw() const { return map_; }

private:
    ecu_map2d_t map_;
};

} // namespace jetecu

#endif // JETECU_MAP_H
