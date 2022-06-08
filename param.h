/**
 * @file drawParam.h
 * @author Bradley Sullivan (bradleysullivan@nevada.unr.edu)
 * @brief Main header file for useful drawing macros
 * @version 0.1
 * @date 2022-06-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#define WINDOW_WIDTH        GetScreenWidth()
#define WINDOW_HEIGHT       GetScreenHeight()
#define EDIT_WIDTH          (WINDOW_WIDTH * 0.8)
#define EDIT_HEIGHT         GetScreenHeight()

#define TILE_PIX_WIDTH     32
#define TILE_PIX_HEIGHT    32

#define TILE_DIRECTORY     "/home/brsul/Nextcloud/Programs/C/Games/PuzzleLevelBuilder/assets/Tile"
#define ENTITY_DIRECTORY   "/home/brsul/Nextcloud/Programs/C/Games/PuzzleLevelBuilder/assets/Entity"
#define OTHER_DIRECTORY    "/home/brsul/Nextcloud/Programs/C/Games/PuzzleLevelBuilder/assets/Other"
#define CURSOR_FILEPATH    "/home/brsul/Nextcloud/Programs/C/Games/PuzzleLevelBuilder/assets/cursor.png"

#define MAX_NUM_LEVELS     128
#define MAX_LEVEL_ROWS     64
#define MAX_LEVEL_COLS     64

#define MAX_NUM_TEX        128
#define MAX_FILENAME_LEN   64