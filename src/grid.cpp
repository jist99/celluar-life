#include "raylib.h"
#include "vector2d.h"
#include "grid.h"
#include "utils.h"
#include <cmath>
#include <iostream>
#include <algorithm>

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
        case Green: return GREEN;
        case Pink: return PINK;
        case Yellow: return YELLOW;
        case Brown: return BROWN;
        case Orange: return ORANGE;
        case White: return WHITE;
        case COUNT: 
            std::cerr << "COUNT has no colour" << std::endl;
            exit(1);
    }

    return BLACK;
}

void update(
    const Grid* original,
    Grid* target,
    const float colour_attraction[NUM_COLOURS][NUM_COLOURS],
    float dt,
    int neighbour_range, int repulsion_range
) {
    // direction pass
    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) {
        if (original->colour[i] == CellColour::Blank) continue;
        Vi2D pos = gridXY(i);
        Vf2D force = {0,0};

        for (int x = pos.x - neighbour_range; x <= pos.x + neighbour_range; x++) {
            for (int y = pos.y - neighbour_range; y <= pos.y + neighbour_range; y++) {
                Vi2D neighbour_pos = Vi2D{x, y};
                if (!inBounds(neighbour_pos))
                    neighbour_pos = gridMod(neighbour_pos);
                int neighbour_index = gridIndex(neighbour_pos);
                CellColour neighbour_colour = original->colour[neighbour_index];
                if (neighbour_colour == CellColour::Blank) continue;
                if (pos == neighbour_pos) continue;
                Vi2D shadow_pos = getShadowCell(pos, neighbour_pos);
                if(pos.distance(shadow_pos) < pos.distance(neighbour_pos))
                    neighbour_pos = shadow_pos;
                if (pos.distance(neighbour_pos) > neighbour_range) continue;
                force += getForceBetweenCells(pos, neighbour_pos, neighbour_colour, colour_attraction, original, repulsion_range, neighbour_range);
            }
        }

        force *= dt;
        target->direction[i] = Vf2D{round_away(force.x), round_away(force.y)};
    }

    // colour pass
    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) {
        Vi2D pos = gridXY(i);
        int votes[CellColour::COUNT] = {0};

        for (int x = -neighbour_range; x <= +neighbour_range; x++) {
            for (int y = -neighbour_range; y <= +neighbour_range; y++) {
                Vi2D neighbour_pos = {x + pos.x, y + pos.y};

                if (!inBounds(neighbour_pos))
                    neighbour_pos = gridMod(neighbour_pos);
                int neighbour_index = gridIndex({neighbour_pos.x, neighbour_pos.y});
                CellColour neighbour_colour = original->colour[neighbour_index];
                if (neighbour_colour == CellColour::Blank) continue;
                if (getShortestDistance(pos,neighbour_pos) > neighbour_range) continue;
                Vi2D direction = target->direction[neighbour_index];

                if (direction.x + x == 0 && direction.y + y == 0) {
                    votes[neighbour_colour]++;
                }
            }
        }
        //assign the colour with most votes
        int max_vote_i = std::distance(votes, std::max_element(votes, votes+CellColour::COUNT));
        if(votes[max_vote_i] > 0)
            target->colour[i] = static_cast<CellColour>(max_vote_i);
    }
}

//Calculate force of cell b on cell a
//Based on particle life: Standard linear repulsion up to repulsion_distance, then linear increase (to coeff) halfway to max_distance, then linear decrease (to 0) up to max_distance
Vf2D getForceBetweenCells(Vi2D cell_pos_a, Vi2D cell_pos_b, CellColour b_colour, const float colour_attraction[NUM_COLOURS][NUM_COLOURS], const Grid* original, float repulsion_distance, float max_distance)
{
    //get coefficient between colours (b acting on a)
    float coeff = colour_attraction[(original->colour[gridIndex(cell_pos_a)]) - 1][b_colour - 1];

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

//function to get a "shadow" version of b off of the screen that repsresents a cell equivilent to wrapping from b across the screen to a
Vi2D getShadowCell(Vi2D a, Vi2D b)
{
    //calculate shadow versions of b on 4 adjacent plains extending offscreen in each direction
    Vi2D b_left = {b.x-GRID_WIDTH, b.y};
    Vi2D b_right = {b.x+GRID_WIDTH, b.y};
    Vi2D b_top = {b.x, b.y-GRID_HEIGHT};
    Vi2D b_bottom = {b.x, b.y+GRID_HEIGHT};

    //find closest point to a
    float left_dist = a.distance(b_left);
    float right_dist = a.distance(b_right);
    float top_dist = a.distance(b_top);
    float bottom_dist = a.distance(b_bottom);

    float min_distance = std::min({left_dist, right_dist, top_dist, bottom_dist});

    if(min_distance == left_dist)
        return b_left;
    else if(min_distance == right_dist)
        return b_right;
    else if(min_distance == top_dist)
        return b_top;
    else
        return b_bottom;

}

//get shortest distance between cells considering wrapping around screen
float getShortestDistance(Vi2D a, Vi2D b)
{
    //calculate shadow versions of b on 4 adjacent plains extending offscreen in each direction
    Vi2D b_left = {b.x-GRID_WIDTH, b.y};
    Vi2D b_right = {b.x+GRID_WIDTH, b.y};
    Vi2D b_top = {b.x, b.y-GRID_HEIGHT};
    Vi2D b_bottom = {b.x, b.y+GRID_HEIGHT};

    //find closest point to a
    float dist = a.distance(b);
    float left_dist = a.distance(b_left);
    float right_dist = a.distance(b_right);
    float top_dist = a.distance(b_top);
    float bottom_dist = a.distance(b_bottom);

    return std::min({dist, left_dist, right_dist, top_dist, bottom_dist});
}

//custom mod to handle negatives
int mod(int a, int b)
{
    int mod = a % b;
    if (mod < 0)
        mod += b;
    return mod;
}

Vi2D gridMod(Vi2D a)
{
    return Vi2D{mod(a.x,GRID_WIDTH),mod(a.y,GRID_HEIGHT)};
}
