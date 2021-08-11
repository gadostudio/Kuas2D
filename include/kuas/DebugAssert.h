#pragma once

#include <cassert>

#ifndef NDEBUG
#define KUAS_ASSERT(x) assert(x)
#else
#define KUAS_ASSERT(x)
#endif
