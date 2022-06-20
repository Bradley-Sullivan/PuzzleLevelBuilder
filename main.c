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
#include <string.h>
#include <raylib.h>
// uncomment if Linux compilation is borked
// #include "include/raylib.h"
#include "menu.h"
#include "param.h"

/*
    Want to implement a system for indexing into tile attr array.
    Thinking of using a master macro system to easily ID seemingly
    arbitrary attr. array indices.

    Tile attr's need to be SOLID cuz editing attribute indices with
    that system is a pain in the fuck
*/

typedef struct Tile {
    char* tileID;

    // bool playerColl;        // has player collision
    // bool entityColl;        // has entity collision
    // bool projectileColl;    // has proj. collision
    // bool moveable;          // player-mutable position
    // bool pSpawn;            // player initial spawn point
    // bool eSpawn;            // entity spawn point
    // bool levelEnd;          // level end goal
    // bool teleporter;        // teleporter tile
    // bool oneWayTeleporter;  // one-way teleporter tile flag (teleporter flag gets inverted once tele is taken)
    
    // int texIndex;           // tex-sheet index
    // int row;                // tile's row-position in level grid
    // int col;                // tile's column-position in level grid
    // int eSpawnChannel;
    // int teleChannel;        // teleporter channel
    
    int attr[14];           // tile's attributes

} Tile;

typedef struct Entity {
    char entityID;              // entity identifier used for quickly handling entity-related events?

    // bool isHostile;             // enemy/entity which does directly harm
    // bool isPassive;             // enemy/entity which does not directly harm
    // bool isActive;              // basic state variable
    // bool isTriggerHead;         // is main point of trigger for other entities on same channel
    // bool isTextured;            // is not blank texture
    // bool playerColl;            // has player collision
    // bool entityColl;            // has base entity collision
    // bool projectileColl;        // has proj. collision
    // bool activeInteract;        // interactable with player "use" key
    // bool passiveInteract;       // interactable with player collision

    // int effectMag;              // gen. purpose effect magnitude var.
    // int dirX;                   // projectile/enemy/etc. x sign
    // int dirY;                   // projectile/enemy/etc. y sign
    // int texIndex;               // texture to be drawn passively (i.e. by default)
    // int activeTexIndex;         // texture to be drawn on true active state
    // int triggerChannel;         // default trigger channel -> -1

    // double posX;                // entity x-position
    // double posY;                // entity y-position
    // double moveSpeed;           // entity moving step value

    int attr[19];

    Rectangle collisionRec;     // entity "hitbox" used for judging collisions

} Entity;

typedef struct Level {
    char* levelID;

    int numRows;
    int numCols;
    int baseFloorTexIndex;

    Tile** tiles;
} Level;

typedef struct Workspace {
    Level levels[MAX_NUM_LEVELS];

    bool editingTile;

    int activeEditLevel;
    int nextNewLevel;

    int numTileTex;
    int numEntityTex;
    int numOtherTex;

    int cursorRow;
    int cursorCol;

    // There's more to this struct, but I'm not sure how to handle it yet.
    Texture2D* tileTex;
    Texture2D* entityTex;
    Texture2D* otherTex;

    Texture2D cursorTex;

} Workspace;

typedef enum BuildState {
    MAIN_MENU,
    NEW_LEVEL,
    LOAD_LEVEL,
    EDITING,
    SAVE_EXPORT,
    HELP,
    EXIT
} BuildState;

void initWorkspace(Workspace* w);

bool initLevel(Level* l, char* id, int texIdx, int r, int c);
void previewTextures(Workspace* w, int tex);
bool loadTextures(Workspace* w);
int loadTexHelper(Texture2D dest[], char* dir);

void renderWorkspace(Workspace* w);
void drawTileAttr(Tile t, double x, double y);

BuildState mainMenuScreen(Menu* m);
BuildState levelInitConfig(Menu* m, Workspace* w);
BuildState editingLoop(Workspace* w, Menu* editContextMenu);

int main(void) {
    Workspace editWorkspace;
    BuildState state = MAIN_MENU;
    Menu mainMenu, levelConfMenu, editContextMenu;

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "test");
    ToggleFullscreen();
    SetTargetFPS(60);

    initWorkspace(&editWorkspace);

    char mainMenuSel[4][MAX_MENU_LEN] = {"NEW", "LOAD", "HELP", "EXIT"};
    int mainMenuTypes[4] = {SIMPLE_MENU, SIMPLE_MENU, SIMPLE_MENU, SIMPLE_MENU};
    initMenu(&mainMenu, 4, 0, 30, mainMenuSel, mainMenuTypes, false);

    char levelConfMenuSel[5][MAX_MENU_LEN] = {"Level ID", "Rows", "Columns", "Init Floor Tex.", "CONFIRM"};
    int levelConfMenuTypes[5] = {TEXT_ENTRY, PLUS_MINUS_MENU, PLUS_MINUS_MENU, PLUS_MINUS_MENU, SIMPLE_MENU};
    initMenu(&levelConfMenu, 5, 0, 20, levelConfMenuSel, levelConfMenuTypes, false);

    char editContextMenuSel[16][MAX_MENU_LEN] = {"TileID", "Player Coll.", "Entity Coll.", "Proj. Coll.", 
                                                "Moveable", "Player Spawn", "Entity Spawn", "Level End", 
                                                "Teleporter", "1-Way Tele.", "Tex. Index", "Row Pos.", 
                                                "Col Pos.", "Entity Sp. Channel", "Tele Channel", "CONFIRM"};
    int editContextMenuTypes[16] = {TEXT_ENTRY, CHECKLIST_MENU, CHECKLIST_MENU, CHECKLIST_MENU, 
                                    CHECKLIST_MENU, CHECKLIST_MENU, CHECKLIST_MENU, CHECKLIST_MENU, 
                                    CHECKLIST_MENU, CHECKLIST_MENU, PLUS_MINUS_MENU, DISPLAY_VAL, 
                                    DISPLAY_VAL, PLUS_MINUS_MENU, PLUS_MINUS_MENU, SIMPLE_MENU};
    initMenu(&editContextMenu, 16, 0, 15, editContextMenuSel, editContextMenuTypes, true);

    while (!WindowShouldClose() && state != EXIT) {
        BeginDrawing();
            ClearBackground((Color){25, 25, 25, 250});
            DrawRectangle(0, 0, EDIT_WIDTH, EDIT_HEIGHT, DARKGRAY);
            switch (state) {
                case MAIN_MENU:
                    state = mainMenuScreen(&mainMenu);
                    break;
                case NEW_LEVEL:
                    state = levelInitConfig(&levelConfMenu, &editWorkspace);
                    break;
                case LOAD_LEVEL:
                    break;
                case EDITING:
                    state = editingLoop(&editWorkspace, &editContextMenu);
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

void initWorkspace(Workspace* w) {
    loadTextures(w);

    w->editingTile = false;
    w->activeEditLevel = 0;
    w->nextNewLevel = 0;
    w->cursorCol = 0;
    w->cursorRow = 0;
}

bool initLevel(Level* l, char* id, int texIdx, int r, int c) {
    if (r <= 0 || c <= 0) {
        printf("\nERROR: invalid level dimensions\n\n");
        return false;
    } else if (texIdx < 0) {
        printf("\nERROR: invalid texture index\n\n");
        return false;
    } else {
        l->levelID = (char*)malloc(sizeof(char) * MAX_LEVEL_ID_LEN);
        for (int i = 0; i < MAX_LEVEL_ID_LEN; i++) l->levelID[i] = id[i];

        l->numRows = r;
        l->numCols = c;
        l->baseFloorTexIndex = texIdx;

        l->tiles = (Tile**)malloc(sizeof(Tile*) * r);
        for (int i = 0; i < r; i++) {
            l->tiles[i] = (Tile*)malloc(sizeof(Tile) * c);
        }

        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                l->tiles[i][j].tileID = (char*)malloc(sizeof(char) * MAX_TILE_ID_LEN);
                for (int k = 0; k < MAX_TILE_ID_LEN; k++) l->tiles[i][j].tileID[k] = '\0';
                for (int k = 0; k < 14; k++) l->tiles[i][j].attr[k] = 0;
                l->tiles[i][j].attr[T_TEXTURE_IDX] = texIdx;
                l->tiles[i][j].attr[T_ROW] = i;
                l->tiles[i][j].attr[T_COL] = j;
            }
        }

        printf("Initialized Level: %s\n", l->levelID);
        printf("\tRows: %d\n", l->numRows);
        printf("\tColumns: %d\n", l->numCols);
        printf("\tBase Floor Tex Index: %d\n", l->baseFloorTexIndex);

    }
    
    return true;
}

void previewTextures(Workspace* w, int tex) {
    int xOffset, yOffset;
    
    DrawRectangle((TILE_PIX_WIDTH + 20) * (tex + 1) - 5, (TILE_PIX_HEIGHT + 10) * ((tex + 5) / 5) - 5, TILE_PIX_WIDTH + 10, TILE_PIX_HEIGHT + 10, RED);

    for (int i = 0; i < w->numTileTex; i++) {
        xOffset = (TILE_PIX_WIDTH + 20) * (i + 1);
        yOffset = (TILE_PIX_HEIGHT + 10) * ((i + 5) / 5);
        DrawTexture(w->tileTex[i], xOffset, yOffset, WHITE);
    }
}

bool loadTextures(Workspace* w) {
    w->cursorRow = 0;
    w->cursorCol = 0;

    w->tileTex = (Texture2D*)malloc(sizeof(Texture2D) * MAX_NUM_TEX);
    w->entityTex = (Texture2D*)malloc(sizeof(Texture2D) * MAX_NUM_TEX);
    w->otherTex = (Texture2D*)malloc(sizeof(Texture2D) * MAX_NUM_TEX);

    w->cursorTex = LoadTexture(CURSOR_FILEPATH);

    w->numTileTex = loadTexHelper(w->tileTex, TILE_DIRECTORY);
    w->numEntityTex = loadTexHelper(w->entityTex, ENTITY_DIRECTORY);
    w->numOtherTex = loadTexHelper(w->otherTex, OTHER_DIRECTORY);

    printf("%d tile textures loaded\n", w->numTileTex);
    printf("%d entity textures loaded\n", w->numEntityTex);
    printf("%d other textures loaded\n", w->numOtherTex);

    return true;
}

int loadTexHelper(Texture2D dest[], char* dir) {
    char** texNames;
    int numTex, curTex = 0;

    texNames = (char**)malloc(sizeof(char*) * MAX_NUM_TEX);
    for (int i = 0; i < MAX_NUM_TEX; i++) {
        texNames[i] = (char*)malloc(sizeof(char) * MAX_FILENAME_LEN);
    }

    texNames = GetDirectoryFiles(dir, &numTex);
    ChangeDirectory(dir);

    for (int i = 0; i < numTex; i++) {
        if (IsFileExtension(texNames[i], ".png")) {
            dest[curTex] = LoadTexture(texNames[i]);
            curTex++;
        }
    }

    return curTex;
}

void renderWorkspace(Workspace* w) {
    int rows = w->levels[w->activeEditLevel].numRows;
    int cols = w->levels[w->activeEditLevel].numCols;
    int xOffset, yOffset;
    double curX = (EDIT_WIDTH / 2) - (TILE_PIX_WIDTH / 2);
    double curY = (EDIT_HEIGHT / 2) - (TILE_PIX_HEIGHT / 2);

    for (int i = 0; i < rows; i++) {
        for (int k = 0; k < cols; k++) {
            xOffset = (EDIT_WIDTH / 2) - ((w->cursorCol - k) * TILE_PIX_WIDTH) - (TILE_PIX_WIDTH / 2);
            yOffset = (EDIT_HEIGHT / 2) - ((w->cursorRow - i) * TILE_PIX_HEIGHT) - (TILE_PIX_HEIGHT / 2);
            int tex = w->levels[w->activeEditLevel].tiles[i][k].attr[T_TEXTURE_IDX];
            if (xOffset < EDIT_WIDTH - (TILE_PIX_WIDTH / 2)) {
                DrawTexture(w->tileTex[tex], xOffset, yOffset, WHITE);
            }
        }
    }

    DrawTexture(w->cursorTex, curX, curY, WHITE);
}

void drawTileAttr(Tile t, double x, double y) {
    static Menu dispMenu;
    static char attr[15][MAX_MENU_LEN] = {"TileID", "Player Coll.", "Entity Coll.", "Proj. Coll.", 
                                   "Moveable", "Player Spawn", "Entity Spawn", "Level End", 
                                   "Teleporter", "1-Way Tele.", "Tex. Index", "Row Pos.", 
                                   "Col Pos.", "Entity Sp. Channel", "Tele Channel"};
    static int attrMenTypes[15] = {TEXT_ENTRY, CHECKLIST_MENU, CHECKLIST_MENU, CHECKLIST_MENU, 
                            CHECKLIST_MENU, CHECKLIST_MENU, CHECKLIST_MENU, CHECKLIST_MENU, 
                            CHECKLIST_MENU, CHECKLIST_MENU, DISPLAY_VAL, DISPLAY_VAL, 
                            DISPLAY_VAL, DISPLAY_VAL, DISPLAY_VAL};
    initMenu(&dispMenu, 15, 0, 20, attr, attrMenTypes, false);

    for (int i = 0; i < MAX_TEXT_ENTRY_LEN; i++) dispMenu.tBox[0].text[i] = t.tileID[i];

    for (int i = 0; i < 14; i++) {
        dispMenu.menuVals[i + 1] = t.attr[i];
    }

    drawMenu(&dispMenu);
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

BuildState levelInitConfig(Menu* m, Workspace *w) {
    drawMenu(m);

    previewTextures(w, m->menuVals[3]);

    switch (m->cursor) {
        case 0:
            traverseMenu(m, TEXT_ENTRY);
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
            if (m->menuVals[3] < 0) m->menuVals[3] = 0;
            if (m->menuVals[3] >= w->numTileTex) m->menuVals[3] = w->numTileTex - 1;
            break;
        case 4:
            if (traverseMenu(m, SIMPLE_MENU) == KEY_ENTER) {
                if (initLevel(&w->levels[w->nextNewLevel], m->tBox[0].text, m->menuVals[3], m->menuVals[1], m->menuVals[2])) {
                    printf("Successfully initialized level\n");
                    printf("\tLevel ID: %s\n", w->levels[w->nextNewLevel].levelID);
                    printf("\tLevel Rows: %d\n", w->levels[w->nextNewLevel].numRows);
                    printf("\tLevel Cols: %d\n", w->levels[w->nextNewLevel].numCols);
                    printf("\tLevel Tile Texture: %d\n", w->levels[w->nextNewLevel].baseFloorTexIndex);
                    w->nextNewLevel++;
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

BuildState editingLoop(Workspace* w, Menu* editContextMenu) {
    renderWorkspace(w);

    if (w->editingTile) {
        int trVal = traverseMenu(editContextMenu, editContextMenu->menuTypes[editContextMenu->cursor]);
        drawMenu(editContextMenu);

        if (trVal == KEY_ENTER && editContextMenu->cursor == editContextMenu->numSel - 1) {
            w->editingTile = false;
            
            for (int i = 1; i < 15; i++) w->levels[w->activeEditLevel].tiles[w->cursorRow][w->cursorCol].attr[i - 1] = editContextMenu->menuVals[i];
            for (int i = 0; i < MAX_TILE_ID_LEN; i++) w->levels[w->activeEditLevel].tiles[w->cursorRow][w->cursorCol].tileID[i] = editContextMenu->tBox[0].text[i];
            resetMenu(editContextMenu);
        } else if (editContextMenu->cursor == T_TEXTURE_IDX + 1) {
            if (editContextMenu->menuVals[T_TEXTURE_IDX + 1] > w->numTileTex - 1) {
                editContextMenu->menuVals[T_TEXTURE_IDX + 1] = w->numTileTex - 1;
            } else if (editContextMenu->menuVals[T_TEXTURE_IDX + 1] < 0) {
                editContextMenu->menuVals[T_TEXTURE_IDX + 1] = 0;
            }
            previewTextures(w, editContextMenu->menuVals[T_TEXTURE_IDX + 1]);
        }

    } else {
        drawTileAttr(w->levels[w->activeEditLevel].tiles[w->cursorRow][w->cursorCol], 0, 0);

        if (IsKeyPressed(KEY_RIGHT)) {
            w->cursorCol++;
            if (w->cursorCol >= w->levels[w->activeEditLevel].numCols) {
                w->cursorCol = 0;
            }
        } else if (IsKeyPressed(KEY_LEFT)) {
            w->cursorCol--;
            if (w->cursorCol < 0) {
                w->cursorCol = w->levels[w->activeEditLevel].numCols - 1;
            }
        } else if (IsKeyPressed(KEY_UP)) {
            w->cursorRow--;
            if (w->cursorRow < 0) {
                w->cursorRow = w->levels[w->activeEditLevel].numRows - 1;
            }
        } else if (IsKeyPressed(KEY_DOWN)) {
            w->cursorRow++;
            if (w->cursorRow >= w->levels[w->activeEditLevel].numRows) {
                w->cursorRow = 0;
            }
        } else if (IsKeyPressed(KEY_SPACE)) {
            if (!w->editingTile) {
                w->editingTile = true;

                // copy current tile attr. into menu vals
                editContextMenu->cursor = 0;
                for (int i = 1; i < 15; i++) {
                    editContextMenu->menuVals[i] = w->levels[w->activeEditLevel].tiles[w->cursorRow][w->cursorCol].attr[i - 1];
                }

                for (int i = 0; i < MAX_TILE_ID_LEN; i++) {
                    editContextMenu->tBox[0].text[i] = w->levels[w->activeEditLevel].tiles[w->cursorRow][w->cursorCol].tileID[i];
                }
            }
        }
    }

    

    return EDITING;
}

