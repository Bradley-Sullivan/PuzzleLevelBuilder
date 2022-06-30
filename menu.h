/**
 * @file menu.h
 * @author Bradley Sullivan (bradleysullivan@nevada.unr.edu)
 * @brief Header file for primitive menu module.
 * @version 0.1
 * @date 2022-06-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef MENU_H
#define MENU_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <raylib.h>
#include "param.h"

#define MAIN_MENU_FS        30
#define SUB_MENU_FS         20
#define CONTEXT_MENU_FS     15

#define MAX_MENU_LEN        64
#define MAX_TEXT_ENTRY_LEN  64
#define MAX_LEVEL_ID_LEN    16

#define SIMPLE_MENU         0
#define PLUS_MINUS_MENU     1
#define CHECKLIST_MENU      2
#define TEXT_ENTRY          3
#define DISPLAY_VAL         4

typedef struct TextBox {
    bool editing;

    double posX;
    double posY;

    int maxLen;
    int maxDispWidth;
    int cursor;
    int lastCharIndex;
    int fontSize;
    int width;
    int height;

    char* text;

    Rectangle background;
    Rectangle editBox;
} TextBox;

typedef struct Menu {
    bool isContextMenu;

    int cursor;
    int numSel;
    int selFS;

    int* menuTypes;
    int* menuVals;

    char** sel;

    TextBox* tBox;

    Rectangle rec;
    Color background;
} Menu;

void initMenu(Menu* m, int numSel, int selFontSize, char sel[][MAX_MENU_LEN], int types[], bool isContextMenu);
void initMenuRec(Menu* m, int numSel, int selFontSize, char sel[][MAX_MENU_LEN], int types[], Rectangle rec, Color c);
void initTextBox(TextBox* t, int editDisplayWidth, int fontSize, int initCursor, double x, double y);

// add reset/clear functions for menu and tboxes
void resetMenu(Menu* m);
void clearTextBox(TextBox* t);

void drawMenu(Menu* m);
void drawMenuRec(Menu* m);
void drawTextBox(TextBox* t, bool active);

int traverseMenu(Menu* m, int menuType);
bool editTextBox(TextBox* t);

int getLongestSel(Menu* m);
int getLongSelSize(Menu* m);

#endif //MENU_H