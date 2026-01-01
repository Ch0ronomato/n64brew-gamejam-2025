#pragma once
#include "joypad.h"
#include "types.hpp"
#include "math/bezier.hpp"
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
void car_init(jam::BezierTrack& track);
void car_update(GameStateBook& gameStateHistory, joypad_buttons_t buttons);
void car_render(T3DModelIter it, T3DModelState& state);

