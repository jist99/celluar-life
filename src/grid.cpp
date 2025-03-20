#include "raylib.h"
#include "grid.h"

int gridIndex(Vi2D pos) {
    // No bounds checking becuase yolo
    return (pos.y * GRID_WIDTH)  + pos.x;
}

Vi2D gridXY(int ind) {
    Vi2D out;
    out.x = ind % GRID_WIDTH;
    out.y = ind / GRID_WIDTH;
    return out;
}

Color getRaylibColour(CellColour col) {
    switch (col) {
        case Blank: return BLACK;
        case Red: return RED;
        case Blue: return BLUE;
    }

    return BLACK;
}

void update(const Grid* original, Grid* target) {
    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) {
        if (i == 0) {
            target->colour[i] = CellColour::Blue;
            continue;
        } else if (i == (GRID_WIDTH * GRID_HEIGHT) - 1) {
            target->colour[i] = CellColour::Blue;
            continue;
        }


        Vi2D pos = gridXY(i);
        if ((pos.x + pos.y) % 2 == 0) {
            target->colour[i] = CellColour::Red;
        } else {
            target->colour[i] = CellColour::Blank;
        }
    }  
}
