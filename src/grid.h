#pragma once

#include "raylib.h"
#include "vector2d.h"

#define GRID_WIDTH 50
#define GRID_HEIGHT 50

enum CellColour {
    Blank,
    Red,
    Blue,
};

struct Grid {
    CellColour colour[GRID_WIDTH * GRID_HEIGHT];
    Vi2D direction[GRID_WIDTH * GRID_HEIGHT];
};

int gridIndex(Vi2D);
Vi2D gridXY(int);
Color getRaylibColour(CellColour);

void update(const Grid* original, Grid* target);
Vf2D getForce(Vi2D cell_pos);
Vf2D getForceBetweenCells(Vi2D cell_pos_a, Vi2D cell_pos_b);
