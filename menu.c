#include "menu.h"

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