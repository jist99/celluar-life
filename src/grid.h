#include "raylib.h"

#define GRID_WIDTH 50
#define GRID_HEIGHT 50

struct Vi2D {
    int x;
    int y;
};

enum CellColour {
    Blank,
    Red,
    Blue,
};

enum Direction {
    N,
    NW,
    NE,
    E,
    SE,
    SW,
    W,
    S,
};

struct Grid {
    CellColour colour[GRID_WIDTH * GRID_HEIGHT];
    Direction direction[GRID_WIDTH * GRID_HEIGHT];
};

int gridIndex(Vi2D);
Vi2D gridXY(int);
Color getRaylibColour(CellColour);

void update(const Grid* original, Grid* target);
