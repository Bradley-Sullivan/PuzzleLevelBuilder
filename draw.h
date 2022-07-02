#ifndef DRAW_H
#define DRAW_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <raylib.h>
#include "param.h"
#include "menu.h"

void renderWorkspace(Workspace* w);
void drawTileAttr(Tile t, double x, double y);
void previewTextures(Workspace* w, int tex, TexType type);
void previewLevel(Workspace* w, int levelIndex);

#endif  //DRAW_H