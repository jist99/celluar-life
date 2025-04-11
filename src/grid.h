#pragma once

#include "raylib.h"
#include "vector2d.h"

#define GRID_WIDTH 100
#define GRID_HEIGHT 100
#define NUM_COLOURS 2

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

void update(const Grid* original, Grid* target, const float colour_attraction[NUM_COLOURS][NUM_COLOURS], float dt );
Vf2D getForceBetweenCells(Vi2D cell_pos_a, Vi2D cell_pos_b, const float colour_attraction[NUM_COLOURS][NUM_COLOURS], const Grid* original, float repulsion_distance, float max_distance);
Vf2D getShadowCell(Vf2D a, Vf2D b);