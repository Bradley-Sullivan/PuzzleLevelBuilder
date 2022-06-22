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

#define TILE_DIRECTORY     "assets/Tile"
#define ENTITY_DIRECTORY   "assets/Entity"
#define OTHER_DIRECTORY    "assets/Other"
#define CURSOR_FILEPATH    "assets/cursor.png"

#define MAX_NUM_LEVELS     128
#define MAX_LEVEL_ROWS     64
#define MAX_LEVEL_COLS     64
#define MAX_NUM_ENTITIES   256

#define MAX_NUM_TEX        128
#define MAX_FILENAME_LEN   64
#define MAX_TILE_ID_LEN    16
#define MAX_ENT_ID_LEN     16

#define T_PLAYER_COLL       0   // has player collision
#define T_ENTITY_COLL       1   // has entity collision
#define T_PROJ_COLL         2   // has proj. collision
#define T_MOVEABLE          3   // player-mutable position
#define T_P_SPAWN           4   // player initial spawn point
#define T_E_SPAWN           5   // entity spawn point
#define T_LVL_END           6   // level end goal
#define T_TELEPORTER        7   // teleporter tile
#define T_ONE_WAY_TELE      8   // one-way teleporter tile flag (teleporter flag gets inverted once tele is taken)
#define T_TEXTURE_IDX       9   
#define T_ROW               10  // tex-sheet index
#define T_COL               11  // tile's row-position in level grid
#define T_E_SPAWN_CH        12  // entity spawn channel
#define T_TELE_CH           13  // teleporter channel

#define E_IS_HOSTILE        0   // enemy/entity which does directly harm
#define E_IS_PASSIVE        1   // enemy/entity which does not directly harm
#define E_IS_ACTIVE         2   // basic state variable
#define E_IS_TRG_HEAD       3   // is main point of trigger for other entities on same channel
#define E_IS_TEXTURED       4   // is not blank texture            
#define E_PLAYER_COLL       5   // has player collision
#define E_ENTITY_COLL       6   // has base entity collision             
#define E_PROJ_COLL         7   // has proj. collision            
#define E_ACTIVE_INT        8   // interactable with player "use" key           
#define E_PASSIVE_INT       9   // interactable with player collision         
#define E_EFFECT_MAG        10  // gen. purpose effect magnitude var. 
#define E_DIR_X             11  // projectile/enemy/etc. x sign
#define E_DIR_Y             12  // projectile/enemy/etc. y sign
#define E_TEXTURE_IDX       13  // texture to be drawn passively (i.e. by default)  
#define E_ACTIVE_TEX_IDX    14  // texture to be drawn on true active state
#define E_TRIGGER_CH        15  // default trigger channel -> -1
#define E_POS_X             16  // entity x-position (top-left of ent. with respect to the top-leftmost corner of level)
#define E_POS_Y             17  // entity y-position (top-left of ent. with respect to the top-leftmost corner of level)
#define E_MOVE_SPEED        18  // entity moving step value    
#define E_ANIM_PERIOD       19  // animation period in millis