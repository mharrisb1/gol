#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ALIVE_TOK "██"
#define DEAD_TOK  "░░"

#define clear_term() fputs("\033c", stdout)
#define sleep_ms(ms) usleep(ms * 1e3)
#define print_help()                                                           \
  printf("👽 gol - Game of life\n\n");                                          \
  printf("Usage: gol [OPTIONS]\n\n");                                          \
  printf("  -w, --width      <INT> Width of board          [default: 50]\n");  \
  printf("  -a, --height     <INT> Height of board         [default: 50]\n");  \
  printf("  -n, --iterations <INT> Iterations (-1 for inf) [default: -1]\n");  \
  printf("  -d, --duration   <INT> Turn duration (ms)      [default: 300]\n"); \
  printf("  -h, --help             Print this message\n");

int width, height, iterations, duration;

void parse(int argc, char *argv[], int *width, int *height, int *iteration,
           int *duration);

typedef struct Grid Grid;

Grid *Grid_new(int width, int height);
Grid *Grid_copy(Grid *src);
void  Grid_seed(Grid *grid);
void  Grid_display(Grid *grid);
void  Grid_transition(Grid *src, Grid *dst);
void  Grid_swap(Grid **a, Grid **b);
void  Grid_free(Grid *grid);

int main(int argc, char *argv[]) {
  parse(argc, argv, &width, &height, &iterations, &duration);

  Grid *grid      = Grid_new(width, height);
  Grid *grid_next = Grid_copy(grid);

  Grid_seed(grid);

  while (iterations != 0) {
    clear_term();
    Grid_display(grid);
    Grid_transition(grid, grid_next);
    Grid_swap(&grid, &grid_next);
    sleep_ms(duration);
    iterations--;
  }

  Grid_free(grid);
  Grid_free(grid_next);
}

static struct option longopts[] = {
    {"height", required_argument, NULL, 'a'},
    {"width", required_argument, NULL, 'w'},
    {"iterations", required_argument, NULL, 'n'},
    {"duration", required_argument, NULL, 'd'},
    {"help", no_argument, NULL, 'h'},
};

void parse(int argc, char *argv[], int *width, int *height, int *iterations,
           int *duration) {
  int         opt;
  const char *optstr = "a:w:n:d:h";

  *width      = 50;
  *height     = 50;
  *iterations = -1;
  *duration   = 500;

  while ((opt = getopt_long_only(argc, argv, optstr, longopts, NULL)) != -1) {
    switch (opt) {
      case 'h':
        print_help();
        exit(EXIT_SUCCESS);
      case 'w':
        if (optarg) {
          *width = atoi(optarg);
        }
        break;
      case 'a':
        if (optarg) {
          *height = atoi(optarg);
        }
        break;
      case 'n':
        if (optarg) {
          *iterations = atoi(optarg);
        }
        break;
      case 'd':
        if (optarg) {
          *duration = atoi(optarg);
        }
        break;
      case '?':
        print_help();
        exit(EXIT_FAILURE);
      default:
        abort();
    }
  }
}

struct Grid {
  int  width;
  int  height;
  int *cells;
};

Grid *Grid_new(int width, int height) {
  Grid *grid   = malloc(sizeof(Grid));
  grid->width  = width;
  grid->height = height;
  grid->cells  = calloc(width * height, sizeof(int));
  return grid;
}

Grid *Grid_copy(Grid *grid) {
  Grid *grid_new = Grid_new(grid->width, grid->height);
  memcpy(grid_new->cells, grid->cells, width * height * sizeof(int));
  return grid_new;
}

void Grid_free(Grid *grid) {
  free(grid->cells);
  free(grid);
}

void Grid_swap(Grid **a, Grid **b) {
  Grid *temp = *a;
  *a         = *b;
  *b         = temp;
}

int Grid_get_cell(Grid *grid, int x, int y) {
  return grid->cells[x + grid->width * y];
}

void Grid_set_cell(Grid *grid, int x, int y, int val) {
  grid->cells[x + grid->width * y] = val;
}

void Grid_seed(Grid *grid) {
  for (int y = 0; y < grid->height; y++) {
    for (int x = 0; x < grid->width; x++) {
      Grid_set_cell(grid, x, y, rand() % 2);
    }
  }
}

int Grid_get_cell_neighbor_count(Grid *grid, int x, int y) {
  int k = 0;
  int i = x + grid->width * y;

  int deltas[3] = {-1, 0, 1};

  for (int dy = 0; dy < 3; dy++) {
    for (int dx = 0; dx < 3; dx++) {
      if (deltas[dy] == 0 && deltas[dx] == 0) {
        continue; // don't include subject
      }
      if (i < grid->width && deltas[dy] < 0) {
        continue; // can't look up
      }
      if (i % grid->width == 0 && deltas[dx] < 0) {
        continue; // can't look left
      }
      if ((i + 1) % grid->width == 0 && deltas[dx] > 0) {
        continue; // can't look right
      }
      if (i >= ((grid->height - 1) * grid->width) && deltas[dy] > 0) {
        continue; // can't look down
      }
      k += grid->cells[i + (deltas[dx] + deltas[dy] * grid->width)];
    }
  }

  return k;
}

void Grid_transition(Grid *src, Grid *dst) {
  for (int y = 0; y < src->height; y++) {
    for (int x = 0; x < src->width; x++) {
      int k = Grid_get_cell_neighbor_count(src, x, y);
      if (Grid_get_cell(dst, x, y)) {
        Grid_set_cell(dst, x, y, 2 <= k && k <= 3);
      } else {
        Grid_set_cell(dst, x, y, k == 3);
      }
    }
  }
}

void Grid_display(Grid *grid) {
  for (int y = 0; y < grid->height; y++) {
    for (int x = 0; x < grid->width; x++) {
      printf("%s", Grid_get_cell(grid, x, y) ? ALIVE_TOK : DEAD_TOK);
    }
    printf("\n");
  }
}
