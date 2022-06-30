#ifndef EDIT_UI_H
#define EDIT_UI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <raylib.h>
#include "param.h"
#include "menu.h"
#include "draw.h"

int editSelect();
int entityCollisionSelect(char** entityIDs, int numCollisions);
void tileEdit(Workspace* w, Menu* editTileContextMenu);
void entityEdit(Workspace* w, Menu* editEntityContextMenu, bool newEntity);

#endif  //EDIT_UI_H