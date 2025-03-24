#include "raylib.h"
#include "grid.h"
#include "particle.h"
#include "vector2d.h"
#include <algorithm>
#include <utility>
#include <iostream>
#include <cmath>

#define PARTICLE

Vi2D getCellSize() {
    return Vi2D{GetScreenWidth() / GRID_WIDTH, GetScreenHeight() / GRID_HEIGHT};
}

void draw(const Grid* grid) {
    Vi2D cell_size = getCellSize();

    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) {
        Vi2D cell_pos = gridXY(i);
        DrawRectangleV(cell_pos * cell_size, cell_size, getRaylibColour(grid->colour[i]));
    }
}

void draw(const Particles* particles) {
    for (Particle p : particles->particles)
    {
        DrawCircle(p.position.x, p.position.y, PARTICLE_RADIUS, getRaylibColour(p.colour));
    }
}

void drawGridLines() {
    Vi2D cell_size = getCellSize();

    for (int x = 0; x < GRID_WIDTH; x++) {
        DrawLine(x * cell_size.x, 0, x * cell_size.x, GetScreenHeight(), DARKGRAY);
    }
    for (int y = 0; y < GRID_HEIGHT; y++) {
        DrawLine(0, y * cell_size.y, GetScreenWidth(), y * cell_size.y, DARKGRAY);
    }
}

int mouseToGrid() {
    Vi2D cell_size = getCellSize();

    Vector2 mouse_pos = GetMousePosition();
    int mouse_x = int(mouse_pos.x / cell_size.x);
    int mouse_y = int(mouse_pos.y / cell_size.y);
    return gridIndex({mouse_x, mouse_y});
}

int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(800, 800, "Cellular Life");

    //Coefficients of attraction of each pair of colours (default 0)
    float colour_attraction[NUM_COLOURS][NUM_COLOURS] = {};
    colour_attraction[CellColour::Blue-1][CellColour::Blue-1] = 1;
    colour_attraction[CellColour::Red-1][CellColour::Red-1] = 1;
    colour_attraction[CellColour::Red-1][CellColour::Blue-1] = -1;
    colour_attraction[CellColour::Blue-1][CellColour::Red-1] = -1;

    bool grid_lines = false;


    #ifdef PARTICLE
    Particles particles_a = {};
    Particles particles_b = {};

    Particles* current_particles = &particles_a;
    Particles* next_particles = &particles_b;

	// game loop
	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BLACK);

        draw(current_particles);
        if (grid_lines) drawGridLines();

		EndDrawing();

        //Update
        if (IsKeyPressed(KEY_G)) {
            grid_lines = !grid_lines;
        }

        //create particles on click
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Particle blue_p;
            blue_p.colour = CellColour::Blue;
            Vector2 pos = GetMousePosition();
            blue_p.position = {pos.x, pos.y};
            current_particles->particles.push_back(blue_p);
            next_particles->particles.push_back(blue_p);
        } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            Particle red_p;
            red_p.colour = CellColour::Red;
            Vector2 pos = GetMousePosition();
            red_p.position = {pos.x, pos.y};
            current_particles->particles.push_back(red_p);
            next_particles->particles.push_back(red_p);
        }

        if (IsKeyPressed(KEY_ENTER))
        {
            *current_particles = {};
            *next_particles = {};
        } 

        if (IsKeyPressed(KEY_SPACE)) {
            update(current_particles, next_particles, colour_attraction);
            current_particles->particles = next_particles->particles;
        }

        //WaitTime(1.0);
	}
    #else
    Grid grid_a = {};
    Grid grid_b = {};
    
    Grid* current_grid = &grid_a;
    Grid* next_grid = &grid_b;

	// game loop
	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BLACK);

        draw(current_grid);
        if (grid_lines) drawGridLines();

		EndDrawing();

        //Update
        if (IsKeyPressed(KEY_G)) {
            grid_lines = !grid_lines;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (current_grid->colour[mouseToGrid()] == CellColour::Blue)
                current_grid->colour[mouseToGrid()] = CellColour::Blank;
            else
                current_grid->colour[mouseToGrid()] = CellColour::Blue;
        } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            if (current_grid->colour[mouseToGrid()] == CellColour::Red)
                current_grid->colour[mouseToGrid()] = CellColour::Blank;
            else
                current_grid->colour[mouseToGrid()] = CellColour::Red;
        }

        if (IsKeyPressed(KEY_ENTER)) *current_grid = {};

        if (IsKeyPressed(KEY_SPACE)) {
            update(current_grid, next_grid, colour_attraction);
            std::swap(current_grid, next_grid);
            *next_grid = {};
        }

        //WaitTime(1.0);
	}
    #endif

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
