#pragma once

// Removes the function-like min and max <windows.h> defines without NOMINMAX; a qualified
// std::min or std::max does not parse while they are live.

#include <algorithm>

#undef min
#undef max
