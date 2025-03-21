#include "raylib.h"
#include "grid.h"
#include <algorithm>
#include <utility>


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

void drawGridLines() {
    int window_width = GetScreenWidth();
    int window_height = GetScreenHeight();

    int cell_width = window_width / GRID_WIDTH;
    int cell_height = window_height / GRID_HEIGHT;

    for (int x = 0; x < GRID_WIDTH; x++) {
        DrawLine(x * cell_width, 0, x * cell_width, window_height, DARKGRAY);
    }
    for (int y = 0; y < GRID_HEIGHT; y++) {
        DrawLine(0, y * cell_height, window_width, y * cell_height, DARKGRAY);
    }
}

int mouseToGrid() {
    int window_width = GetScreenWidth();
    int window_height = GetScreenHeight();

    int cell_width = window_width / GRID_WIDTH;
    int cell_height = window_height / GRID_HEIGHT;

    Vector2 mouse_pos = GetMousePosition();
    int mouse_x = int(mouse_pos.x / cell_width);
    int mouse_y = int(mouse_pos.y / cell_height);
    return gridIndex({mouse_x, mouse_y});
}

int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(800, 800, "Cellular Life");

    Grid grid_a = {};
    Grid grid_b = {};
    
    Grid* current_grid = &grid_a;
    Grid* next_grid = &grid_b;

    bool grid_lines = false;

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
            update(current_grid, next_grid);
            std::swap(current_grid, next_grid);
            *next_grid = {};
        }

        //WaitTime(1.0);
	}

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
