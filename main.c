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
#include "menu.h"
#include "param.h"
#include "edit_ui.h"
#include "draw.h"

/*
    TODO: (for now)
        Implement workspace saving functionality
        Lay groundwork for save file parsing and loading [x]
        Figure out how to implement animation(s) for entities
            within editLoop and how it should get saved to disk
            expand texture loading to incorporate piecing together animations
                creating animation tex. sheet from individual images (numbered images)
            preview animation function
        Re-evaluate tile attr. and their purpose within the scope of a puzzle game
            Moveable?? Teleporter??
            Secondary textures?/texture mixing??
     >> Re-evaluate entity attr. in general
     >> Implementing edit tools!!
            shouldn't be too complex :>
        Implement adding levels to workspace!!  [x]
            set active edit to next new, call init level, yadda yadda, bada bing bada boom
        Implementing level editing switching    [x]
*/

void initWorkspace(Workspace* w);
bool initLevel(Level* l, char* id, int texIdx, int r, int c, int e);
bool loadTextures(Workspace* w);
int loadTexHelper(Texture2D dest[], char* dir);
// bool exportWorkspace(Workspace* w, char* filepath);
bool exportLevels(Workspace* w, char* filepath);
bool loadLevels(Workspace* w, char* filepath);
bool parseLevels(FILE* fp, Level* l);
bool parseEntities(FILE* fp, Level*l, int numEnts);
bool importLevels(Workspace* w, char* filepath);

int getEntIdx(Level* l, char* id, int row, int col);
int getNumEntCollisions(Level* l, int row, int col);
bool getEntCollisionIDs(Level* l, char** ids, int row, int col);
int listCurrentSaves();



BuildState mainMenuScreen(Menu* m);
BuildState levelInitConfig(Menu* m, Workspace* w);
BuildState editAltMenu(Workspace* w, Menu* editSubMenu);
BuildState editingLoop(Workspace* w, Menu* editTileContextMenu, Menu* editEntityContextMenu, Menu* editSubMenu);
BuildState saveExportMenu(Workspace* w, Menu* m);
BuildState loadLevelsMenu(Workspace* w, Menu* m);
BuildState switchLevel(Workspace* w, Menu* m);

int main(void) {
    Workspace editWorkspace;
    BuildState state = MAIN_MENU;
    Menu mainMenu, levelConfMenu, editTileContextMenu 
        ,editEntityContextMenu, editSubMenu, saveMenu
        ,loadLevelMenu, switchLevelMenu;

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "test");
    ToggleFullscreen();
    SetTargetFPS(60);

    initWorkspace(&editWorkspace);

    char mainMenuSel[4][MAX_MENU_LEN] = {"NEW", "LOAD", "HELP", "EXIT"};
    int mainMenuTypes[4] = {SIMPLE_MENU, SIMPLE_MENU, SIMPLE_MENU, SIMPLE_MENU};
    initMenu(&mainMenu, 4, MAIN_MENU_FS, mainMenuSel, mainMenuTypes, false);

    char levelConfMenuSel[6][MAX_MENU_LEN] = {"Level ID", "Rows", "Columns", "Init Floor Tex.", "CONFIRM", "BACK"};
    int levelConfMenuTypes[6] = {TEXT_ENTRY, PLUS_MINUS_MENU, PLUS_MINUS_MENU, PLUS_MINUS_MENU, SIMPLE_MENU, SIMPLE_MENU};
    initMenu(&levelConfMenu, 6, SUB_MENU_FS, levelConfMenuSel, levelConfMenuTypes, false);

    char editTileContextMenuSel[T_NUM_ATTR + 2][MAX_MENU_LEN] = {"TileID", "Player Coll.", "Entity Coll.", "Proj. Coll.", 
                                                "Moveable", "Player Spawn", "Entity Spawn", "Level End", 
                                                "Teleporter", "1-Way Tele.", "Tex. Index", "Row Pos.", 
                                                "Col Pos.", "Entity Sp. Channel", "Tele Channel", "CONFIRM"};
    int editTileContextMenuTypes[T_NUM_ATTR + 2] = {TEXT_ENTRY, CHECKLIST_MENU, CHECKLIST_MENU, CHECKLIST_MENU, 
                                    CHECKLIST_MENU, CHECKLIST_MENU, CHECKLIST_MENU, CHECKLIST_MENU, 
                                    CHECKLIST_MENU, CHECKLIST_MENU, PLUS_MINUS_MENU, DISPLAY_VAL, 
                                    DISPLAY_VAL, PLUS_MINUS_MENU, PLUS_MINUS_MENU, SIMPLE_MENU};
    initMenu(&editTileContextMenu, T_NUM_ATTR + 2, CONTEXT_MENU_FS, editTileContextMenuSel, editTileContextMenuTypes, true);

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
    initMenu(&editEntityContextMenu, E_NUM_ATTR + 2, CONTEXT_MENU_FS, editEntityContextMenuSel, editEntityContextMenuTypes, true);

    char editSubMenuSel[5][MAX_MENU_LEN] = {"TOOLS", "SWITCH LEVEL", "ADD LEVEL", "SAVE", "QUIT"};
    int editSubMenuTypes[5] = {SIMPLE_MENU, SIMPLE_MENU, SIMPLE_MENU, SIMPLE_MENU, SIMPLE_MENU};
    initMenu(&editSubMenu, 5, SUB_MENU_FS, editSubMenuSel, editSubMenuTypes, false);

    char saveMenuSel[3][MAX_MENU_LEN] = {"Save Level:", "CONFIRM", "BACK"};
    int saveMenuTypes[3] = {TEXT_ENTRY, SIMPLE_MENU, SIMPLE_MENU};
    initMenuRec(&saveMenu, 3, 25, saveMenuSel, saveMenuTypes, (Rectangle){25, WINDOW_HEIGHT - 185, 0, 0}, BLANK);

    char loadLevelMenuSel[3][MAX_MENU_LEN] = {"Filename:", "CONFIRM", "BACK"};
    int loadLevelMenuTypes[3] = {TEXT_ENTRY, SIMPLE_MENU, SIMPLE_MENU};
    initMenuRec(&loadLevelMenu, 3, 25, loadLevelMenuSel, loadLevelMenuTypes, (Rectangle){25, WINDOW_HEIGHT - 160, 0, 0}, BLANK);

    char switchMenuSel[4][MAX_MENU_LEN] = {"LEVEL ID:", "LEVEL INDEX:", "CONFIRM", "BACK"};
    int switchMenuTypes[4] = {DISPLAY_TEXT, PLUS_MINUS_MENU, SIMPLE_MENU, SIMPLE_MENU};
    initMenu(&switchLevelMenu, 4, SUB_MENU_FS, switchMenuSel, switchMenuTypes, false);    

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
                    state = loadLevelsMenu(&editWorkspace, &loadLevelMenu);
                    break;
                case SWITCH_LEVEL:
                    state = switchLevel(&editWorkspace, &switchLevelMenu);
                    break;
                case EDITING:
                    state = editingLoop(&editWorkspace, &editTileContextMenu, &editEntityContextMenu, &editSubMenu);
                    break;
                case SAVE_EXPORT:
                    state = saveExportMenu(&editWorkspace, &saveMenu);
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
    w->visibleGridlines = false;
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

bool initLevel(Level* l, char* id, int texIdx, int r, int c, int e) {
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
        
        l->nextFreeEnt = e;

        l->initialized = true;

        printf("Initialized Level: %s\n", l->levelID);
        printf("\tRows: %d\n", l->numRows);
        printf("\tColumns: %d\n", l->numCols);
        printf("\tBase Floor Tex Index: %d\n", l->baseFloorTexIndex);
    }
    
    return true;
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
    ChangeDirectory(rootDir);

    printf("%d tile textures loaded\n", w->numTileTex);
    printf("%d entity textures loaded\n", w->numEntityTex);
    printf("%d other textures loaded\n", w->numOtherTex);

    return true;
}

int loadTexHelper(Texture2D dest[], char* dir) {
    char** texNames;
    int numTex, curTex = 0;

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

int listCurrentSaves() {
    static char** filenames;
    static int numLevelSave;

    DrawRectangle(15, 15, EDIT_WIDTH - 30, WINDOW_HEIGHT - 60, BLACK);

    filenames = LoadDirectoryFiles(LEVEL_SAVE_PATH, &numLevelSave);

    for (int i = 0; i < numLevelSave; i++) {
        if (strcmp(".", filenames[i]) == 0 || strcmp("..", filenames[i]) == 0) {
            for (int k = i; k < numLevelSave - 1; k++) {
                strcpy(filenames[k], filenames[k + 1]);
            }
            numLevelSave -= 1;
        }
    }

    DrawText("SAVED LEVEL FILES", 25, 25, 25, RAYWHITE);
    if (numLevelSave > 0) {
        int xOffset = 50;
        for (int i = 0; i < numLevelSave; i++) {
            int yOffset = 25 * (i + 2);
            DrawText(filenames[i], xOffset, yOffset, 25, RAYWHITE);
        }
    } else {
        DrawText("NO SAVED LEVELS", 50, 50, 25, RAYWHITE);
    }

    UnloadDirectoryFiles();
    return 0;
}

bool exportLevels(Workspace* w, char* filepath) {
    FILE* fp;
    fp = fopen(filepath, "w");
    
    if (fp == NULL) {
        printf("ERROR: Could not open path to file in order to export levels\nFILEPATH: %s\n", filepath);
        return false;
    } else {
        fprintf(fp, "[%d]\n", w->nextNewLevel);
        for (int i = 0; i < w->nextNewLevel; i++) {   
            fprintf(fp, ":%s,%d,%d,%d:\n;", w->levels[i].levelID, w->levels[i].numRows, w->levels[i].numCols, w->levels[i].nextFreeEnt);
            for (int k = 0; k < w->levels[i].numRows; k++) {
                for (int j = 0; j < w->levels[i].numCols; j++) {
                    char* printStr = (char*)malloc(sizeof(char) * (2 * T_NUM_ATTR * w->levels[i].numCols + MAX_TILE_ID_LEN));
                    char* tileAttr = (char*)malloc(sizeof(char) * 2 * T_NUM_ATTR);
                    if (w->levels[i].tiles[j][k].tileID[0] == '\0') {
                        strcpy(printStr, "unnamed,");
                    } else {
                        sprintf(printStr, "%s,", w->levels[i].tiles[k][j].tileID);
                    }
                    for (int l = 0; l < T_NUM_ATTR; l++) {
                        sprintf(tileAttr, "%d", w->levels[i].tiles[k][j].attr[l]);
                        if (l < T_NUM_ATTR - 1) {
                            strcat(tileAttr, ",");
                        } else {
                            strcat(tileAttr, ";");
                        }
                        strcat(printStr, tileAttr);
                    }
                    fprintf(fp, "%s", printStr);
                }
            }
            fprintf(fp, "\n");
            for (int k = 0; k < w->levels[i].nextFreeEnt; k++) {
                char* printStr = (char*)malloc(sizeof(char) * (3 * E_NUM_ATTR + MAX_ENT_ID_LEN));
                char* entityAttr = (char*)malloc(sizeof(char) * (2 * E_NUM_ATTR));
                if (w->levels[i].ents[k].entityID[0] == '\0') {
                    strcpy(printStr, ";unnamed,");
                } else {
                    sprintf(printStr, ";%s,", w->levels[i].ents[k].entityID);
                }                
                for (int j = 0; j < E_NUM_ATTR; j++) {
                    sprintf(entityAttr, "%d", w->levels[i].ents[k].attr[j]);
                    if (j < E_NUM_ATTR - 1) {
                        strcat(entityAttr, ",");
                    } else {
                        strcat(entityAttr, ";");
                    }
                    strcat(printStr, entityAttr);
                }
                fprintf(fp, "%s", printStr);
            }
            fprintf(fp, "\n");
        }

        fclose(fp);
    }
    
    return true;
}

bool loadLevels(Workspace* w, char* filepath) {
    FILE* fp;
    fp = fopen(filepath, "r");

    if (fp == NULL) {
        printf("ERROR: Could not open path to file in order to export levels\nFILEPATH: %s\n", filepath);
        return false;
    } else {
        if (fscanf(fp, "[%d]", &w->nextNewLevel) == 1) {
            for (int i = 0; i < w->nextNewLevel; i++) {
                if (!parseLevels(fp, &w->levels[i])) {
                    printf("ERROR: Unable to parse levels. Aborting entity loading.\n");
                    return false;
                }                 
            }
        } else {
            printf("ERROR: File is in the incorrect format. Could not parse file header.\n");
            return false;
        }        
    }

    return true;
}

bool parseLevels(FILE* fp, Level* l) {
    int r, c, e;
    char* idBuf = (char*)malloc(sizeof(char) * MAX_LEVEL_ID_LEN);
    if (fscanf(fp, "\n:%[^,],%d,%d,%d:\n;", idBuf, &r, &c, &e) == 4) {      // parse level header
        printf(":%s,%d,%d,%d:\n", idBuf, r, c, e);
        initLevel(l, idBuf, 0, r, c, e);
        for (int j = 0; j < r; j++) {
            for (int k = 0; k < c; k++) {
                if (fscanf(fp, "%[^,],", l->tiles[j][k].tileID) == 1) {     // parse tileID
                    printf(";%s,", l->tiles[j][k].tileID);
                    for (int m = 0; m < T_NUM_ATTR; m++) {
                        if (m < T_NUM_ATTR - 1) {
                            if (fscanf(fp, "%d%*[,;]", &l->tiles[j][k].attr[m]) == 1) {     // parse tile attr.
                                printf(" %d ", l->tiles[j][k].attr[m]);
                            } else {
                                printf("ERROR: File is in the incorrect format. Could not parse tile attr.\n");
                                return false;
                            }
                        } else {
                            if (fscanf(fp, "%d;", &l->tiles[j][k].attr[m]) == 1) {
                                printf(" %d;", l->tiles[j][k].attr[m]);
                            } else {
                                printf("ERROR: File is in the incorrect format. Could not parse tile attr.\n");
                                return false;
                            }
                        }                            
                    }
                    printf("\n");
                } else {
                    printf("ERROR: File is in the incorrect format. Could not parse tileID.\n");
                    return false;
                }
                
            }
        }
        if (!parseEntities(fp, l, e)) {
            printf("ERROR: Unable to parse entities. Aborting level load process\n");
            return false;       
        }
    } else {
        printf("ERROR: File is in the incorrect format. Could not parse level header.\n");
        return false;
    }

    return true;
}

bool parseEntities(FILE* fp, Level* l, int numEnts) {
    printf("attempting to parse %d entities\n", numEnts);
    for (int i = 0; i < numEnts; i++) {
        l->ents[i].existsInWorkspace = true;
        if (fscanf(fp, "%[^,],", l->ents[i].entityID) == 1) {
            printf(";%s,", l->ents[i].entityID);
            for (int k = 0; k < E_NUM_ATTR; k++) {
                if (k < E_NUM_ATTR - 1) {
                    if (fscanf(fp, "%d%*[,;]", &l->ents[i].attr[k]) == 1) {
                        printf(" %d ", l->ents[i].attr[k]);
                    } else {
                        printf("ERROR: File is in the incorrect format. Could not parse entity attr.\n");
                        return false;
                    }
                } else {
                    if (fscanf(fp, "%d;", &l->ents[i].attr[k]) == 1) {
                        printf(" %d ;", l->ents[i].attr[k]);
                    } else {
                        printf("ERROR: File is in the incorrect format. Could not parse entity attr.\n");
                        return false;
                    }
                }
            }
            printf("\n");
        } else {
            printf("ERROR: File is in the incorrect format. Could not parse entityID.\n");
            return false;
        }
    }


    return true;
}

bool importLevels(Workspace* w, char* filepath) {

    return true;
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
                if (initLevel(&w->levels[w->nextNewLevel], m->tBox[0].text, m->menuVals[3], m->menuVals[1], m->menuVals[2], 0)) {
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
            } else if (m->cursor == 5) {
                return MAIN_MENU;
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

BuildState editAltMenu(Workspace* w, Menu* editSubMenu) {
    drawMenu(editSubMenu);

    if (traverseMenu(editSubMenu, editSubMenu->menuTypes[editSubMenu->cursor]) == KEY_ENTER) {
        if (editSubMenu->cursor == 0) {
            // tool menu
            return EDITING;
        } else if (editSubMenu->cursor == 1) {
            // level switch menu
            return SWITCH_LEVEL;
        } else if (editSubMenu->cursor == 2) {
            // add level
            w->activeEditLevel = w->nextNewLevel;
            return NEW_LEVEL;
        } else if (editSubMenu->cursor == 3) {
            // save
            return SAVE_EXPORT;
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
                    w->levels[w->activeEditLevel].ents[editEntityIdx].attr[E_POS_X] = TILE_PIX_HEIGHT * w->cursorCol;
                    w->levels[w->activeEditLevel].ents[editEntityIdx].attr[E_POS_Y] = TILE_PIX_WIDTH * w->cursorRow;
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
        return editAltMenu(w, editSubMenu);
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

BuildState saveExportMenu(Workspace* w, Menu* m) {
    static char* levelFilepath;
    listCurrentSaves();
    drawMenuRec(m);

    switch(traverseMenu(m, m->menuTypes[m->cursor])) {
        case KEY_ENTER:
            if (m->cursor == 1) {
                // confirm
                levelFilepath = (char*)malloc(sizeof(char) * 64);
                strcpy(levelFilepath, LEVEL_SAVE_PATH);
                strcat(levelFilepath, m->tBox[0].text);

                if (exportLevels(w, levelFilepath)) {
                    free(levelFilepath);
                    return EDITING;
                } else {
                    // error menu?
                    return SAVE_EXPORT;
                }                
            } else if (m->cursor == 2) {
                // back
                return EDITING;
            }
            break;
        default:
            break;
    }

    return SAVE_EXPORT;
}

BuildState loadLevelsMenu(Workspace* w, Menu* m) {
    static char* filepath;
    listCurrentSaves();
    drawMenuRec(m);

    switch (traverseMenu(m, m->menuTypes[m->cursor])) {
        case KEY_ENTER:
            if (m->cursor == 1) {
                // confirm
                filepath = (char*)malloc(sizeof(char) * 64);
                strcpy(filepath, LEVEL_SAVE_PATH);
                strcat(filepath, m->tBox[0].text);
                initWorkspace(w);
                if (loadLevels(w, filepath)) {
                    return EDITING;
                } else {
                    return MAIN_MENU;
                }
            } else if (m->cursor == 2) {
                // back
                return MAIN_MENU;
            }
            break;
        default:
            break;
    }

    return LOAD_LEVEL;
}

BuildState switchLevel(Workspace* w, Menu* m) {
    strncpy(m->tBox[0].text, w->levels[m->menuVals[1]].levelID, MAX_LEVEL_ID_LEN);
    
    // previewLevel(w, m->menuVals[1]);
    drawMenu(m);

    switch (traverseMenu(m, m->menuTypes[m->cursor])) {
        case KEY_ENTER:
            if (m->cursor == 2) {
                // confirm
                w->activeEditLevel = m->menuVals[1];
                return EDITING;
            } else if (m->cursor == 3) {
                // back
                return EDITING;
            }
            break;
        default:
            break;
    }

    if (m->menuVals[1] < 0) m->menuVals[1] = 0;
    if (m->menuVals[1] > w->nextNewLevel - 1) m->menuVals[1] = w->nextNewLevel - 1;

    return SWITCH_LEVEL;
}
