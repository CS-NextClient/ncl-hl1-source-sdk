#ifndef COMMON_MINMAX_H
#define COMMON_MINMAX_H

#undef min
#undef max
#undef clamp

#include <algorithm>

#if defined( min ) || defined( max )
#error "Don't define min and/or max, use std::min and std::max instead!"
#endif

#endif //COMMON_MINMAX_H
