#include "menu.h"

void initMenu(Menu *m, int numSel, int initCursor, int selFontSize, char sel[][MAX_MENU_LEN], int types[], bool isContextMenu) {
    int numTextEntry = 0;

    m->isContextMenu = isContextMenu;
    m->cursor = initCursor;
    m->numSel = numSel;
    m->selFS = selFontSize;

    m->menuTypes = (int*)malloc(sizeof(int) * numSel);
    m->menuVals = (int*)malloc(sizeof(int) * numSel);
    m->sel = (char**)malloc(sizeof(char*) * numSel);
    for (int i = 0; i < numSel; i++) {
        if (types[i] == TEXT_ENTRY) {
            numTextEntry++;
        }
        m->sel[i] = (char*)malloc(sizeof(char) * MAX_MENU_LEN);
        m->sel[i] = sel[i];
        m->menuTypes[i] = types[i];
        m->menuVals[i] = 0;
    }

    if (numTextEntry > 0) {
        m->tBox = (TextBox*)malloc(sizeof(TextBox) * numSel);
        for (int i = 0; i < numSel; i++) {
            if (m->menuTypes[i] == TEXT_ENTRY) {
                if (isContextMenu) {
                    int xOff = (EDIT_WIDTH / 2) + getLongSelSize(m);
                    initTextBox(&m->tBox[i], 10, m->selFS, 0, xOff, EDIT_HEIGHT / 2 - 5);
                } else {
                    int xOff = EDIT_WIDTH + getLongSelSize(m);
                    initTextBox(&m->tBox[i], 10, m->selFS, 0, xOff, ((i + 1) * m->selFS) - 5);
                }
            }
        }
    }
}

void initTextBox(TextBox* t, int editDisplayWidth, int fontSize, int initCursor, double x, double y) {
    t->editing = false;
    t->maxLen = MAX_TEXT_ENTRY_LEN;
    t->maxDispWidth = editDisplayWidth;
    t->fontSize = fontSize;
    t->cursor = initCursor;
    t->width = MeasureText("X", fontSize) * (editDisplayWidth + 1);
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

    t->text = (char*)malloc(sizeof(char) * MAX_TEXT_ENTRY_LEN);
    for (int i = 0; i < MAX_TEXT_ENTRY_LEN; i++) {
        t->text[i] = '\0';
    }
}

void resetMenu(Menu* m) {
    for (int i = 0; i < m->numSel; i++) {
        m->menuVals[i] = 0;
        if (m->menuTypes[i] == TEXT_ENTRY) {
            clearTextBox(&m->tBox[i]);
        }
    }
}

void clearTextBox(TextBox* t) {
    t->editing = false;
    t->cursor = 0;

    for (int i = 0; i < MAX_TEXT_ENTRY_LEN; i++) {
        t->text[i] = '\0';
    }
}

void drawMenu(Menu* m) {
    int begOffset, yOffset;
    int identSelSize = getLongSelSize(m) + 50;
    char buf[8];

    if (m->isContextMenu) {
        begOffset = (EDIT_WIDTH / 2) + 50;
        DrawRectangle((EDIT_WIDTH / 2) + 40, (EDIT_HEIGHT / 2) - 10, identSelSize + 50, m->numSel * m->selFS + 20, (Color){25, 25, 25, 250});
        DrawText(">", begOffset - MeasureText(">", m->selFS), (m->cursor * m->selFS) + (WINDOW_HEIGHT / 2), m->selFS, RAYWHITE);
    } else {
        begOffset = EDIT_WIDTH + 2 * m->selFS;
        DrawText(">", EDIT_WIDTH + m->selFS, m->selFS + (m->cursor * m->selFS), m->selFS, RAYWHITE);
    }


    for (int i = 0; i < m->numSel; i++) {
        int curSelSize = MeasureText(m->sel[i], m->selFS);
        int identWidth = curSelSize + (identSelSize - curSelSize);

        if (m->isContextMenu) {
            yOffset = (WINDOW_HEIGHT / 2) + (i * m->selFS);
        } else {
            yOffset = m->selFS + (i * m->selFS);
        }

        switch (m->menuTypes[i]) {
            case SIMPLE_MENU:
                DrawText(m->sel[i], begOffset, yOffset, m->selFS, RAYWHITE);
                break;
            case PLUS_MINUS_MENU:
                DrawText(m->sel[i], begOffset, yOffset, m->selFS, RAYWHITE);
                sprintf(buf, "- %d +", m->menuVals[i]);
                DrawText(buf, begOffset + identWidth, yOffset, m->selFS, RAYWHITE);
                break;
            case CHECKLIST_MENU:
                DrawText(m->sel[i], begOffset, yOffset, m->selFS, RAYWHITE);
                if (m->menuVals[i]) {
                    DrawText("[x]", begOffset + identWidth, yOffset, m->selFS, RAYWHITE);
                } else {
                    DrawText("[ ]", begOffset + identWidth, yOffset, m->selFS, RAYWHITE);
                }
                break;
            case TEXT_ENTRY:
                DrawText(m->sel[i], begOffset, yOffset, m->selFS, RAYWHITE);
                drawTextBox(&m->tBox[i], m->tBox[i].editing);
                break;
            case DISPLAY_VAL:
                DrawText(m->sel[i], begOffset, yOffset, m->selFS, RAYWHITE);
                sprintf(buf, "%d", m->menuVals[i]);
                DrawText(buf, begOffset + identWidth, yOffset, m->selFS, RAYWHITE);
                break;
            default:
                break;
        }

    }
}

void drawTextBox(TextBox* t, bool active) {
    if (active) {
        DrawRectangleRec(t->background, RAYWHITE);
        DrawRectangleRec(t->editBox, BLACK);
        if (t->cursor > t->maxDispWidth) {
            DrawText(TextSubtext(t->text, t->cursor - t->maxDispWidth, t->maxLen), t->posX + 1, t->posY, t->fontSize, RAYWHITE);
            DrawRectangle(t->posX + MeasureText(TextSubtext(t->text, t->cursor - t->maxDispWidth, t->cursor), t->fontSize), t->posY + t->height - 2, t->fontSize - 5, 2, RAYWHITE);
        } else {
            DrawText(t->text, t->posX + 1, t->posY, t->fontSize, RAYWHITE);
            DrawRectangle(t->posX + MeasureText(TextSubtext(t->text, 0, t->cursor), t->fontSize), t->posY + t->height - 2, t->fontSize - 5, 2, RAYWHITE);
        } 
    } else {
        DrawRectangleRec(t->background, (Color){245, 245, 245, 150});
        DrawRectangleRec(t->editBox, (Color){10, 10, 10, 150});

        if (t->cursor > t->maxDispWidth) {
            DrawText(TextSubtext(t->text, t->cursor - t->maxDispWidth, t->maxLen), t->posX + 1, t->posY, t->fontSize, (Color){245, 245, 245, 150});
        } else {
            DrawText(t->text, t->posX + 1, t->posY, t->fontSize, (Color){245, 245, 245, 150});
        }
    }
}

int traverseMenu(Menu* m, int menuType) {
    static float tBuf;
    tBuf += GetFrameTime();

    int retVal = -1;


    switch (menuType) {
        case SIMPLE_MENU:
            if (IsKeyPressed(KEY_UP)) {
                m->cursor = (m->cursor == 0) ? m->numSel - 1 : m->cursor - 1;
                retVal = -1;
            } else if (IsKeyPressed(KEY_DOWN)) {
                m->cursor = (m->cursor + 1) % m->numSel;
                retVal = -1;
            } else if (IsKeyPressed(KEY_ENTER)) {
                retVal = KEY_ENTER;
            } else if (IsKeyPressed(KEY_SPACE)) {
                retVal = KEY_SPACE;
            }
            break;
        case PLUS_MINUS_MENU:
            if (IsKeyPressed(KEY_UP)) {
                m->cursor = (m->cursor == 0) ? m->numSel - 1 : m->cursor - 1;
                retVal = -1;
            } else if (IsKeyPressed(KEY_DOWN)) {
                m->cursor = (m->cursor + 1) % m->numSel;
                retVal = -1;
            } else if (IsKeyDown(KEY_LEFT) && tBuf > 0.1f) {
                m->menuVals[m->cursor] -= 1;
                retVal = KEY_LEFT;
                tBuf = 0.0f;
            } else if (IsKeyDown(KEY_RIGHT) && tBuf > 0.1f) {
                m->menuVals[m->cursor] += 1;
                retVal = KEY_RIGHT;
                tBuf = 0.0f;
            }
            break;
        case CHECKLIST_MENU:
            if (IsKeyPressed(KEY_UP)) {
                m->cursor = (m->cursor == 0) ? m->numSel - 1 : m->cursor - 1;
                retVal = -1;
            } else if (IsKeyPressed(KEY_DOWN)) {
                m->cursor = (m->cursor + 1) % m->numSel;
                retVal = -1;
            } else if (IsKeyPressed(KEY_SPACE)) {
                m->menuVals[m->cursor] = (m->menuVals[m->cursor] == 0) ? 1 : 0;
                retVal = KEY_SPACE;
            }
            break;
        case TEXT_ENTRY:
            if (!m->tBox[m->cursor].editing) {
                if (IsKeyPressed(KEY_UP)) {
                    m->cursor = (m->cursor == 0) ? m->numSel - 1 : m->cursor - 1;
                    retVal = -1;
                } else if (IsKeyPressed(KEY_DOWN)) {
                    m->cursor = (m->cursor + 1) % m->numSel;
                    retVal = -1;
                } else if (IsKeyPressed(KEY_ENTER)) {
                    m->tBox[m->cursor].editing = true;
                }
            } else {
                editTextBox(&m->tBox[m->cursor]);
            }            
            break;            
        default:
            if (IsKeyPressed(KEY_UP)) {
                m->cursor = (m->cursor == 0) ? m->numSel - 1 : m->cursor - 1;
                retVal = -1;
            } else if (IsKeyPressed(KEY_DOWN)) {
                m->cursor = (m->cursor + 1) % m->numSel;
                retVal = -1;
            }
            break;
    }
    

    return retVal;
}

bool editTextBox(TextBox* t) {
    int key = GetCharPressed();

    if (t->cursor < t->maxLen - 1 && key != 0) {
        t->text[t->cursor] = (char)key;
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
            if (t->cursor < t->maxLen - 1 && t->text[0] != '\0') {
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

int getLongestSel(Menu* m) {
    int maxLen = 0, maxIdx = 0;

    for (int i = 0; i < m->numSel; i++) {
        if (MeasureText(m->sel[i], m->selFS) > maxLen || maxLen == 0) {
            maxLen = MeasureText(m->sel[i], m->selFS);
            maxIdx = i;
        }
    }

    return maxIdx;
}

int getLongSelSize(Menu* m) {
    int maxSelIdx = getLongestSel(m);

    return MeasureText(m->sel[maxSelIdx], m->selFS);
}