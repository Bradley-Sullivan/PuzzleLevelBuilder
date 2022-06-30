#include "edit_ui.h"

int editSelect() {
    static bool isInit = false;
    static Menu editSelectionMenu;
    static char editSelectionSel[3][MAX_MENU_LEN] = {"Edit/Add Entity", "Edit Tile", "BACK"};
    static int editSelectionTypes[3] = {SIMPLE_MENU, SIMPLE_MENU, SIMPLE_MENU};
    if (!isInit) {
        initMenu(&editSelectionMenu, 3, 15, editSelectionSel, editSelectionTypes, true);
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
        initMenu(&entityCollisionMenu, numCollisions + 2, CONTEXT_MENU_FS, entityCollisionMenuSel, entityCollisionMenuTypes, true); 
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