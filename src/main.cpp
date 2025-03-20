/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"
#include "grid.h"


void draw(const Grid* grid) {
    int window_width = GetScreenWidth();
    int window_height = GetScreenHeight();

    //int horizontal_buffer = window_width % GRID_WIDTH;
    //int vertical_buffer = window_height % GRID_HEIGHT;

    int cell_width = window_width / GRID_WIDTH;
    int cell_height = window_height / GRID_HEIGHT;

    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) {
        Vi2D cell_pos = gridXY(i);

        DrawRectangle(
            cell_pos.x * cell_width, cell_pos.y * cell_height,
            cell_width,  cell_height, getRaylibColour(grid->colour[i])
        );
    }
}

int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(800, 800, "Cellular Life");

    Grid grid;

	// game loop
	while (!WindowShouldClose())
	{
        //Update
        update(nullptr, &grid);

		BeginDrawing();
		ClearBackground(BLACK);

        draw(&grid);

		EndDrawing();
	}

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
