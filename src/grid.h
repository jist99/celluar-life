#pragma once

#include "raylib.h"
#include "vector2d.h"

#define GRID_WIDTH 100
#define GRID_HEIGHT 100
#define NUM_COLOURS 8
#define UPDATE_THRESHOLD 0.1

enum CellColour {
    Blank,
    Red,
    Blue,
    Green,
    Pink,
    Yellow,
    Brown,
    Orange,
    White,
    COUNT,
};

struct Grid {
    CellColour colour[GRID_WIDTH * GRID_HEIGHT];
    Vi2D direction[GRID_WIDTH * GRID_HEIGHT];
};

int gridIndex(Vi2D);
Vi2D gridXY(int);
Color getRaylibColour(CellColour);

void update(
    const Grid* original, Grid* target,
    const float colour_attraction[NUM_COLOURS][NUM_COLOURS],
    float dt,
    int neighbour_range, int repulsion_range
);
Vf2D getForceBetweenCells(Vi2D cell_pos_a, Vi2D cell_pos_b, CellColour b_colour, const float colour_attraction[NUM_COLOURS][NUM_COLOURS], const Grid* original, float repulsion_distance, float max_distance);
Vi2D getShadowCell(Vi2D a, Vi2D b);
float getShortestDistance(Vi2D a, Vi2D b);
Vi2D gridMod(Vi2D a);
void SaveGrid(Grid* g, int& neighbour, int& repulsion, float colour_attraction[NUM_COLOURS][NUM_COLOURS], std::string name);
void LoadGrid(Grid* g, int& neighbour, int& repulsion, float colour_attraction[NUM_COLOURS][NUM_COLOURS], std::string name);
bool EndsWith(std::string str, std::string end);
