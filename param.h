/**
 * @file drawParam.h
 * @author Bradley Sullivan (bradleysullivan@nevada.unr.edu)
 * @brief Main header file for core data structs and macros
 * @version 0.1
 * @date 2022-06-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef PARAM_H
#define PARAM_H

#define WINDOW_WIDTH        GetScreenWidth()
#define WINDOW_HEIGHT       GetScreenHeight()
#define EDIT_WIDTH          (WINDOW_WIDTH * 0.8)
#define EDIT_HEIGHT         GetScreenHeight()

#define TILE_PIX_WIDTH     48
#define TILE_PIX_HEIGHT    48

#define TILE_DIRECTORY     "assets/Tile/"
#define ENTITY_DIRECTORY   "assets/Entity/"
#define OTHER_DIRECTORY    "assets/Other/"
#define CURSOR_FILEPATH    "assets/cursor.png"

#define LEVEL_SAVE_PATH     "data/level_save/"
#define WKSPC_SAVE_PATH     "data/wkspc_save/"

#define MAX_NUM_LEVELS     128
#define MAX_LEVEL_ROWS     64
#define MAX_LEVEL_COLS     64
#define MAX_NUM_ENTITIES   256

#define MAX_NUM_TEX        128
#define MAX_FILENAME_LEN   64
#define MAX_TILE_ID_LEN    16
#define MAX_ENT_ID_LEN     16

#define T_NUM_ATTR          14
#define E_NUM_ATTR          20
#define TRG_NUM_ATTR        12

#define T_TEXTURE_IDX       0
#define T_PLAYER_COLL       1
#define T_ENTITY_COLL       2
#define T_IS_TEXTURED       3
#define T_IS_LVL_END        4
#define T_IS_MOVEABLE       5
#define T_IS_P_SPAWN        6
#define T_IS_E_SPAWN        7
#define T_TELEPORTER        8
#define T_ONE_WAY_TELE      9
#define T_TELE_CH           10
#define T_E_SPAWN_CH        11
#define T_ROW               12
#define T_COL               13

#define E_TEXTURE_IDX       0
#define E_PLAYER_COLL       1
#define E_ENTITY_COLL       2
#define E_IS_TEXTURED       3
#define E_IS_MOBILE         4
#define E_IS_ANIMATED       5
#define E_IS_HOSTILE        6
#define E_IS_PASSIVE        7
#define E_IS_ACTIVE         8
#define E_IS_TRIGGERABLE    9
#define E_RENDER_LAYER      10
#define E_EFFECT_MAG        11
#define E_MOVE_SPEED        12
#define E_ANIM_PERIOD       13
#define E_TRIGGER_CH        14
#define E_SPAWN_CH          15
#define E_POS_X             16
#define E_POS_Y             17
#define E_DIR_X             18
#define E_DIR_Y             19

#define TRG_CHANNEL         0
#define TRG_INPUT_CH        1
#define TRG_IS_HEAD         2
#define TRG_IS_ACTIVE       3
#define TRG_IS_OR           4
#define TRG_IS_AND          5
#define TRG_IS_XOR          6
#define TRG_IS_NAND         7
#define TRG_IS_NOT          8
#define TRG_FORCE_ACTIVE    9
#define TRG_FORCE_INACTIVE  10
#define TRG_FORCE_INVERT    11

typedef struct Tile {
    char* tileID;

    int attr[T_NUM_ATTR];           // tile's attributes
} Tile;

typedef struct Entity {
    char* entityID;              // entity identifier used for quickly handling entity-related events?

    bool existsInWorkspace;

    int attr[2 * E_NUM_ATTR];

    Texture2D* animFrames;

    Rectangle collisionRec;     // entity "hitbox" used for judging collisions
} Entity;

typedef struct Level {
    char* levelID;

    bool initialized;

    int numRows;
    int numCols;
    int baseFloorTexIndex;
    int nextFreeEnt;

    Tile** tiles;
    Entity* ents;
} Level;

typedef struct Workspace {
    Level levels[MAX_NUM_LEVELS];

    bool editingTile;
    bool editingEntity;
    bool entityEditCollision;
    bool editingNewEntity;
    bool visibleGridlines;

    int activeEditLevel;
    int nextNewLevel;

    int editingEntityIdx;

    int numTileTex;
    int numEntityTex;
    int numOtherTex;

    int cursorRow;
    int cursorCol;

    Texture2D* tileTex;
    Texture2D* entityTex;
    Texture2D* otherTex;
    
    Texture2D cursorTex;

} Workspace;

typedef enum BuildState {
    MAIN_MENU,
    NEW_LEVEL,
    LOAD_LEVEL,
    SWITCH_LEVEL,
    EDITING,
    SAVE_EXPORT,
    HELP,
    EXIT
} BuildState;

typedef enum TexType {
    TILE_TEX,
    ENTITY_TEX,
    OTHER_TEX
} TexType;

#endif  // PARAM_H