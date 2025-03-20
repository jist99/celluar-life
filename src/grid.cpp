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
        Vector2 force = {0,0};

        for (int x = pos.x - neighbour_range; x < pos.x + neighbour_range; x++) {
            for (int y = pos.y - neighbour_range; y < pos.y + neighbour_range; y++) {
                int neighbour_index = gridIndex({x, y});

                if (!inBounds({x,y})) continue;
                if (original->colour[neighbour_index] == CellColour::Blank) continue;
                if (pos.x == x && pos.y == y) continue;

                // TODO: Need some global repulsion when too close

                Vector2 vec = {};
                vec.x = pos.x - x != 0 ? 1/float(pos.x - x) : 0;
                vec.y = pos.y - y != 0 ? 1/float(pos.y - y) : 0;

                float vec_magnitude = std::sqrt(vec.x * vec.x + vec.y * vec.y);
                vec.x /= vec_magnitude;
                vec.y /= vec_magnitude;

                bool repulse = std::abs(pos.x - x) <= repulsion_range && std::abs(pos.y - y) <= repulsion_range;
                if (repulse) {
                    vec.x *= 16*16;
                    vec.y *= 16*16;
                }

                if (!repulse && original->colour[neighbour_index] == original->colour[i]) {
                    vec.x *= -1;
                    vec.y *= -1;
                }
                force.x += vec.x;
                force.y += vec.y;
            }
        }

        float force_magnitude = std::sqrt(force.x * force.x + force.y * force.y);
        force.x /= force_magnitude;
        force.y /= force_magnitude;

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
