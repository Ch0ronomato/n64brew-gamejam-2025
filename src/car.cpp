#include "car.hpp"
#include "joypad.h"
#include "types.hpp"
#include "math/bezier.hpp"
#include <cstddef>
#include <t3d/t3dmodel.h>
#include <t3d/t3dmath.h>

namespace
{
  float speedMagnitude = .05f;
  T3DMat4FP *loc[2]{
    (T3DMat4FP *)malloc_uncached(sizeof(T3DMat4FP)),
    (T3DMat4FP *)malloc_uncached(sizeof(T3DMat4FP))
  };
  color_t orderColor[4] {
    {0xFF, 0x00, 0x00, 0xFF},
    {0x00, 0xFF, 0x00, 0xFF},
    {0x00, 0x00, 0xFF, 0xFF},
    {0xFF, 0xFF, 0x00, 0xFF}
  };
  T3DVec3 corrected {{0.f, 0.f, 0.f}};
  jam::Vec3 normal(0, 0, 0);
  jam::Vec3 currentTrackPoint(0, 0, 0);
  jam::Vec3 currentLocal(0, 0, 0);
  jam::Vec3 lastDirection(0.f, 0.f, 0.f);
  jam::BezierTrack::Iterator trackIter = nullptr;

  T3DModel* carModel;
}

void print_vec(const char* name, const jam::Vec3& vec, bool buffer = false)
{
  debugf("= %s: %.4f, %.4f, %.4f =", name, vec.x, vec.y, vec.z);
  if (buffer) debugf("\n");
}

void car_init(jam::BezierTrack& track)
{
  trackIter = track.begin();
  auto tempIter = track.begin();
  ++tempIter;
  currentTrackPoint = (*trackIter).position;
  currentLocal = (*trackIter).position;
  lastDirection = (currentTrackPoint - currentLocal).normalize();

  carModel = t3d_model_load("rom://car.t3dm");
}

// @todo: We don't need base
void car_render(T3DModelState& state) {
  // it.object->material->setColorFlags &= 0b110;
  t3d_mat4fp_from_srt_euler(loc[0], (float[3]){0.1f, 0.1f, 0.1f},
                            (float[3]){0.f, 0.f, 0.f}, corrected.v);
  t3d_matrix_push(loc[0]);
  auto iter = t3d_model_iter_create(carModel, T3D_CHUNK_TYPE_OBJECT);
  int32_t i = 0;
  while (t3d_model_iter_next(&iter)) {
    iter.object->material->setColorFlags &= 0b110; 
    rdpq_set_prim_color(RGBA32(orderColor[i % 4].r, orderColor[i % 4].g, orderColor[i % 4].b, orderColor[i % 4].a));
    t3d_model_draw_material(iter.object->material, &state);
    t3d_model_draw_object(iter.object, NULL);
    state = t3d_model_state_create();
    i += 1;
  }
  t3d_matrix_pop(1);
  
  // No normals
  // T3DModelState extraState = t3d_model_state_create();
  // t3d_mat4fp_from_srt_euler(loc[1], (float[3]){1.f, 1.f, 1.f},
  //                           (float[3]){0.f, 0.f, 0.f}, base.v);
  // rdpq_set_prim_color(RGBA32(0x00, orderColor.g, 0x00, orderColor.a));
  // t3d_matrix_push(loc[1]);
  // t3d_model_draw_material(it.object->material, &extraState);
  // t3d_model_draw_object(it.object, NULL);
  // t3d_matrix_pop(1);
}

void car_update(GameStateBook &gameStateHistory, joypad_buttons_t buttons) {
  // Add our direction to this
  jam::Vec3 newPosition = currentLocal;
  if (buttons.a)
  {
    // Have we passed the current track point in our current direction
    auto p = lastDirection * speedMagnitude;
    newPosition = currentLocal + p;
    jam::Vec3 expectedDir = currentTrackPoint - currentLocal;
    jam::Vec3 headingDir = newPosition - currentLocal;
    if (newPosition == currentTrackPoint)
    {
      // we can skip this one
      ++trackIter;
      currentTrackPoint = (*trackIter).position;
    }
    else
    {
      auto proj = headingDir.projected_onto(expectedDir);
      if (proj.mag() >= expectedDir.mag())
      {
        // We've passed this point
        ++trackIter;
        currentTrackPoint = (*trackIter).position;
      }
    }
    lastDirection = (currentTrackPoint - newPosition).normalize();
  }

  // Check if we should increment iterator
  // normal = jam::Vec3(0, 0, 0);
  // if (gameStateHistory.size() > 5) {
  //   for (auto it = gameStateHistory.rbegin(); it != gameStateHistory.rbegin() + 5; it++) {
  //     normal += nextStop.data.normal * it->lastInput * 0.1f;
  //   }
  // }
  // corrected = static_cast<T3DVec3>(nextStop.position + normal);
  corrected = static_cast<T3DVec3>(newPosition);
  currentLocal = newPosition;

  // update our history
  gamestate_page_t& current = gameStateHistory.back();
  current.lastPoint = currentLocal;
}
