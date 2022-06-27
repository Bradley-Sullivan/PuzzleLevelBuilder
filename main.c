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
    Need to figure out entity rendering and how changing entities
    attr. is handled within the editingLoop menu-ing and whatnot

    Need to figure out general "SAVE", "QUIT", "Level Switch", etc.
    menu-ing within level navigation in editingLoop

    Need to think more about entities and how they will relate to
    the tile grid in terms of position (top left, centered, etc)

    2-D ent. grid?? probably unnecessary

    Entity is animated and other animation shit
        entity animation array
        use other folder with separate animation directories where each frame filename is numbered
        PREVIEW ANIMATION FUNCTION!! with similar texture selection menu/submenu
        need anim. period in milliseconds

    NEED EFFICIENT HASHING FUNCTION FOR PICKING ENTITY INDEX FROM WKSPC. CURSOR COORDS!!!
    HOW TO HANDLE MULTIPLE-ENTITY-PER-TILE-CONFLICTS? IDFK. ENTITY IDs ?
        heap style conflict resolution i.e. picking the next open spot?
        differentiate conflicts by id?
        work scaled-modulo 4 for even/odd row/columns, scaled with row mod 4?
            i.e. 0 = even-even, 1 = even-odd, 2 = odd-even, 3 = odd-odd
        hashed into (CUR-ROW * n) + k for 0 <= k <= #-COL, n = #-ROW???
        better hashing if we also take into account the # of entities currently residing on a tile?
        3-D array??
    Pass an Entity pointer to a multipleEntityEval function which counts and allocates
    space in an array then returns true upon multiple whatevers on one tile. use this array
    to fill menu selection titles with entity ids!! and selections are then hashed into level
    entity array ??
*/

void initWorkspace(Workspace* w);

bool initLevel(Level* l, char* id, int texIdx, int r, int c);
void previewTextures(Workspace* w, int tex, TexType type);
bool loadTextures(Workspace* w);
int loadTexHelper(Texture2D dest[], char* dir);
int getEntIdx(Level* l, char* id, int row, int col);
int getNumEntCollisions(Level* l, int row, int col);
bool getEntCollisionIDs(Level* l, char** ids, int row, int col);

void renderWorkspace(Workspace* w);
void drawTileAttr(Tile t, double x, double y);

int editSelect();
int entityCollisionSelect(char** entityIDs, int numCollisions);
void tileEdit(Workspace* w, Menu* editTileContextMenu);
void entityEdit(Workspace* w, Menu* editEntityContextMenu, bool newEntity);

BuildState mainMenuScreen(Menu* m);
BuildState levelInitConfig(Menu* m, Workspace* w);
BuildState editAltMenu(Menu* editSubMenu);
BuildState editingLoop(Workspace* w, Menu* editTileContextMenu, Menu* editEntityContextMenu, Menu* editSubMenu);

int main(void) {
    Workspace editWorkspace;
    BuildState state = MAIN_MENU;
    Menu mainMenu, levelConfMenu, editTileContextMenu, editEntityContextMenu, editSubMenu;

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "test");
    ToggleFullscreen();
    SetTargetFPS(60);

    initWorkspace(&editWorkspace);

    char mainMenuSel[4][MAX_MENU_LEN] = {"NEW", "LOAD", "HELP", "EXIT"};
    int mainMenuTypes[4] = {SIMPLE_MENU, SIMPLE_MENU, SIMPLE_MENU, SIMPLE_MENU};
    initMenu(&mainMenu, 4, 0, MAIN_MENU_FS, mainMenuSel, mainMenuTypes, false);

    char levelConfMenuSel[5][MAX_MENU_LEN] = {"Level ID", "Rows", "Columns", "Init Floor Tex.", "CONFIRM"};
    int levelConfMenuTypes[5] = {TEXT_ENTRY, PLUS_MINUS_MENU, PLUS_MINUS_MENU, PLUS_MINUS_MENU, SIMPLE_MENU};
    initMenu(&levelConfMenu, 5, 0, SUB_MENU_FS, levelConfMenuSel, levelConfMenuTypes, false);

    char editTileContextMenuSel[T_NUM_ATTR + 2][MAX_MENU_LEN] = {"TileID", "Player Coll.", "Entity Coll.", "Proj. Coll.", 
                                                "Moveable", "Player Spawn", "Entity Spawn", "Level End", 
                                                "Teleporter", "1-Way Tele.", "Tex. Index", "Row Pos.", 
                                                "Col Pos.", "Entity Sp. Channel", "Tele Channel", "CONFIRM"};
    int editTileContextMenuTypes[T_NUM_ATTR + 2] = {TEXT_ENTRY, CHECKLIST_MENU, CHECKLIST_MENU, CHECKLIST_MENU, 
                                    CHECKLIST_MENU, CHECKLIST_MENU, CHECKLIST_MENU, CHECKLIST_MENU, 
                                    CHECKLIST_MENU, CHECKLIST_MENU, PLUS_MINUS_MENU, DISPLAY_VAL, 
                                    DISPLAY_VAL, PLUS_MINUS_MENU, PLUS_MINUS_MENU, SIMPLE_MENU};
    initMenu(&editTileContextMenu, T_NUM_ATTR + 2, 0, CONTEXT_MENU_FS, editTileContextMenuSel, editTileContextMenuTypes, true);

    char editEntityContextMenuSel[E_NUM_ATTR + 2][MAX_MENU_LEN] = {"EntityID", "Hostile", "Passive", "Active", "Trigger Head",
                                                    "Is Textured", "Player Coll.", "Entity Coll.", "Proj. Coll.",
                                                    "Active Interact", "Passive Interact", "Effect Magnitude", "X Dir.",
                                                    "Y Dir.", "Tex. Index", "Active Tex. Index", "Trigger Channel", 
                                                    "X Position", "Y Position", "Move Speed", "Anim. Period", "CONFIRM"};
    int editEntityContextMenuTypes[E_NUM_ATTR + 2] = {TEXT_ENTRY, CHECKLIST_MENU, CHECKLIST_MENU, CHECKLIST_MENU, CHECKLIST_MENU,
                                        CHECKLIST_MENU, CHECKLIST_MENU, CHECKLIST_MENU, CHECKLIST_MENU, CHECKLIST_MENU,
                                        CHECKLIST_MENU, PLUS_MINUS_MENU, PLUS_MINUS_MENU, PLUS_MINUS_MENU, PLUS_MINUS_MENU,
                                        PLUS_MINUS_MENU, PLUS_MINUS_MENU, DISPLAY_VAL, DISPLAY_VAL, PLUS_MINUS_MENU,
                                        PLUS_MINUS_MENU, SIMPLE_MENU};
    initMenu(&editEntityContextMenu, E_NUM_ATTR + 2, 0, CONTEXT_MENU_FS, editEntityContextMenuSel, editEntityContextMenuTypes, true);

    char editSubMenuSel[5][MAX_MENU_LEN] = {"TOOLS", "SWITCH LEVEL", "SAVE", "SAVE & EXIT", "QUIT"};
    int editSubMenuTypes[5] = {SIMPLE_MENU, SIMPLE_MENU, SIMPLE_MENU, SIMPLE_MENU, SIMPLE_MENU};
    initMenu(&editSubMenu, 5, 0, SUB_MENU_FS, editSubMenuSel, editSubMenuTypes, false);

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
                case SWITCH_LEVEL:
                    break;
                case EDITING:
                    state = editingLoop(&editWorkspace, &editTileContextMenu, &editEntityContextMenu, &editSubMenu);
                    break;
                case SAVE_EXPORT:
                    break;
                case HELP:
                    break;
                case EXIT:
                    // save current workspace to file for save failsafe and recent edit session shtuff
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
    w->editingEntity = false;
    w->entityEditCollision = false;
    w->editingNewEntity = false;
    w->activeEditLevel = 0;
    w->nextNewLevel = 0;
    w->editingEntityIdx = -1;
    w->cursorCol = 0;
    w->cursorRow = 0;

    for (int i = 0; i < MAX_NUM_LEVELS; i++) {
        w->levels[i].initialized = false;
        w->levels[i].numCols = 0;
        w->levels[i].numRows = 0;
        w->levels[i].baseFloorTexIndex = 0;
    }
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

        l->ents = (Entity*)malloc(sizeof(Entity) * MAX_NUM_ENTITIES);
        for (int i = 0; i < MAX_NUM_ENTITIES; i++) {
            l->ents[i].existsInWorkspace = false;
            for (int k = 0; k < E_NUM_ATTR; k++) l->ents[i].attr[k] = 0;
            l->ents[i].entityID = (char*)malloc(sizeof(char) * MAX_ENT_ID_LEN);
            for (int k = 0; k < MAX_ENT_ID_LEN; k++) l->ents[i].entityID[k] = '\0';
        }

        l->tiles = (Tile**)malloc(sizeof(Tile*) * r);
        for (int i = 0; i < r; i++) {
            l->tiles[i] = (Tile*)malloc(sizeof(Tile) * c);
        }

        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                l->tiles[i][j].tileID = (char*)malloc(sizeof(char) * MAX_TILE_ID_LEN);
                for (int k = 0; k < MAX_TILE_ID_LEN; k++) l->tiles[i][j].tileID[k] = '\0';
                for (int k = 0; k < T_NUM_ATTR; k++) l->tiles[i][j].attr[k] = 0;
                l->tiles[i][j].attr[T_TEXTURE_IDX] = texIdx;
                l->tiles[i][j].attr[T_ROW] = i;
                l->tiles[i][j].attr[T_COL] = j;
            }
        }
        
        l->nextFreeEnt = 0;

        l->initialized = true;

        printf("Initialized Level: %s\n", l->levelID);
        printf("\tRows: %d\n", l->numRows);
        printf("\tColumns: %d\n", l->numCols);
        printf("\tBase Floor Tex Index: %d\n", l->baseFloorTexIndex);
    }
    
    return true;
}

void previewTextures(Workspace* w, int tex, TexType type) {
    int xOffset, yOffset;
    
    DrawRectangle((TILE_PIX_WIDTH + 20) * ((tex % 5) + 1) - 5, (TILE_PIX_HEIGHT + 10) * ((tex + 5) / 5) - 5, TILE_PIX_WIDTH + 10, TILE_PIX_HEIGHT + 10, RED);

    if (type == TILE_TEX) {
        for (int i = 0; i < w->numTileTex; i++) {
            xOffset = (TILE_PIX_WIDTH + 20) * ((i % 5) + 1);
            yOffset = (TILE_PIX_HEIGHT + 10) * ((i + 5) / 5);
            DrawTexture(w->tileTex[i], xOffset, yOffset, WHITE);
        }
    } else if (type == ENTITY_TEX) {
        for (int i = 0; i < w->numEntityTex; i++) {
            xOffset = (TILE_PIX_WIDTH + 20) * (i + 1);
            yOffset = (TILE_PIX_HEIGHT + 10) * ((i + 5) / 5);
            DrawTexture(w->entityTex[i], xOffset, yOffset, WHITE);
        }
    } else if (type == OTHER_TEX) {
        for (int i = 0; i < w->numOtherTex; i++) {
            xOffset = (TILE_PIX_WIDTH + 20) * (i + 1);
            yOffset = (TILE_PIX_HEIGHT + 10) * ((i + 5) / 5);
            DrawTexture(w->otherTex[i], xOffset, yOffset, WHITE);
        }
    }
    
}

bool loadTextures(Workspace* w) {
    const char* rootDir;

    w->cursorRow = 0;
    w->cursorCol = 0;

    w->tileTex = (Texture2D*)malloc(sizeof(Texture2D) * MAX_NUM_TEX);
    w->entityTex = (Texture2D*)malloc(sizeof(Texture2D) * MAX_NUM_TEX);
    w->otherTex = (Texture2D*)malloc(sizeof(Texture2D) * MAX_NUM_TEX);
    rootDir = (char*)malloc(sizeof(char) * 64);

    rootDir = GetWorkingDirectory();

    w->cursorTex = LoadTexture(CURSOR_FILEPATH);

    w->numTileTex = loadTexHelper(w->tileTex, TILE_DIRECTORY);
    ChangeDirectory(rootDir);
    w->numEntityTex = loadTexHelper(w->entityTex, ENTITY_DIRECTORY);
    ChangeDirectory(rootDir);
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
    // texNames = GetDirectoryFiles(dir, &numTex);
    texNames = LoadDirectoryFiles(dir, &numTex);
    ChangeDirectory(dir);

    for (int i = 0; i < numTex; i++) {
        if (IsFileExtension(texNames[i], ".png")) {
            dest[curTex] = LoadTexture(texNames[i]);
            curTex++;
        }
    }

    // ClearDirectoryFiles();
    UnloadDirectoryFiles();

    return curTex;
}

int getEntIdx(Level* l, char* id, int row, int col) {
    for (int i = 0; i < MAX_NUM_ENTITIES; i++) {
        if (l->ents[i].existsInWorkspace) {
            if (l->ents[i].attr[E_POS_X] / TILE_PIX_WIDTH == col) {
                if (l->ents[i].attr[E_POS_Y] / TILE_PIX_HEIGHT == row) {
                    if (strcmp(id, l->ents[i].entityID) == 0) {
                        return i;
                    }
                }
            }
        }
        
    }

    return -1;
}

int getNumEntCollisions(Level* l, int row, int col) {
    int entCollisionCt = 0;
    for (int i = 0; i < MAX_NUM_ENTITIES; i++) {
        if (l->ents[i].existsInWorkspace) {
            if (l->ents[i].attr[E_POS_X] / TILE_PIX_WIDTH == col) {
                if (l->ents[i].attr[E_POS_Y] / TILE_PIX_HEIGHT == row) {
                    entCollisionCt++;
                }
            }
        }        
    }

    return entCollisionCt;
}

bool getEntCollisionIDs(Level* l, char** ids, int row, int col) {
    int entCollisionCt = 0;
    for (int i = 0; i < MAX_NUM_ENTITIES; i++) {
        if (l->ents[i].existsInWorkspace) {
            if (l->ents[i].attr[E_POS_X] / TILE_PIX_WIDTH == col) {
                if (l->ents[i].attr[E_POS_Y] / TILE_PIX_HEIGHT == row) {
                    strcpy(ids[entCollisionCt], l->ents[i].entityID);
                    entCollisionCt++;
                }
            }
        }        
    }

    if (entCollisionCt > 0) {
        return true;
    } else {
        return false;
    }
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
            int tileTex = w->levels[w->activeEditLevel].tiles[i][k].attr[T_TEXTURE_IDX];
            if (xOffset < EDIT_WIDTH - (TILE_PIX_WIDTH / 2)) {
                DrawTexture(w->tileTex[tileTex], xOffset, yOffset, WHITE);
                
            }
        }
    }

    for (int i = 0; i < w->numEntityTex; i++) {        
        if (w->levels[w->activeEditLevel].ents[i].attr[E_IS_TEXTURED] == 1) {
            int entTex = w->levels[w->activeEditLevel].ents[i].attr[E_TEXTURE_IDX];
            int xPos = w->levels[w->activeEditLevel].ents[i].attr[E_POS_X];
            int yPos = w->levels[w->activeEditLevel].ents[i].attr[E_POS_Y];
            int gridX = xPos / TILE_PIX_WIDTH;
            int gridY = yPos / TILE_PIX_HEIGHT;
            double xOffset = (EDIT_WIDTH / 2) - ((w->cursorCol - gridX) * TILE_PIX_WIDTH) - (TILE_PIX_WIDTH / 2);
            double yOffset = (EDIT_HEIGHT / 2) - ((w->cursorRow - gridY) * TILE_PIX_HEIGHT) - (TILE_PIX_HEIGHT / 2);
            if (xOffset < EDIT_WIDTH - (TILE_PIX_WIDTH / 2)) {
                DrawTexture(w->entityTex[entTex], xOffset, yOffset, RAYWHITE);
            }
        }
    }

    DrawTexture(w->cursorTex, curX, curY, WHITE);
}

void drawTileAttr(Tile t, double x, double y) {
    static bool dispMenuInit = false;
    static Menu dispMenu;
    static char attr[15][MAX_MENU_LEN] = {"TileID", "Player Coll.", "Entity Coll.", "Proj. Coll.", 
                                   "Moveable", "Player Spawn", "Entity Spawn", "Level End", 
                                   "Teleporter", "1-Way Tele.", "Tex. Index", "Row Pos.", 
                                   "Col Pos.", "Entity Sp. Channel", "Tele Channel"};
    static int attrMenTypes[15] = {TEXT_ENTRY, CHECKLIST_MENU, CHECKLIST_MENU, CHECKLIST_MENU, 
                            CHECKLIST_MENU, CHECKLIST_MENU, CHECKLIST_MENU, CHECKLIST_MENU, 
                            CHECKLIST_MENU, CHECKLIST_MENU, DISPLAY_VAL, DISPLAY_VAL, 
                            DISPLAY_VAL, DISPLAY_VAL, DISPLAY_VAL};
    if (!dispMenuInit) {
        initMenu(&dispMenu, 15, 0, 20, attr, attrMenTypes, false);
        dispMenuInit = true;
    }

    for (int i = 0; i < MAX_TEXT_ENTRY_LEN; i++) dispMenu.tBox[0].text[i] = t.tileID[i];

    for (int i = 0; i < 14; i++) {
        dispMenu.menuVals[i + 1] = t.attr[i];
    }

    drawMenu(&dispMenu);
}

int editSelect() {
    static bool isInit = false;
    static Menu editSelectionMenu;
    static char editSelectionSel[3][MAX_MENU_LEN] = {"Edit/Add Entity", "Edit Tile", "BACK"};
    static int editSelectionTypes[3] = {SIMPLE_MENU, SIMPLE_MENU, SIMPLE_MENU};
    if (!isInit) {
        initMenu(&editSelectionMenu, 3, 0, 15, editSelectionSel, editSelectionTypes, true);
        isInit = true;
    }

    drawMenu(&editSelectionMenu);

    switch (traverseMenu(&editSelectionMenu, editSelectionMenu.menuTypes[editSelectionMenu.cursor])) {
        case KEY_ENTER:
            return editSelectionMenu.cursor;
        default:
            break;
    }

    return -1;
}

int entityCollisionSelect(char** entityIDs, int numCollisions) {
    static bool isInit = false;
    static Menu entityCollisionMenu;
    static char entityCollisionMenuSel[MAX_NUM_ENTITIES][MAX_MENU_LEN];
    static int entityCollisionMenuTypes[MAX_NUM_ENTITIES];

    if (!isInit) {
        for (int i = 0; i < numCollisions; i++) {
            strncpy(entityCollisionMenuSel[i], entityIDs[i], MAX_ENT_ID_LEN);
        }
        strncpy(entityCollisionMenuSel[numCollisions], "Add Entity", MAX_MENU_LEN);
        strncpy(entityCollisionMenuSel[numCollisions + 1], "BACK", MAX_MENU_LEN);
        for (int i = 0; i < numCollisions + 2; i++) entityCollisionMenuTypes[i] = SIMPLE_MENU;
        initMenu(&entityCollisionMenu, numCollisions + 2, 0, CONTEXT_MENU_FS, entityCollisionMenuSel, entityCollisionMenuTypes, true); 
        isInit = true;
    }

    drawMenu(&entityCollisionMenu);

    switch (traverseMenu(&entityCollisionMenu, entityCollisionMenu.menuTypes[entityCollisionMenu.cursor])) {
        case KEY_ENTER:
            isInit = false;
            return entityCollisionMenu.cursor;
        default:
            break;
    }

    return -1;
}

void tileEdit(Workspace* w, Menu* editTileContextMenu) {
    int trVal = traverseMenu(editTileContextMenu, editTileContextMenu->menuTypes[editTileContextMenu->cursor]);
    drawMenu(editTileContextMenu);

    if (trVal == KEY_ENTER && editTileContextMenu->cursor == editTileContextMenu->numSel - 1) {
        w->editingTile = false;
        
        for (int i = 1; i < 15; i++) w->levels[w->activeEditLevel].tiles[w->cursorRow][w->cursorCol].attr[i - 1] = editTileContextMenu->menuVals[i];
        for (int i = 0; i < MAX_TILE_ID_LEN; i++) w->levels[w->activeEditLevel].tiles[w->cursorRow][w->cursorCol].tileID[i] = editTileContextMenu->tBox[0].text[i];
        resetMenu(editTileContextMenu);
    } else if (editTileContextMenu->cursor == T_TEXTURE_IDX + 1) {

        if (editTileContextMenu->menuVals[T_TEXTURE_IDX + 1] > w->numTileTex - 1) {
            editTileContextMenu->menuVals[T_TEXTURE_IDX + 1] = w->numTileTex - 1;
        } else if (editTileContextMenu->menuVals[T_TEXTURE_IDX + 1] < 0) {
            editTileContextMenu->menuVals[T_TEXTURE_IDX + 1] = 0;
        }
        previewTextures(w, editTileContextMenu->menuVals[T_TEXTURE_IDX + 1], TILE_TEX);
    }
}

void entityEdit(Workspace* w, Menu* editEntityContextMenu, bool newEntity) {
    int trVal = traverseMenu(editEntityContextMenu, editEntityContextMenu->menuTypes[editEntityContextMenu->cursor]);
    int entEditIndex;

    drawMenu(editEntityContextMenu);

    // use newEntity boolean to switch between using level's next entity index and given 
    // entity index to edit.

    if (newEntity) {
        // use active edit level's next new entity index to edit
        entEditIndex = w->levels[w->activeEditLevel].nextFreeEnt;
    } else {
        // use workspaces edit entity index to edit
        entEditIndex = w->editingEntityIdx;
    }

    if (trVal == KEY_ENTER && editEntityContextMenu->cursor == editEntityContextMenu->numSel - 1) {
        w->editingEntity = false;
        for (int i = 1; i < E_NUM_ATTR; i++) {
            w->levels[w->activeEditLevel].ents[entEditIndex].attr[i - 1] = editEntityContextMenu->menuVals[i];
        }
        for (int i = 0; i < MAX_TILE_ID_LEN; i++) {
            w->levels[w->activeEditLevel].ents[entEditIndex].entityID[i] = editEntityContextMenu->tBox[0].text[i];
        }
        w->levels[w->activeEditLevel].ents[entEditIndex].existsInWorkspace = true;
        if (newEntity) w->levels[w->activeEditLevel].nextFreeEnt += 1;
        resetMenu(editEntityContextMenu);
    } else if (editEntityContextMenu->cursor == E_TEXTURE_IDX + 1) {
        if (editEntityContextMenu->menuVals[E_TEXTURE_IDX + 1] > w->numEntityTex - 1) {
            editEntityContextMenu->menuVals[E_TEXTURE_IDX + 1] = w->numEntityTex - 1;
        } else if (editEntityContextMenu->menuVals[E_TEXTURE_IDX + 1] < 0) {
            editEntityContextMenu->menuVals[E_TEXTURE_IDX + 1] = 0;
        }
        previewTextures(w, editEntityContextMenu->menuVals[E_TEXTURE_IDX + 1], ENTITY_TEX);
    } else if (editEntityContextMenu->cursor == E_ACTIVE_TEX_IDX + 1) {
        if (editEntityContextMenu->menuVals[E_ACTIVE_TEX_IDX + 1] > w->numEntityTex - 1) {
            editEntityContextMenu->menuVals[E_ACTIVE_TEX_IDX + 1] = w->numEntityTex - 1;
        } else if (editEntityContextMenu->menuVals[E_ACTIVE_TEX_IDX + 1] < 0) {
            editEntityContextMenu->menuVals[E_ACTIVE_TEX_IDX + 1] = 0;
        }
        previewTextures(w, editEntityContextMenu->menuVals[E_ACTIVE_TEX_IDX + 1], ENTITY_TEX);
    }
}

BuildState mainMenuScreen(Menu *m) {
    drawMenu(m);

    switch(traverseMenu(m, m->menuTypes[m->cursor])) {
        case KEY_ENTER:
            if (m->cursor == 0) {
                return NEW_LEVEL;
            } else if (m->cursor == 1) {
                return LOAD_LEVEL;
            } else if (m->cursor == 2) {
                return HELP;
            } else if (m->cursor == 3) {
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

    previewTextures(w, m->menuVals[3], TILE_TEX);

    switch (traverseMenu(m, m->menuTypes[m->cursor])) {
        case KEY_ENTER:
            if (m->cursor == 4) {
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
            if (m->menuVals[1] > MAX_LEVEL_ROWS) m->menuVals[1] = MAX_LEVEL_ROWS;
            if (m->menuVals[1] < 0) m->menuVals[1] = 0;
            if (m->menuVals[2] > MAX_LEVEL_COLS) m->menuVals[2] = MAX_LEVEL_COLS;
            if (m->menuVals[2] < 0) m->menuVals[2] = 0;
            if (m->menuVals[3] >= w->numTileTex) m->menuVals[3] = w->numTileTex - 1;
            if (m->menuVals[3] < 0) m->menuVals[3] = 0;
            break;
    }

    return NEW_LEVEL;
}

BuildState editAltMenu(Menu* editSubMenu) {
    drawMenu(editSubMenu);

    if (traverseMenu(editSubMenu, editSubMenu->menuTypes[editSubMenu->cursor]) == KEY_ENTER) {
        if (editSubMenu->cursor == 0) {
            // tool menu
            return EDITING;
        } else if (editSubMenu->cursor == 1) {
            // level switch menu
            return SWITCH_LEVEL;
        } else if (editSubMenu->cursor == 2) {
            // save
            return EDITING;
        } else if (editSubMenu->cursor == 3) {
            // save & exit
            return EXIT;
        } else if (editSubMenu->cursor == 4) {
            return EXIT;
        }
    }

    return EDITING;
}

BuildState editingLoop(Workspace* w, Menu* editTileContextMenu, Menu* editEntityContextMenu, Menu* editSubMenu) {
    static bool editSelectSwitch = false;
    static char** entityCollisionIDs;
    static int numEntityCollisions;

    renderWorkspace(w);

    if (editSelectSwitch) {
        switch (editSelect()) {
            case 0:
                w->editingEntity = true;
                editEntityContextMenu->cursor = 0;

                numEntityCollisions = getNumEntCollisions(&w->levels[w->activeEditLevel], w->cursorRow, w->cursorCol);
                if (numEntityCollisions > 0) {
                    entityCollisionIDs = (char**)malloc(sizeof(char*) * numEntityCollisions);
                    for (int i = 0; i < numEntityCollisions; i++) entityCollisionIDs[i] = (char*)malloc(sizeof(char) * MAX_ENT_ID_LEN);
                    if (getEntCollisionIDs(&w->levels[w->activeEditLevel], entityCollisionIDs, w->cursorRow, w->cursorCol)) {
                        w->entityEditCollision = true;
                    }
                } else {
                    w->entityEditCollision = false;
                    w->editingNewEntity = true;
                    int editEntityIdx = w->levels[w->activeEditLevel].nextFreeEnt;
                    w->levels[w->activeEditLevel].ents[editEntityIdx].attr[E_POS_X] = TILE_PIX_HEIGHT * w->cursorRow;
                    w->levels[w->activeEditLevel].ents[editEntityIdx].attr[E_POS_Y] = TILE_PIX_WIDTH * w->cursorCol;
                    for (int i = 1; i < E_NUM_ATTR; i++) {
                        editEntityContextMenu->menuVals[i] = w->levels[w->activeEditLevel].ents[editEntityIdx].attr[i - 1];
                    }

                    for (int i = 0; i < MAX_ENT_ID_LEN; i++) {
                        editEntityContextMenu->tBox[0].text[i] = w->levels[w->activeEditLevel].ents[editEntityIdx].entityID[i];
                    }
                }

                editSelectSwitch = false;
                break;
            case 1:
                w->editingTile = true;
                editTileContextMenu->cursor = 0;
                for (int i = 1; i < T_NUM_ATTR; i++) {
                    editTileContextMenu->menuVals[i] = w->levels[w->activeEditLevel].tiles[w->cursorRow][w->cursorCol].attr[i - 1];
                }
                for (int i = 0; i < MAX_TILE_ID_LEN; i++) {
                    editTileContextMenu->tBox[0].text[i] = w->levels[w->activeEditLevel].tiles[w->cursorRow][w->cursorCol].tileID[i];
                }

                editSelectSwitch = false;
                break;
            case 2:
                editSelectSwitch = false;
                break;
            default:
                break;
        }
    } else if (w->editingTile) {
        tileEdit(w, editTileContextMenu);
    } else if (w->editingEntity) {
        if (w->entityEditCollision) {
            int selVal = entityCollisionSelect(entityCollisionIDs, numEntityCollisions);

            if (selVal == numEntityCollisions) {
                // adding entity
                w->entityEditCollision = false;
                w->editingNewEntity = true;
            } else if (selVal == numEntityCollisions + 1) {
                // back
                w->entityEditCollision = false;
                w->editingEntity = false;
                editSelectSwitch = true;
            } else if (selVal >= 0 && selVal < numEntityCollisions) {
                w->editingEntityIdx = getEntIdx(&w->levels[w->activeEditLevel], entityCollisionIDs[selVal], w->cursorRow, w->cursorCol);
                w->entityEditCollision = false;
                w->editingNewEntity = false;

                for (int i = 1; i < E_NUM_ATTR; i++) {
                    editEntityContextMenu->menuVals[i] = w->levels[w->activeEditLevel].ents[w->editingEntityIdx].attr[i - 1];
                }

                for (int i = 0; i < MAX_ENT_ID_LEN; i++) {
                    editEntityContextMenu->tBox[0].text[i] = w->levels[w->activeEditLevel].ents[w->editingEntityIdx].entityID[i];
                }
            }
        } else {
            entityEdit(w, editEntityContextMenu, w->editingNewEntity);
        }
    } else if (IsKeyDown(KEY_LEFT_SHIFT)) {
        return editAltMenu(editSubMenu);
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
            editSelectSwitch = true;
        }
    }   

    return EDITING;
}

