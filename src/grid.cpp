#include "raylib.h"
#include "grid.h"
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

void update(const Grid* original, Grid* target) {
    const int neighbour_range = 16;
    const int repulsion_range = 1;

    // direction pass
    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) {
        Vi2D pos = gridXY(i);
        Vf2D force = {0,0};

        for (int x = pos.x - neighbour_range; x < pos.x + neighbour_range; x++) {
            for (int y = pos.y - neighbour_range; y < pos.y + neighbour_range; y++) {
                Vi2D neighbour_pos = Vi2D{x, y};
                int neighbour_index = gridIndex(neighbour_pos);

                if (!inBounds(neighbour_pos)) continue;
                if (original->colour[neighbour_index] == CellColour::Blank) continue;
                if (pos == neighbour_pos) continue;

                Vf2D vec = {
                    pos.x - neighbour_pos.x != 0 ? 1/float(pos.x - neighbour_pos.x) : 0,
                    pos.y - neighbour_pos.y != 0 ? 1/float(pos.y - neighbour_pos.y) : 0,
                };
                vec = vec.norm();

                Vi2D repulsion_dist = (pos - neighbour_pos).abs();
                bool repulse = repulsion_dist.x <= repulsion_range && repulsion_dist.y <= repulsion_range;
                if (repulse) {
                    vec = vec * 16*16;
                }

                if (!repulse && original->colour[neighbour_index] == original->colour[i]) {
                    vec = -vec;
                }
                force += vec;
            }
        }

        force = force.norm();

        Vi2D direction = {0,0};
        if (force.x <= -0.1) direction.x = -1;
        else if (force.x >= 0.1) direction.x = 1;
        if (force.y <= -0.1) direction.y = -1;
        else if (force.y >= 0.1) direction.y = 1;

        target->direction[i] = direction;
    }

    // colour pass
    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) {
        Vi2D pos = gridXY(i);
        CellColour colour = CellColour::Blank;
        int blue_votes = 0;
        int red_votes = 0;

        for (int x = -1; x <= +1; x++) {
            for (int y = -1; y <= +1; y++) {
                if (!inBounds({x + pos.x, y + pos.y})) continue;

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


//TODO: get force based on coefficient of attraction/repulsion with colour
//Based on particle life: Standard linear repulsion up to distance dRep, then Linear increase up to
//(dMax-dRep)/2 then linear decrease to 0 up to dMax
//at coeff = 1 force = 
//get the force acting on a specific cell
Vf2D getForce(Vi2D cell_pos, const int repulsion_distance, const int max_distance, const float coeff)
{
    //for each cell in neighbourhood get force between cells
}

//calculate force of cell b on cell a
Vf2D getForceBetweenCells(Vi2D cell_pos_a, Vi2D cell_pos_b)
{
    //get colour of each cell
    //get coefficient between colours
    //calculate distance between cells
    
    //calculate magnitude of force between cells
    float force = 0;
    if(distance < repulsion_distance)
    {
        //linear force increase from -1 at d=0 to 0 at repulsion_distance
        force = (distance/repulsion_distance) - 1;
    }
    else if (distance < ((max_distance-repulsion_distance)/2))
    {
        //linear force increase up to max value of coeff
        force = ((2*coeff) / (max_distance - 3*repulsion_distance)) * (distance - repulsion_distance);
    }
    else if (distance < max_distance)
    {
        //linear force decrease to 0 at max_distance
        force = -((2*coeff) / (max_distance - 3*repulsion_distance)) * (distance - max_distance);
    }

    //get x and y components of force on cell a
}