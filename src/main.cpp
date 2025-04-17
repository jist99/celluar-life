#include <stdio.h>
#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "grid.h"
#include "particle.h"
#include "vector2d.h"
#include "raygui.h"
#include <algorithm>
#include <utility>
#include <stdio.h>


#define PARTICLE

Vi2D getCellSize() {
    return Vi2D{GetScreenWidth() / GRID_WIDTH, GetScreenHeight() / GRID_HEIGHT};
}

void draw(const Grid* grid) {
    Vi2D cell_size = getCellSize();

    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) {
        Vi2D cell_pos = gridXY(i);
        if(grid->colour[i] == CellColour::Blank) continue;
        DrawRectangleV(cell_pos * cell_size, cell_size, getRaylibColour(grid->colour[i]));
    }
}

void draw(const Particles* particles) {
    Vi2D cell_size = getCellSize();
    for (Particle p : particles->particles)
    {
        DrawCircleV(p.position * cell_size, PARTICLE_RADIUS, getRaylibColour(p.colour));
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

struct GUIState {
    bool repulsion_editmode;
    int *repulsion_range;
    bool neighbour_editmode;
    int *neighbour_range;

    char value_box_texts[NUM_COLOURS][NUM_COLOURS][32];
    bool value_box_edits[NUM_COLOURS][NUM_COLOURS];

    bool clear;
    bool pause;
    bool grid_lines;

    CellColour selected_col;
};

void initGuiState(
    GUIState& gui_state,
    float colour_attraction[NUM_COLOURS][NUM_COLOURS],
    int *repulsion_range,
    int *neighbour_range
) {
    gui_state.repulsion_range = repulsion_range;
    gui_state.neighbour_range = neighbour_range;
    gui_state.selected_col = CellColour::Blue;

    for (int x = 0; x < NUM_COLOURS; x++) {
        for (int y = 0; y < NUM_COLOURS; y++) {
            snprintf(gui_state.value_box_texts[x][y], 32, "%.0f", colour_attraction[x][y]);
        }
    }
}

void guiPanel(GUIState& state, float colour_attraction[NUM_COLOURS][NUM_COLOURS]) {
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    // Repulsion settings
    GuiLabel({10, 10, 160, 30}, "Repulsion Range:");
    if (GuiValueBox({10, 40, 160, 30}, "", state.repulsion_range, 0, 50, state.repulsion_editmode)) {
        state.repulsion_editmode = !state.repulsion_editmode;
    }

    // Neighbour range settings
    GuiLabel({10, 90, 160, 30}, "Neigbour Range:");
    if (GuiValueBox({10, 120, 160, 30}, "", state.neighbour_range, 0, 100, state.neighbour_editmode)) {
        state.neighbour_editmode = !state.neighbour_editmode;
    }

    // Attraction settings
    GuiLabel({10, 170, 160, 30}, "Attraction");
    int x_pos = 30;
    for(int i = CellColour::Red; i < CellColour::COUNT; i++) {
        CellColour col = static_cast<CellColour>(i);
        DrawRectangleV({float(x_pos), 203}, {16, 16}, getRaylibColour(col));
        x_pos += 24;
    }

    int y_pos = 226;
    for(int i = CellColour::Red; i < CellColour::COUNT; i++) {
        CellColour col = static_cast<CellColour>(i);
        DrawRectangleV({6, float(y_pos)}, {16, 16}, getRaylibColour(col));
        y_pos += 24;
    }

    GuiSetStyle(DEFAULT, TEXT_SIZE, 15);
    for(int x = 0; x < CellColour::COUNT - 1; x++) {
        for(int y = 0; y < CellColour::COUNT - 1; y++) {
            Rectangle rect = {float(26+(24*x)), float(222+(24*y)), 24.0, 24.0};
            if (GuiValueBoxFloat(rect, "", state.value_box_texts[x][y], &colour_attraction[x][y], state.value_box_edits[x][y])) {
                state.value_box_edits[x][y] = !state.value_box_edits[x][y];
            }
        }
    }

    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    // Simulation Controls
    if (GuiButton({10, 440, 160, 30}, "Clear")) {
        state.clear = true;
    }
    if (GuiButton({10, 480, 160, 30}, state.pause ? "Unpause" : "Pause")) {
        state.pause = !state.pause;
    }
    if (GuiButton({10, 520, 160, 30}, "Gridlines")) {
        state.grid_lines = !state.grid_lines;
    }

    // Colour palette
    GuiLabel({10, 570, 160, 30}, "Selected:");
    DrawRectangleRec({100, 570, 24, 24}, getRaylibColour(state.selected_col));
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 2; y++) {
            CellColour col = static_cast<CellColour>(y*4+x + 1);

            Rectangle rect = {float(10 + x * 24), float(604 + y * 24), 24, 24};

            // Little hack, hide a button behind the rectangle, a bit sneaky
            if (GuiButton(rect, "")) {
                state.selected_col = col;
            }

            DrawRectangleRec(rect, getRaylibColour(col));
        }
    }
}

void particleGame(
    float colour_attraction[NUM_COLOURS][NUM_COLOURS],
    int *repulsion_range,
    int *neighbour_range
) {
    Particles particles_a = {};
    Particles particles_b = {};

    Particles* current_particles = &particles_a;
    Particles* next_particles = &particles_b;

    Vf2D cell_size = getCellSize();

    GUIState gui_state = GUIState{0};
    initGuiState(gui_state, colour_attraction, repulsion_range, neighbour_range);
    bool menu_mode = false;

	// game loop
	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BLACK);

        draw(current_particles);
        if (gui_state.grid_lines) drawGridLines();

        if (menu_mode) guiPanel(gui_state, colour_attraction);
        else DrawText("Press TAB to open menu", 10, 10, 20, GRAY);

		EndDrawing();

        //Update
        if (IsKeyPressed(KEY_G)) {
            gui_state.grid_lines = !gui_state.grid_lines;
        }

        //create particles on click
        if (!menu_mode || GetMouseX() > 220) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Particle part;
                part.colour = gui_state.selected_col;
                Vector2 pos = GetMousePosition();
                part.position = Vf2D{pos.x, pos.y} / cell_size;
                current_particles->particles.push_back(part);
                next_particles->particles.push_back(part);
            }
        }

        if (IsKeyPressed(KEY_C)) gui_state.clear = true;
        if (gui_state.clear)
        {
            *current_particles = {};
            *next_particles = {};
            gui_state.clear = false;
        } 

        if (IsKeyPressed(KEY_TAB)) menu_mode = !menu_mode;

        if (IsKeyPressed(KEY_SPACE)) gui_state.pause = !gui_state.pause;
        if (!gui_state.pause) {
            update(
                current_particles, next_particles,
                colour_attraction,
                GetFrameTime(),
                *gui_state.neighbour_range, *gui_state.repulsion_range
            );
            current_particles->particles = next_particles->particles;
        }

        
        if (IsKeyPressed(KEY_ESCAPE)) {
            menu_mode = false;
            // TODO: Do the return next turn after a draw to remove the menu
            return;
        }
	}
}

void cellularGame(
    float colour_attraction[NUM_COLOURS][NUM_COLOURS],
    int *repulsion_range,
    int *neighbour_range
) {
    Grid grid_a = {};
    Grid grid_b = {};
    
    Grid* current_grid = &grid_a;
    Grid* next_grid = &grid_b;

    GUIState gui_state = GUIState{0};
    initGuiState(gui_state, colour_attraction, repulsion_range, neighbour_range);
    bool menu_mode = false;

    float dt_acc = 0;

	// game loop
	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BLACK);

        draw(current_grid);
        if (gui_state.grid_lines) drawGridLines();

        if (menu_mode) guiPanel(gui_state, colour_attraction);
        else DrawText("Press TAB to open menu", 10, 10, 20, GRAY);

		EndDrawing();

        //Update
        if (IsKeyPressed(KEY_G)) {
            gui_state.grid_lines = !gui_state.grid_lines;
        }

        if (!menu_mode || GetMouseX() > 220) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                current_grid->colour[mouseToGrid()] = gui_state.selected_col;
            } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                current_grid->colour[mouseToGrid()] = CellColour::Blank;
            }
        }

        if (IsKeyPressed(KEY_C)) gui_state.clear = true;
        if (gui_state.clear) {
            *current_grid = {};
            gui_state.clear = false;
        }

        if (IsKeyPressed(KEY_TAB)) menu_mode = !menu_mode;

        if (IsKeyPressed(KEY_SPACE)) gui_state.pause = !gui_state.pause;
        if (!gui_state.pause) {
            if(dt_acc >= UPDATE_THRESHOLD)
            {
                dt_acc -= UPDATE_THRESHOLD;
                update(
                    current_grid, next_grid, colour_attraction, GetFrameTime(),
                    *gui_state.neighbour_range, *gui_state.repulsion_range
                );
                std::swap(current_grid, next_grid);
                *next_grid = {};
            }
            dt_acc += GetFrameTime();
        }

        // Manual stepping
        if (gui_state.pause) {
            if (IsKeyPressed(KEY_RIGHT)) {
                update(
                    current_grid, next_grid, colour_attraction, GetFrameTime(),
                    *gui_state.neighbour_range, *gui_state.repulsion_range
                );
                std::swap(current_grid, next_grid);
                *next_grid = {};
            }
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            menu_mode = false;
            // TODO: Do the return next turn after a draw to remove the menu
            return;
        }
	}
}

int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(1000, 1000, "Cellular Life");
    SetExitKey(KEY_NULL);

    //Coefficients of attraction of each pair of colours (default 0)
    float colour_attraction[NUM_COLOURS][NUM_COLOURS] = {};
    for (int x = 0; x < NUM_COLOURS; x++) {
        for (int y = 0; y < NUM_COLOURS; y++) {
            colour_attraction[x][y] = (x == y);
        }
    }
    int neighbour_range = 16;
    int repulsion_range = 2;

    // Ask the user which game they want to start particle or cellular
    while (!WindowShouldClose()) {
        // Having these two while loops with the same exit condition may seem strange
        // but we need to ensure that when in the inner loop (the main menu)
        // that we can exit properly, and when in a game that exiting works also

        bool cellular_game = false;
        while (!WindowShouldClose()) {
            bool mouse_left = GetMouseX() < GetScreenWidth() / 2;

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (mouse_left) {
                    cellular_game = true;
                }
                break;
            }

            BeginDrawing();
            DrawText("Click on a game to start", GetScreenWidth()/2 - 200, 20, 32, WHITE);
            DrawText("Cellular", 100, GetScreenHeight() / 2, 32, mouse_left ? BLUE : WHITE);
            DrawText("Particle", GetScreenWidth()/2 + 100, GetScreenHeight() / 2, 32, !mouse_left ? RED : WHITE);
            EndDrawing();
        }

        // Deal with when we want to exit from the main menu
        if (WindowShouldClose()) {
            CloseWindow();
            return 0;
        }

        if (cellular_game) {
            cellularGame(colour_attraction, &repulsion_range, &neighbour_range);
        } else {
            particleGame(colour_attraction, &repulsion_range, &neighbour_range);
        }
    }

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
