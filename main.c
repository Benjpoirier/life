#include <SDL2/SDL.h>
#include <stdbool.h>
#include <unistd.h>


#define TABLE_ROWS 100
#define TABLE_COLS 100
#define SQUARE 8

enum cell_status{DEAD = 0, LIVE = 1};

struct world_s {
  enum cell_status** data;
  unsigned rows;
  unsigned cols;
};

void print_world(struct world_s w) {
 for (unsigned i = 0; i < w.rows; i++) {
  for (unsigned j = 0; j < w.cols; j++) {
    if (w.data[i][j]) {
      printf("x ");
    } else {
      printf("  ");
    }
  }
  printf("\n");
 }
}

void init_world(struct world_s* pw) {
  pw->rows = TABLE_ROWS;
  pw->cols = TABLE_COLS;
  pw->data = calloc(pw->rows, sizeof(enum cell_status*));

  for (unsigned i = 0; i < pw->rows; i++) {
    pw->data[i] = calloc(pw->cols, sizeof(enum cell_status));

     for (unsigned j = 0; j < pw->cols; j++) {
      pw->data[i][j] = rand() % 2;
    }
  }
}

void save_world(struct world_s w, struct world_s* current_world) {
  current_world->rows = w.rows;
  current_world->cols = w.cols;
  current_world->data = calloc(current_world->rows, sizeof(enum cell_status*));

  for (unsigned i = 0; i < current_world->rows; i++) {
    current_world->data[i] = calloc(current_world->cols, sizeof(enum cell_status));

     for (unsigned j = 0; j < current_world->cols; j++) {
      current_world->data[i][j] = w.data[i][j];
    }
  }
}

enum cell_status get_cell(struct world_s* pw, unsigned i, unsigned j) {
  if( i >= pw->rows || j >= pw->cols) {
    return 0;
  }
  return pw->data[i][j];
}

void free_world(struct world_s* current_world) {
  for (unsigned i = 0; i < current_world->rows; i++) {
    free(current_world->data[i]);
  }
  free(current_world->data);
}

void step(struct world_s* pw) {
  struct world_s current_world;
  save_world(*pw, &current_world);

  int number_of_living_neighbours = 0; // - 2 ou + 3 mort, 3 pile naissance, 2 rien si en vie.

  for (unsigned i = 0; i < current_world.rows; i++) {
     for (unsigned j = 0; j < current_world.cols; j++) {
      number_of_living_neighbours = 
        +get_cell(&current_world, i-1,  j)
        +get_cell(&current_world,  i , j-1)
        +get_cell(&current_world, i-1, j-1)
        +get_cell(&current_world, i+1,  j)
        +get_cell(&current_world,  i , j+1)
        +get_cell(&current_world, i+1, j+1)
        +get_cell(&current_world, i+1, j-1)
        +get_cell(&current_world, i-1, j+1);

      if (number_of_living_neighbours < 2 || number_of_living_neighbours > 3) {
        pw->data[i][j] = DEAD;
      }
      if (number_of_living_neighbours == 3) {
        pw->data[i][j] = LIVE;
      }
      if(number_of_living_neighbours == 2) {
        pw->data[i][j] = current_world.data[i][j];
      }
    }
  }
  free_world(&current_world);
}

int main() {
  srand(time(NULL));
  SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS );
  unsigned x = TABLE_ROWS * SQUARE;
  unsigned y = TABLE_COLS * SQUARE;

  SDL_Window *window = SDL_CreateWindow( "Game of life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, x, y, SDL_WINDOW_SHOWN );
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED );
  SDL_bool quit = SDL_FALSE;

  struct world_s w;

  SDL_Event e = { 0 };

  init_world(&w);

  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = SDL_TRUE;
      }
    }
      SDL_RenderClear(renderer);
      for (unsigned i = 0; i < w.rows; i++) {
          for (unsigned j = 0; j < w.cols; j++) {
            SDL_Rect r;
            r.x = i * SQUARE;
            r.y = j * SQUARE;
            r.w = SQUARE;
            r.h = SQUARE;

            if (w.data[i][j] == LIVE) {
              SDL_SetRenderDrawColor(renderer, 72, 168, 119, 255);
              SDL_RenderFillRect(renderer, &r );

            } else {
              SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
              SDL_RenderFillRect(renderer, &r);
            }
          }
        }
        SDL_RenderPresent(renderer);
        step(&w);
        SDL_Delay(80);
    }
  

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
