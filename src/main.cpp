/*
 * Entry point
 */

#include <cmath>   // floor
#include <cstring> // memcpy
#include <libdragon.h>
#include <rdpq.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include <vector>

#include "./math/vec3.hpp"
#include "joypad.h"
#include "math/bezier.hpp"
#include "t3d/t3dmath.h"

int main(void) {
  // Initialize debug console
  debug_init_isviewer();

  // Setting up the display
  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE,
               FILTERS_RESAMPLE_ANTIALIAS_DEDITHER);
  rdpq_init();
  dfs_init(DFS_DEFAULT_LOCATION);
  asset_init_compression(3);
  joypad_init();

  // debug font
  rdpq_font_t *fnt1 = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO);
  rdpq_text_register_font(1, fnt1);

  // t3d
  t3d_init((T3DInitParams){0});

  bool loaded = false;
  int track_data_size = 0;
  // The blender script guarentees we are float point aligned
  float *track_data =
      (float *)asset_load("rom:/track_layout.bin", &track_data_size);
  unsigned short version = 0, num_floats = 0;
  std::vector<jam::Vec3> track_points, normals;

  int header = *(int *)&track_data[0]; // Can be UB
  version = (header & 0xFF000000) >> 24;
  int vector_size = (header & 0x00FF0000) >> 16;
  assertf(vector_size == 3,
          "Data file does not contain points of 3 dimensions: %d", vector_size);
  num_floats = header & 0x0000FFFF;
  num_floats = (num_floats & 0x00FF) | ((num_floats & 0xFF00) >> 8);
  assertf(num_floats == 45, "Float size is wrong");
  track_points.reserve(std::floor(num_floats / vector_size));
  for (size_t i = 0; i < num_floats; i += vector_size) {
    auto p =
        jam::Vec3(track_data[i + 1], track_data[i + 2], -track_data[i + 3]);
    track_points.push_back(std::move(p));
  }
  // Load our normals
  int normals_num_floats = *(int *)&track_data[num_floats + 1];
  normals_num_floats = (normals_num_floats & 0xFFFF0000) >> 16;
  normals_num_floats =
      (normals_num_floats & 0x00FF) | ((normals_num_floats & 0xFF00) >> 8);
  assertf(normals_num_floats == 180, "Normals count incorrect: %d",
          normals_num_floats);
  normals.reserve(normals_num_floats / 12u);
  for (size_t i = 0; i < (size_t)normals_num_floats; i += vector_size) {
    size_t offset = num_floats + i + 2;
    auto p = jam::Vec3(track_data[offset], track_data[offset + 1],
                       track_data[offset + 2]);
    normals.push_back(std::move(p));
  }
  assertf((normals.size() * vector_size) == (size_t)normals_num_floats,
          "Invalid normals count from blender data %d, expected %d",
          normals.size() * vector_size, normals_num_floats / 4);

  // models
  T3DModel *track = t3d_model_load("rom://track.t3dm");
  assertf(track != nullptr, "Something went wrong");

  // viewport, lighting, camera
  T3DViewport viewport = t3d_viewport_create();

  uint8_t color_ambient[4] = {254, 254, 254, 0xFF};
  T3DVec3 light_dir_vec = {{0.f, 1.f, 0.f}};
  t3d_vec3_norm(&light_dir_vec);
  color_t light_dir_color = RGBA32(0xFF, 0xFF, 0xFF, 0xFF);

  // Pulled these from blender. If you want the relevant python
  // `mathutils.Matrix.decompose(C.scene.camera.matrix_world)[0].xzy * 64`
  // T3DVec3 cameraPos = {312.9292297363281, 186.78846740722656,
  //                      381.92987060546875};
  T3DVec3 cameraPos = {2.92, 346.78, 106.92};
  T3DVec3 origin = {0, 0, 0};
  T3DVec3 yUp = {0.f, 1.f, 0.f};
  unsigned short idx = 0, t = 0;
  T3DMat4FP *loc[1]{(T3DMat4FP *)malloc_uncached(sizeof(T3DMat4FP))};

  auto bezierTrack = *jam::BezierTrack::from_blender_track_data(
      track_points.size() / 3, track_points, normals);
  auto orderColor = RGBA32(0xFF, 0xFF, 0xFF, 0x00);
  bool run = false;
  while (true) {
  }

  rdpq_close();
  display_close();
  return 0;
}
