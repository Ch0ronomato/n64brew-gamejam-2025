/*
 * Entry point
 */

#include <cmath>   // floor, tan
#include <cstring> // memcpy
#include <libdragon.h>
#include <deque>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dmath.h>
#include <vector>
#include <iterator>

#include "math/vec3.hpp"
#include "math/bezier.hpp"
#include "types.hpp"
#include "car.hpp"

namespace {
  T3DModel *track;
  T3DViewport viewport;
  uint8_t color_ambient[4] = {254, 254, 254, 0xFF};
  T3DVec3 light_dir_vec = {{0.f, 1.f, 0.f}};
  color_t light_dir_color {0xFF, 0xFF, 0xFF, 0xFF};
  T3DVec3 cameraPos = {2.92, 346.78, 106.92};
  T3DVec3 origin = {0, 0, 0};
  T3DVec3 yUp = {0.f, 1.f, 0.f};
  // @todo: In reality, this needs to be some kind of radius `r` always "behind" the ball
  jam::Vec3 cameraDistance = {-2.f, -1.f, -2.f};
}
void render_init() {
  // debug font
  rdpq_font_t *fnt1 = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO);
  rdpq_text_register_font(1, fnt1);

  // t3d
  t3d_init((T3DInitParams){0});


  // models
  track = t3d_model_load("rom://track.t3dm");
  assertf(track != nullptr, "Something went wrong");

  // viewport, lighting, camera
  viewport = t3d_viewport_create();

  t3d_vec3_norm(&light_dir_vec);

  // Pulled these from blender. If you want the relevant python
  // `mathutils.Matrix.decompose(C.scene.camera.matrix_world)[0].xzy * 64`
  // T3DVec3 cameraPos = {312.9292297363281, 186.78846740722656,
  //                      381.92987060546875};
}

jam::BezierTrack* track_init() {
  int track_data_size = 0;
  // The blender script guarentees we are float point aligned
  float *track_data =
      (float *)asset_load("rom:/track_layout.bin", &track_data_size);
  unsigned short version = 0, num_floats = 0;
  std::vector<jam::Vec3> track_points, normals;

  int header = *(int *)&track_data[0]; // Can be UB
  version = (header & 0xFF000000) >> 24;
  assertf(version == 1, "We expected file type 1");
  int vector_size = (header & 0x00FF0000) >> 16;
  assertf(vector_size == 3,
          "Data file does not contain points of 3 dimensions: %d", vector_size);
  // Load our points
  {
    num_floats = header & 0x0000FFFF;
    num_floats = (num_floats & 0x00FF) | ((num_floats & 0xFF00) >> 8);
    assertf(num_floats == 45, "Float size is wrong");
    track_points.reserve(std::floor(num_floats / vector_size));
    for (size_t i = 0; i < num_floats; i += vector_size) {
      auto p =
          jam::Vec3(track_data[i + 1], track_data[i + 2], -track_data[i + 3]);
      track_points.push_back(std::move(p));
    }
  }
  // Load our normals
  {
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
                        -track_data[offset + 2]);
      normals.push_back(std::move(p));
    }
    assertf((normals.size() * vector_size) == (size_t)normals_num_floats,
            "Invalid normals count from blender data %d, expected %d",
            normals.size() * vector_size, normals_num_floats / 4);
  }
  return jam::BezierTrack::from_blender_track_data(
        track_points.size() / 3, track_points, normals);
}

void camera_update(GameStateBook& lastPoints) {
    // Look at our last position, always trailing behind
    // our object, ala unreal spring arm component with a camera
    gamestate_page_t& state = lastPoints.back();
    if (lastPoints.size() < MAX_HISTORY) {
      cameraPos = static_cast<T3DVec3>(state.lastPoint - cameraDistance);
    } else {
      jam::Vec3 lastStop = lastPoints.front().lastPoint;

      auto headingDir = (state.lastPoint - lastStop).normalize();
      auto expectedDistance = std::sqrt(std::pow(cameraDistance.x, 2) + std::pow(cameraDistance.z, 2));
      assertf(expectedDistance > 0.f, "Distance is 0 or negative");
      auto angle = std::atan2(headingDir.z, headingDir.x);
      // assertf(angle > 0.087f, "Angle is < 5deg in radian (0.087)");
      jam::Vec3 p = jam::Vec3(std::cos(angle) * expectedDistance, cameraDistance.y, std::sin(angle) * expectedDistance);
      cameraPos = static_cast<T3DVec3>(state.lastPoint - p);
    }
}

gamestate_page_t& gamestatebook_update(GameStateBook& book) {
  gamestate_page_t newPage {{0,0,0}, 0};
  book.push_back(newPage);
  if (book.size() > MAX_HISTORY)
  {
    book.pop_front(); 
  }
  return book.back();
}

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

  render_init();
  jam::BezierTrack bezierTrack = *track_init();

  // get our initial position for the camera
  GameStateBook gameStateHistory;
  car_init(bezierTrack);
  while (true) {
    joypad_poll();
    auto pressed = joypad_get_buttons_held(JOYPAD_PORT_1);
    int xInput = joypad_get_axis_held(JOYPAD_PORT_1, JOYPAD_AXIS_STICK_X);

    // Tick
    gamestate_page_t& gstate = gamestatebook_update(gameStateHistory);
    gstate.lastInput = xInput;
    car_update(gameStateHistory, pressed);
    camera_update(gameStateHistory);

    // Render
    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(65.f), 10.0f,
                                5000.0f);
    t3d_viewport_look_at(&viewport, &cameraPos, &origin, &yUp);

    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(&viewport);

    t3d_screen_clear_color(RGBA32(254, 254, 254, 0xFF));
    t3d_screen_clear_depth();

    t3d_light_set_ambient(color_ambient);
    t3d_light_set_directional(0, &light_dir_color.r, &light_dir_vec);
    t3d_light_set_count(1);

    rdpq_set_prim_color(RGBA32(0xFF, 0xFF, 0xFF, 0xFF));

    T3DModelIter it = t3d_model_iter_create(track, T3D_CHUNK_TYPE_OBJECT);
    while (t3d_model_iter_next(&it)) {
      T3DModelState state = t3d_model_state_create();
      if (strcmp(it.object->name, "Sphere") == 0) {
        car_render(it, state);
      } else {
        t3d_model_draw_material(it.object->material, &state);
        t3d_model_draw_object(it.object, NULL);
      }
    }

    rdpq_sync_pipe();
    rdpq_detach_show();
    origin = static_cast<T3DVec3>(gstate.lastPoint);
  }

  rdpq_close();
  display_close();
  return 0;
}
