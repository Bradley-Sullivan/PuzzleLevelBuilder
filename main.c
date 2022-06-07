#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "include/raylib.h"

#define WINDOW_WIDTH        GetScreenWidth()
#define WINDOW_HEIGHT       GetScreenHeight()
#define EDIT_WIDTH          (WINDOW_WIDTH * 0.8)
#define EDIT_HEIGHT         GetScreenHeight()

#define MAX_NUM_LEVELS      128
#define MAX_LEVEL_ROWS      64
#define MAX_LEVEL_COLS      64

#define MAX_MENU_LEN        64
#define MAX_LEVEL_ID_LEN    16

#define SIMPLE_MENU         0
#define PLUS_MINUS_MENU     1
#define CHECKLIST_MENU      2
#define TEXT_ENTRY          3


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

typedef struct {
    int cursor;
    int numSel;
    int selFS;

    int* menuTypes;
    int* menuVals;

    char** sel;
} Menu;

typedef struct {
    bool editing;

    double posX;
    double posY;

    int maxLen;
    int cursor;
    int lastCharIndex;
    int fontSize;
    int width;
    int height;

    char* text;

    Rectangle background;
    Rectangle editBox;
} TextBox;

typedef enum {
    MAIN_MENU,
    NEW_LEVEL,
    LOAD_LEVEL,
    EDITING,
    SAVE_EXPORT,
    HELP,
    EXIT
} BuildState;

void initMenu(Menu* m, int numSel, int initCursor, int selFontSize, char sel[][MAX_MENU_LEN], int types[]);
bool initLevel(Level* l, char* id, int texIdx, int r, int c);
void initTextBox(TextBox* t, int len, int fontSize, int initCursor, double x, double y);

void drawMenu(Menu* m);
void drawTextBox(TextBox* t, bool active);
int traverseMenu(Menu* m, int menuType);
bool editTextBox(TextBox* t);

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

void initMenu(Menu *m, int numSel, int initCursor, int selFontSize, char sel[][MAX_MENU_LEN], int types[]) {
    m->cursor = initCursor;
    m->numSel = numSel;
    m->selFS = selFontSize;

    m->menuTypes = (int*)malloc(sizeof(int) * numSel);
    m->menuVals = (int*)malloc(sizeof(int) * numSel);
    m->sel = (char**)malloc(sizeof(char*) * numSel);
    for (int i = 0; i < numSel; i++) {
        m->sel[i] = (char*)malloc(sizeof(char) * MAX_MENU_LEN);
        m->sel[i] = sel[i];
        m->menuTypes[i] = types[i];
        m->menuVals[i] = 0;
    }
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

void initTextBox(TextBox* t, int len, int fontSize, int initCursor, double x, double y) {
    t->editing = false;
    t->maxLen = len;
    t->fontSize = fontSize;
    t->cursor = initCursor;
    t->width = fontSize * len - 5;
    t->height = fontSize;
    t->posX = x;
    t->posY = y;

    t->background.height = t->height + 5;
    t->background.width = t->width + 5;
    t->background.x = x - 2.5;
    t->background.y = y - 2.5;

    t->editBox.height = t->height;
    t->editBox.width = t->width ;
    t->editBox.x = x;
    t->editBox.y = y;

    t->text = (char*)malloc(sizeof(char) * len);
    for (int i = 0; i < len; i++) {
        t->text[i] = '\0';
    }
}

void drawMenu(Menu* m) {
    int begOffset = EDIT_WIDTH + 2 * m->selFS;
    for (int i = 0; i < m->numSel; i++) {
        int yOffset = m->selFS + (i * m->selFS);
        switch (m->menuTypes[i]) {
            case SIMPLE_MENU:
                DrawText(m->sel[i], begOffset, yOffset, m->selFS, RAYWHITE);
                break;
            case PLUS_MINUS_MENU:
                char buf[8];
                sprintf(buf, "- %d +", m->menuVals[i]);
                DrawText(m->sel[i], begOffset, yOffset, m->selFS, RAYWHITE);
                DrawText(buf, begOffset + 10 + MeasureText(m->sel[i], m->selFS), yOffset, m->selFS, RAYWHITE);
                break;
            case CHECKLIST_MENU:
                DrawText(m->sel[i], begOffset, yOffset, m->selFS, RAYWHITE);
                if (m->menuVals[i]) {
                    DrawText("[x]", begOffset + 5 + MeasureText(m->sel[i], m->selFS), yOffset, m->selFS, RAYWHITE);
                } else {
                    DrawText("[ ]", begOffset + 5 + MeasureText(m->sel[i], m->selFS), yOffset, m->selFS, RAYWHITE);
                }
                break;
            default:
                DrawText(m->sel[i], begOffset, yOffset, m->selFS, RAYWHITE);
                break;
        }
    }

    DrawText(">", EDIT_WIDTH + m->selFS, m->selFS + (m->cursor * m->selFS), m->selFS, RAYWHITE);
}

void drawTextBox(TextBox* t, bool active) {
    if (active) {
        DrawRectangleRec(t->background, RAYWHITE);
        DrawRectangleRec(t->editBox, BLACK);
        DrawText(t->text, t->posX + 1, t->posY, t->fontSize, RAYWHITE);
        DrawRectangle(t->posX + MeasureText(TextSubtext(t->text, 0, t->cursor), t->fontSize), t->posY + t->height - 2, t->fontSize - 5, 2, RAYWHITE); 
    } else {
        DrawRectangleRec(t->background, (Color){245, 245, 245, 150});
        DrawRectangleRec(t->editBox, (Color){10, 10, 10, 150});
        DrawText(t->text, t->posX + 1, t->posY, t->fontSize, (Color){245, 245, 245, 150});
    }
    
}

int traverseMenu(Menu* m, int menuType) {
    int retVal = -1;

    switch (menuType) {
        case SIMPLE_MENU:
            switch (GetKeyPressed()) {
                case KEY_UP:
                    m->cursor = (m->cursor == 0) ? m->numSel - 1 : m->cursor - 1;
                    retVal = -1;
                    break;
                case KEY_DOWN:
                    m->cursor = (m->cursor + 1) % m->numSel;
                    retVal = -1;
                    break;
                case KEY_ENTER:
                    retVal = KEY_ENTER;
                    break;
                case KEY_SPACE:
                    retVal = KEY_SPACE;
                    break;
                default:
                    break;
            }
            break;
        case PLUS_MINUS_MENU:
            switch (GetKeyPressed()) {
                case KEY_UP:
                    m->cursor = (m->cursor == 0) ? m->numSel - 1 : m->cursor - 1;
                    retVal = -1;
                    break;
                case KEY_DOWN:
                    m->cursor = (m->cursor + 1) % m->numSel;
                    retVal = -1;
                    break;
                case KEY_LEFT:
                    m->menuVals[m->cursor] -= 1;
                    retVal = KEY_LEFT;
                    break;
                case KEY_RIGHT:
                    m->menuVals[m->cursor] += 1;
                    retVal = KEY_RIGHT;
                    break;
                default:
                    retVal = -1;
                    break;
            }
            break;
        case CHECKLIST_MENU:
            switch(GetKeyPressed()) {
                case KEY_UP:
                    m->cursor = (m->cursor == 0) ? m->numSel - 1 : m->cursor - 1;
                    retVal = -1;
                    break;
                case KEY_DOWN:
                    m->cursor = (m->cursor + 1) % m->numSel;
                    retVal = -1;
                    break;
                case KEY_SPACE:
                    m->menuVals[m->cursor] = (m->menuVals[m->cursor] == 0) ? 1 : 0;
                    retVal = KEY_SPACE;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return retVal;
}

bool editTextBox(TextBox* t) {
    int key = GetCharPressed();

    if (t->cursor < t->maxLen - 1 && key != 0) {
        t->text[t->cursor] = key;
        t->cursor += 1;
    }

    key = GetKeyPressed();

    switch (key) {
        case KEY_BACKSPACE:
            if (t->cursor > 0) {
                for (int i = t->cursor; i < t->maxLen; i++) t->text[i - 1] = t->text[i];
                t->cursor -= 1;
            }
            break;
        case KEY_DELETE:
            if (t->cursor < t->maxLen - 1) {
                for (int i = t->cursor; i < t->maxLen; i++) t->text[i] = t->text[i + 1];
            }
            break;
        case KEY_LEFT:
            if (t->cursor > 0) {
                t->cursor -= 1;
            }
            break;
        case KEY_RIGHT:
            if (t->cursor < t->maxLen - 1) {
                t->cursor += 1;
            }
            break;
        case KEY_HOME:
            t->cursor = 0;
            break;
        case KEY_END:
            t->cursor = t->maxLen - 1;
            break;
        case KEY_ENTER:
            t->editing = false;
            return false;
        default:
            break;
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

