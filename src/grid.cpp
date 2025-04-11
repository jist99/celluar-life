#include "raylib.h"
#include "vector2d.h"
#include "grid.h"
#include "utils.h"
#include <cmath>
#include <iostream>

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

bool inBounds(Vi2D pos) {
    return !(pos.x < 0 || pos.x >= GRID_WIDTH || pos.y < 0 || pos.y >= GRID_HEIGHT);
}

Color getRaylibColour(CellColour col) {
    switch (col) {
        case Blank: return BLACK;
        case Red: return RED;
        case Blue: return BLUE;
    }

    return BLACK;
}

void update(
    const Grid* original,
    Grid* target,
    const float colour_attraction[NUM_COLOURS][NUM_COLOURS],
    float dt
) {
    const int neighbour_range = 16;
    const int repulsion_range = 2;

    // direction pass
    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) {
        if (original->colour[i] == CellColour::Blank) continue;
        Vi2D pos = gridXY(i);
        Vf2D force = {0,0};

        for (int x = pos.x - neighbour_range; x <= pos.x + neighbour_range; x++) {
            for (int y = pos.y - neighbour_range; y <= pos.y + neighbour_range; y++) {
                Vi2D neighbour_pos = Vi2D{x, y};
                int neighbour_index = gridIndex(neighbour_pos);

                if (!inBounds(neighbour_pos)) continue;
                if (original->colour[neighbour_index] == CellColour::Blank) continue;
                if (pos == neighbour_pos) continue;
                if (pos.distance(neighbour_pos) > neighbour_range) continue;

                force += getForceBetweenCells(pos, neighbour_pos, colour_attraction, original, repulsion_range, neighbour_range);
            }
        }

        force *= dt;
        target->direction[i] = Vf2D{round_away(force.x), round_away(force.y)};
    }

    // colour pass
    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) {
        Vi2D pos = gridXY(i);
        CellColour colour = CellColour::Blank;
        int blue_votes = 0;
        int red_votes = 0;

        for (int x = -neighbour_range; x <= +neighbour_range; x++) {
            for (int y = -neighbour_range; y <= +neighbour_range; y++) {
                Vi2D neighbour_pos = {x + pos.x, y + pos.y};

                if (!inBounds(neighbour_pos)) continue;
                if (pos.distance(neighbour_pos) > neighbour_range) continue;

                int neighbour_index = gridIndex({x + pos.x, y + pos.y});
                CellColour neighbour_colour = original->colour[neighbour_index];
                if (neighbour_colour == CellColour::Blank) continue;
                Vi2D direction = target->direction[neighbour_index];

                if (direction.x + x == 0 && direction.y + y == 0) {
                    if (neighbour_colour == CellColour::Blue) blue_votes++;
                    else red_votes++;
                }
            }
        }

        if (blue_votes > red_votes) target->colour[i] = CellColour::Blue;
        else if (red_votes > blue_votes) target->colour[i] = CellColour::Red;
    }
}

//Calculate force of cell b on cell a
//Based on particle life: Standard linear repulsion up to repulsion_distance, then linear increase (to coeff) halfway to max_distance, then linear decrease (to 0) up to max_distance
Vf2D getForceBetweenCells(Vi2D cell_pos_a, Vi2D cell_pos_b, const float colour_attraction[NUM_COLOURS][NUM_COLOURS], const Grid* original, float repulsion_distance, float max_distance)
{
    //get coefficient between colours (b acting on a)
    float coeff = colour_attraction[(original->colour[gridIndex(cell_pos_a)]) - 1][(original->colour[gridIndex(cell_pos_b)]) - 1];

    //calculate distance between cells
    float distance = cell_pos_a.distance(cell_pos_b);
    
    float magnitude = 0;

    //calculate magnitude of force between cells
    if(distance < repulsion_distance)
    {
        //linear force increase from -1 at d=0 to 0 at repulsion_distance
        magnitude = (distance/repulsion_distance) - 1;
        magnitude *= 2;
    }
    else if (distance < ((max_distance-repulsion_distance)/2))
    {
        //linear force increase up to max value of coeff
        magnitude = ((2*coeff) / (max_distance - 3*repulsion_distance)) * (distance - repulsion_distance);
    }
    else if (distance < max_distance)
    {
        //linear force decrease to 0 at max_distance
        magnitude = -((2*coeff) / (max_distance - 3*repulsion_distance)) * (distance - max_distance);
    }

    //return x and y component of force
    return Vf2D{magnitude * (float(cell_pos_b.x-cell_pos_a.x)/distance), magnitude * (float(cell_pos_b.y-cell_pos_a.y)/distance)};
}
