#include "draw.h"

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
    if (w->visibleGridlines) drawGridOverlay(w);
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
        initMenu(&dispMenu, 15, 20, attr, attrMenTypes, false);
        dispMenuInit = true;
    }

    for (int i = 0; i < MAX_TEXT_ENTRY_LEN; i++) dispMenu.tBox[0].text[i] = t.tileID[i];

    for (int i = 0; i < 14; i++) {
        dispMenu.menuVals[i + 1] = t.attr[i];
    }

    drawMenu(&dispMenu);
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

void previewLevel(Workspace* w, int levelIndex) {
    
}

void drawGridOverlay(Workspace* w) {
    int yOffsetV, xOffsetV, yOffsetH, xOffsetH;

    yOffsetV = (EDIT_HEIGHT / 2) - ((w->cursorRow + 1) * TILE_PIX_HEIGHT) - (TILE_PIX_HEIGHT / 2);
    for (int i = 0; i < w->levels[w->activeEditLevel].numCols; i++) {
        xOffsetV = (EDIT_WIDTH / 2) - ((w->cursorCol - i) * TILE_PIX_WIDTH) + (TILE_PIX_WIDTH / 2);
        DrawLineEx((Vector2){xOffsetV, yOffsetV}, (Vector2){xOffsetV, yOffsetV + TILE_PIX_HEIGHT * (w->levels[w->activeEditLevel].numRows + 1)}, 2.5, WHITE);
    }

    xOffsetH = (EDIT_WIDTH / 2) - ((w->cursorCol + 1) * TILE_PIX_WIDTH) - (TILE_PIX_WIDTH / 2);
    for (int i = 0; i < w->levels[w->activeEditLevel].numRows; i++) {
        yOffsetH = (EDIT_HEIGHT / 2) - ((w->cursorRow - i) * TILE_PIX_HEIGHT) + (TILE_PIX_HEIGHT / 2);
        DrawLineEx((Vector2){xOffsetH, yOffsetH}, (Vector2){xOffsetH + TILE_PIX_WIDTH * (w->levels[w->activeEditLevel].numCols + 1), yOffsetH}, 2.5, WHITE);
    }
}