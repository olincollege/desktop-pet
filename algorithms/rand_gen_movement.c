/*
 * Show a sprite in a window, change position
 */

#include "rand_gen_movement.h"
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// gcc -o window scripts/window.c -lSDL2 -lSDL2_image

/* Sets constants */
// replace the width and height with screen size
#define WIDTH 1920 // this is based on the background image size currently
#define HEIGHT 1080
#define SPRITE_WIDTH 260
#define SPRITE_HEIGHT 280

int generate_random(int min, int max, int *prev) {
  // not inclusive min/max
  // if the number was chosen last time, then has a 40% chance to get picked again (everything else has 20%)
  int random = *prev;
  int probability = rand() % 10;
  if (probability > 4) {
    int random = rand() % (max - min) + min;
    *prev = random;
  }
  return random;
}

void move_up(SDL_Rect *sprite, int distance) {
  if (sprite->y > 0) {
    sprite->y -= distance;
  } else {
    sprite->y += distance;
  }
}

void move_down(SDL_Rect *sprite, int distance) {
  if (sprite->y < HEIGHT - SPRITE_HEIGHT) {
    sprite->y += distance;
  } else {
    sprite->y -= distance;
  }
}

void move_left(SDL_Rect *sprite, int distance) {
  if (sprite->x > 0) {
    sprite->x -= distance;
  } else {
    sprite->x += distance;
  }
}

void move_right(SDL_Rect *sprite, int distance) {
  if (sprite->x < WIDTH - SPRITE_WIDTH) {
    sprite->x += distance;
  } else {
    sprite->x -= distance;
  }
}

void move_random_direction(int num, SDL_Rect *sprite_pos, int distance) {
  switch (num) {
  case 0:
    move_up(sprite_pos, distance);
    break;
  case 1:
    move_down(sprite_pos, distance);
    break;
  case 2:
    move_left(sprite_pos, distance);
    break;
  case 3:
    move_right(sprite_pos, distance);
    break;
  case 4:
    // do nothing; idle
    break;
  default:
    move_up(sprite_pos, distance);
    printf("default case reached\n");
    break;
  }
}

void end_program(SDL_Texture *texture, SDL_Texture *boat_texture,
                 SDL_Renderer *renderer, SDL_Window *window) {
  SDL_DestroyTexture(texture);
  SDL_DestroyTexture(boat_texture);
  // printf("got here 4\n");
  SDL_DestroyRenderer(renderer);
  // printf("got here 6\n");
  SDL_DestroyWindow(window);
  // printf("got here 7\n");
  SDL_Quit();
  printf("got here 8\n");
}

int main(int argc, char **argv) {
  SDL_Window *window = NULL;

  bool quit = false;
  SDL_Event event;

  /*
   * Initialises the SDL video subsystem (as well as the events subsystem).
   * Returns 0 on success or a negative error code on failure using
   * SDL_GetError().
   */
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
    return 1;
  }

  if (IMG_Init(IMG_INIT_PNG) == 0) {
    printf("Error SDL2_image Initialization\n");
    return 2;
  }

  // Creates a SDL window
  window = SDL_CreateWindow(
      "SDL Example",           /* Title of the SDL window */
      SDL_WINDOWPOS_UNDEFINED, /* Position x of the window */
      SDL_WINDOWPOS_UNDEFINED, /* Position y of the window */
      WIDTH,                   /* Width of the window in pixels */
      HEIGHT,                  /* Height of the window in pixels */
      SDL_WINDOW_BORDERLESS || SDL_WINDOW_MAXIMIZED); /* Additional flag(s) */

  // Checks if window has been created; if not, exits program
  if (window == NULL) {
    fprintf(stderr, "SDL window failed to initialise: %s\n", SDL_GetError());
    return 3;
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    printf("renderer error\n");
    return 4;
  }

  if (SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT) != 0) {
    printf("resolution setting error\n");
    printf("%s\n", SDL_GetError());
    return 5;
  }

  SDL_Surface *image = IMG_Load("background.png");
  if (image == NULL) {
    printf("error loading image\n");
    return 6;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
  if (texture == NULL) {
    printf("error creating texture\n");
    return 7;
  }
  SDL_FreeSurface(image); // once the texture is made, you don't have to keep
                          // the surface around

  // Create sprite
  SDL_Texture *sprite;

  SDL_Surface *boat_image = IMG_Load("boat.png");
  if (boat_image == NULL) {
    printf("error loading boat_image\n");
    return 6;
  }
  SDL_Texture *boat_texture =
      SDL_CreateTextureFromSurface(renderer, boat_image);
  if (boat_texture == NULL) {
    printf("error creating boat_texture\n");
    return 7;
  }
  SDL_FreeSurface(boat_image); // once the texture is made, you don't have to
                               // keep the surface around
  SDL_Rect dstrect = {50, 50, SPRITE_WIDTH, SPRITE_HEIGHT}; // sets the desired size/pos of the sprite

  int movement_counter = 0; // spaces out movement commands to look smoother
  int random_num = 0; // afaik declaring it once and just changing the value is faster
  int prev = 0;

  fprintf(stdout, "window initialized\n");
  while (!quit) {
    if (SDL_PollEvent(&event)) {
      switch (event.type) {
      // if you press a key
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {

        case SDLK_ESCAPE:
          quit = true;
          break;
        }
        switch (event.type) {
        // if you press a key
        case SDL_KEYDOWN:
          switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
            printf("got here\n");
            quit = true;
            break;
          }

          // break out of larger SDL_KEYDOWN
          break;

        case SDL_QUIT:
          quit = true;
          break;
        }
      }
    }

    movement_counter++;
    if (movement_counter % 200000 == 0) {
      random_num = generate_random(0, 5, &prev);
      move_random_direction(random_num, &dstrect, 50);
      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, texture, NULL, NULL);
      SDL_RenderCopy(renderer, boat_texture, NULL,
                     &dstrect); // display boat sprite
      SDL_RenderPresent(renderer);
    }
  }

  end_program(texture, boat_texture, renderer, window);

  return 0;
}
