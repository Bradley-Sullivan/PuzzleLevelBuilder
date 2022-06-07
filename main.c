/**
 * @file main.c
 * @author Bradley Sullivan (bradleysullivan@nevada.unr.edu)
 * @brief Main level editor entry point.
 * @version 0.1
 * @date 2022-06-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "include/raylib.h"
#include "menu.h"
#include "drawParam.h"

#define MAX_NUM_LEVELS      128
#define MAX_LEVEL_ROWS      64
#define MAX_LEVEL_COLS      64


typedef struct {
    char tileID;

    bool playerColl;        // has player collision
    bool entityColl;        // has entity collision
    bool projectileColl;    // has proj. collision
    bool moveable;          // player-mutable position
    bool pSpawn;            // player initial spawn point
    bool eSpawn;            // entity spawn point
    bool levelEnd;          // level end goal
    bool teleporter;        // teleporter tile
    bool oneWayTeleporter;  // one-way teleporter tile flag (teleporter flag gets inverted once tele is taken)
    
    int texIndex;           // tex-sheet index
    int row;                // tile's row-position in level grid
    int col;                // tile's column-position in level grid
    int teleChannel;        // teleporter channel
    
} Tile;

typedef struct {
    char entityID;              // entity identifier used for quickly handling entity-related events?

    bool isHostile;             // enemy/entity which does directly harm
    bool isPassive;             // enemy/entity which does not directly harm
    bool isActive;              // basic state variable
    bool isTriggerHead;         // is main point of trigger for other entities on same channel
    bool isTextured;            // is not blank texture
    bool playerColl;            // has player collision
    bool entityColl;            // has base entity collision
    bool projectileColl;        // has proj. collision
    bool activeInteract;        // interactable with player "use" key
    bool passiveInteract;       // interactable with player collision

    int effectMag;              // gen. purpose effect magnitude var.
    int dirX;                   // projectile/enemy/etc. x sign
    int dirY;                   // projectile/enemy/etc. y sign
    int texIndex;               // texture to be drawn passively (i.e. by default)
    int activeTexIndex;         // texture to be drawn on true active state
    int triggerChannel;         // default trigger channel -> -1

    double posX;                // entity x-position
    double posY;                // entity y-position
    double moveSpeed;           // entity moving step value

    Rectangle collisionRec;     // entity "hitbox" used for judging collisions

} Entity;

typedef struct {
    char* levelID;

    int numRows;
    int numCols;
    int baseFloorTexIndex;

    Tile** tiles;
} Level;

typedef struct {
    Level levels[MAX_NUM_LEVELS];

    int activeEditLevel;

    // There's more to this struct, but I'm not sure how to handle it yet.

} Workspace;

typedef enum {
    MAIN_MENU,
    NEW_LEVEL,
    LOAD_LEVEL,
    EDITING,
    SAVE_EXPORT,
    HELP,
    EXIT
} BuildState;

bool initLevel(Level* l, char* id, int texIdx, int r, int c);

BuildState mainMenuScreen(Menu* m);
BuildState levelInitConfig(Menu* m, TextBox* t, Level* l);
BuildState editingLoop();

int main(void) {
    BuildState state = MAIN_MENU;
    Menu mainMenu, levelConfMenu;
    TextBox levelIDTextBox;
    Level levelWorkspace[MAX_NUM_LEVELS];

    int activeEditLevel = -1;

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "test");
    ToggleFullscreen();
    SetTargetFPS(60);

    char mainMenuSel[4][MAX_MENU_LEN] = {"NEW", "LOAD", "HELP", "EXIT"};
    int mainMenuTypes[4] = {SIMPLE_MENU, SIMPLE_MENU, SIMPLE_MENU, SIMPLE_MENU};
    initMenu(&mainMenu, 4, 0, 30, mainMenuSel, mainMenuTypes);

    char levelConfMenuSel[5][MAX_MENU_LEN] = {"Level ID", "Rows", "Columns", "Init Floor Tex.", "CONFIRM"};
    int levelConfMenuTypes[5] = {TEXT_ENTRY, PLUS_MINUS_MENU, PLUS_MINUS_MENU, PLUS_MINUS_MENU, SIMPLE_MENU};
    initTextBox(&levelIDTextBox, MAX_LEVEL_ID_LEN, 20, 0, EDIT_WIDTH - 20 * MAX_LEVEL_ID_LEN, 20);
    initMenu(&levelConfMenu, 5, 0, 20, levelConfMenuSel, levelConfMenuTypes);

    while (!WindowShouldClose() && state != EXIT) {
        BeginDrawing();
            ClearBackground((Color){25, 25, 25, 250});
            DrawRectangle(0, 0, EDIT_WIDTH, EDIT_HEIGHT, DARKGRAY);
            switch (state) {
                case MAIN_MENU:
                    state = mainMenuScreen(&mainMenu);
                    break;
                case NEW_LEVEL:
                    activeEditLevel++;
                    state = levelInitConfig(&levelConfMenu, &levelIDTextBox, &levelWorkspace[activeEditLevel]);
                    break;
                case LOAD_LEVEL:
                    break;
                case EDITING:
                    break;
                case SAVE_EXPORT:
                    break;
                case HELP:
                    break;
                case EXIT:
                    printf("exiting\n");
                    break;
            }
        EndDrawing();
    }
    return 0;
}

bool initLevel(Level* l, char* id, int texIdx, int r, int c) {
    if (r <= 0 || c <= 0) {
        printf("invalid level dimensions\n");
        return false;
    } else if (texIdx < 0) {
        printf("invalid texture index\n");
        return false;
    } else {
       l->levelID = id;

        l->numRows = r;
        l->numCols = c;
        l->baseFloorTexIndex = texIdx;

        l->tiles = (Tile**)malloc(sizeof(Tile*) * r);
        for (int i = 0; i < r; i++) {
            l->tiles[i] = (Tile*)malloc(sizeof(Tile) * c);
        }
    }
    
    return true;
}

BuildState mainMenuScreen(Menu *m) {
    drawMenu(m);

    switch (m->cursor) {
        case 0:
            if (traverseMenu(m, SIMPLE_MENU) == KEY_ENTER) {
                return NEW_LEVEL;
            }
            break;
        case 1:
            if (traverseMenu(m, SIMPLE_MENU) == KEY_ENTER) {
                return LOAD_LEVEL;
            }
            break;
        case 2:
            if (traverseMenu(m, SIMPLE_MENU) == KEY_ENTER) {
                return HELP;
            }
            break;
        case 3:
            if (traverseMenu(m, SIMPLE_MENU) == KEY_ENTER) {
                return EXIT;
            }
            break;
        default:
            break;
    }

    return MAIN_MENU;
}

BuildState levelInitConfig(Menu* m, TextBox* t, Level* l) {
    drawMenu(m);

    switch (m->cursor) {
        case 0:
            if (!t->editing) {
                if (traverseMenu(m, SIMPLE_MENU) == KEY_ENTER) {
                    t->editing = true;
                }
                drawTextBox(t, false);
            } else {
                editTextBox(t);
                drawTextBox(t, true);
            }
            break;
        case 1:
            traverseMenu(m, PLUS_MINUS_MENU);
            if (m->menuVals[1] > MAX_LEVEL_ROWS) m->menuVals[1] = MAX_LEVEL_ROWS;
            break;
        case 2:
            traverseMenu(m, PLUS_MINUS_MENU);
            if (m->menuVals[2] > MAX_LEVEL_COLS) m->menuVals[2] = MAX_LEVEL_COLS;
            break;
        case 3:
            traverseMenu(m, PLUS_MINUS_MENU);
            break;
        case 4:
            if (traverseMenu(m, SIMPLE_MENU) == KEY_ENTER) {
                if (initLevel(l, t->text, m->menuVals[3], m->menuVals[1], m->menuVals[2])) {
                    return EDITING;
                } else {
                    return MAIN_MENU;
                }
            }
            break;
        default:
            break;
    }

    return NEW_LEVEL;
}

