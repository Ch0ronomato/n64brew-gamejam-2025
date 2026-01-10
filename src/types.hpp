#pragma once
#include <deque>
#include "math/vec3.hpp"
const size_t MAX_HISTORY = 15;

typedef struct gamestate_s {
  jam::Vec3 lastPoint;
  int lastXInput;
  int lastYInput;
} gamestate_page_t;

using GameStateBook = std::deque<gamestate_page_t>;
