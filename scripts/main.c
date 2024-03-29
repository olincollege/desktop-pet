/**
 * Main game loop for a desktop pet.
 */

#include "controller.h"
#include "model.h"
#include "view.h"
#include <SDL2/SDL_image.h>
#include <math.h> // math seems to require the -lm flag
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  state init;
  bool quit = false;
  SDL_Event event;

  // Initializes the window.
  init = setup_state();

  SDL_Texture *cat_texture =
      initialize_texture("assets/catsheet_1.jpg", init.renderer);

  // NOLINTBEGIN(*-magic-numbers)
  const int number_frames = 26;
  int walk_gray[26] = {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
                       2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1};
  int walk_white[26] = {3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4,
                        5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4};
  int walk_black[26] = {6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7,
                        8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7};
  int walk_orange[26] = {9,  9,  9,  9,  9,  9,  9,  10, 10, 10, 10, 10, 10,
                         11, 11, 11, 11, 11, 11, 11, 10, 10, 10, 10, 10, 10};
  // NOLINTEND(*-magic-numbers)
  animation cat_animate = {.texture = cat_texture,
                           .frames_loop = walk_orange,
                           .num_frames = number_frames};
  animation cat_animate1 = cat_animate;
  animation cat_animate2 = cat_animate;
  animation cat_animate3 = cat_animate;
  const int cat_sheet_rows = 8;
  const int cat_sheet_cols = 12;
  const float cat_box_scale = 3.0F;

  initialize_animation(&cat_animate, cat_sheet_rows, cat_sheet_cols, 0);
  initialize_animation(&cat_animate1, cat_sheet_rows, cat_sheet_cols, 1);
  initialize_animation(&cat_animate2, cat_sheet_rows, cat_sheet_cols, 2);
  initialize_animation(&cat_animate3, cat_sheet_rows, cat_sheet_cols, 3);
  SDL_Rect cat_box;
  make_animation_box(&cat_box, &cat_animate, 0, 0, cat_box_scale);
  SDL_Rect cat_box1;
  make_animation_box(&cat_box1, &cat_animate1, 0, SPRITE_HEIGHT, cat_box_scale);
  SDL_Rect cat_box2;
  make_animation_box(&cat_box2, &cat_animate2, SPRITE_WIDTH, 0, cat_box_scale);
  SDL_Rect cat_box3;
  make_animation_box(&cat_box3, &cat_animate3, SPRITE_WIDTH, SPRITE_HEIGHT,
                     cat_box_scale);

  int direction = 0;
  int prev = 0;  // Represents the previous direction the sprite moved.
  int cycle = 0; // Used for my manual pseudo-lerping implementation.
  int speed = 2; // An int from 1-10. higher # = higher speed.
  const int speed_range = 10;

  while (!quit) {
    // Check the time of this update cycle.
    Uint64 time_start = SDL_GetPerformanceCounter();

    /* Poll event, contents of if need to be in a function, if statement is
     * important. */
    if (SDL_PollEvent(&event)) {
      int action = handle_event(event);

      switch (action) {
      // If a key is pressed.
      case 1:
        speed = rand() % speed_range +
                1; // NOLINT(cert-msc30-c, cert-msc50-cpp concurrency-mt-unsafe)
        break;
      case 2:
        // For some reason removing this print statement breaks code.
        printf("Clicked mouse\n");
        SDL_Point mousePosition;
        // Mouse click coords from event handler.
        mousePosition.x = event.motion.x;
        mousePosition.y = event.motion.y;

        if (SDL_PointInRect(&mousePosition, &cat_box)) {
          change_random_cat_color(&cat_animate, &cat_animate1, &cat_animate2,
                                  &cat_animate3, walk_orange, walk_black,
                                  walk_white, walk_gray);
        }
        break;
      case 3:
        quit = true;
        break;
      default:
        break;
      }
    }

    // NOLINTBEGIN(*-magic-numbers)
    // Randomly changes speed.
    if (rand() % 70000000 ==
        0) { // NOLINT(cert-msc30-c, cert-msc50-cpp concurrency-mt-unsafe)
      speed = rand() % 10 +
              1; // NOLINT(cert-msc30-c, cert-msc50-cpp concurrency-mt-unsafe)
    }
    cycle++;
    if (cycle == SMOOTHNESS) {
      // Only update random num if the sprite has pseudo-lerped to another spot.
      direction = generate_random(0, 5, prev);
      cycle = 0;
    }
    move_direction(direction, &cat_box, speed, &prev);
    SDL_RenderClear(init.renderer);
    SDL_RenderCopy(init.renderer, init.texture, NULL, NULL);

    switch (direction) {
    case 0:
      loop_animation(&cat_animate3, init.renderer, &cat_box);
      break;
    case 1:
      loop_animation(&cat_animate, init.renderer, &cat_box);
      break;
    case 2:
      loop_animation(&cat_animate1, init.renderer, &cat_box);
      break;
    case 3:
    default:
      loop_animation(&cat_animate2, init.renderer, &cat_box);
      break;
    }

    SDL_RenderPresent(init.renderer);
    Uint64 time_end = SDL_GetPerformanceCounter();
    float elapsed_time = (time_end - time_start) /
                         (float)SDL_GetPerformanceFrequency() * 1000.0F;
    // Cap fps at 60.
    SDL_Delay(floor(16.666F - elapsed_time));
    // NOLINTEND(*-magic-numbers)
  }

  end_program(init.texture, cat_texture, init.renderer, init.window);
  return 0;
}
