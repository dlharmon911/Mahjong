#ifndef _HEADER_GUARD_MAHJONG_COMMON_H_
#define _HEADER_GUARD_MAHJONG_COMMON_H_

#include <allegro5/allegro5.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_memfile.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <libogle.h>
#include <libmodeler.h>

#define MAHJONG_DISPLAY_WIDTH	1200
#define MAHJONG_DISPLAY_HEIGHT	800

#define MAHJONG_TILE_COUNT		144
#define MAHJONG_SELECTION_COUNT		2

#define MAHJONG_TILE_WIDTH 1.0f
#define MAHJONG_TILE_HEIGHT 1.25f
#define MAHJONG_TILE_DEPTH 0.40f
#define MAHJONG_TILE_RADIUS 0.05f

enum MAHJONG_TEXTURES
{
	MAHJONG_TEXTURE_EMPTY = -1,
	MAHJONG_TEXTURE_ICON,
	MAHJONG_TEXTURE_TILES,
	MAHJONG_TEXTURE_POINTER,
	MAHJONG_TEXTURE_COUNT
};

enum MAHJONG_FONTS
{
	MAHJONG_FONT_DEFAULT = 0,
	MAHJONG_FONT_TITLE,
	MAHJONG_FONT_SUBTITLE,
	MAHJONG_FONT_GAME,
	MAHJONG_FONT_GAME_SMALL,
	MAHJONG_FONT_GAME_INSTRUCTIONS,
	MAHJONG_FONT_ICONS,
	MAHJONG_FONT_COUNT
};

enum MAHJONG_SAMPLES
{
	MAHJONG_SAMPLE_EMPTY = -1,
	MAHJONG_SAMPLE_CHIP_FLIP,
	MAHJONG_SAMPLE_COUNT
};

enum MAHJONG_SHADER_ID
{
	MAHJONG_SHADER_MATERIAL,
	MAHJONG_SHADER_ID,
	MAHJONG_SHADER_LIGHT,
	MAHJONG_SHADER_DEPTH,
	MAHJONG_SHADER_COUNT
};

enum MAHJONG_MODEL_ID
{
	MAHJONG_MODEL_TILE,
	MAHJONG_MODEL_BOARD,
	MAHJONG_MODEL_COUNT
};

enum MAHJONG_GLYPH_ID
{
	MAHJONG_GLYPH_EMPTY = -1,
	MAHJONG_GLYPH_CHARACTER_1,
	MAHJONG_GLYPH_CHARACTER_2,
	MAHJONG_GLYPH_CHARACTER_3,
	MAHJONG_GLYPH_CHARACTER_4,
	MAHJONG_GLYPH_CHARACTER_5,
	MAHJONG_GLYPH_CHARACTER_6,
	MAHJONG_GLYPH_CHARACTER_7,
	MAHJONG_GLYPH_CHARACTER_8,
	MAHJONG_GLYPH_CHARACTER_9, 
	MAHJONG_GLYPH_BAMBOO_1,
	MAHJONG_GLYPH_BAMBOO_2,
	MAHJONG_GLYPH_BAMBOO_3,
	MAHJONG_GLYPH_BAMBOO_4,
	MAHJONG_GLYPH_BAMBOO_5,
	MAHJONG_GLYPH_BAMBOO_6,
	MAHJONG_GLYPH_BAMBOO_7,
	MAHJONG_GLYPH_BAMBOO_8,
	MAHJONG_GLYPH_BAMBOO_9,
	MAHJONG_GLYPH_CIRCLE_1,
	MAHJONG_GLYPH_CIRCLE_2,
	MAHJONG_GLYPH_CIRCLE_3,
	MAHJONG_GLYPH_CIRCLE_4,
	MAHJONG_GLYPH_CIRCLE_5,
	MAHJONG_GLYPH_CIRCLE_6,
	MAHJONG_GLYPH_CIRCLE_7,
	MAHJONG_GLYPH_CIRCLE_8,
	MAHJONG_GLYPH_CIRCLE_9,
	MAHJONG_GLYPH_WIND_EAST,
	MAHJONG_GLYPH_WIND_SOUTH,
	MAHJONG_GLYPH_WIND_WEST,
	MAHJONG_GLYPH_WIND_NORTH,
	MAHJONG_GLYPH_SEASON_SPRING,
	MAHJONG_GLYPH_SEASON_SUMMER,
	MAHJONG_GLYPH_SEASON_AUTUMN,
	MAHJONG_GLYPH_SEASON_WINTER,
	MAHJONG_GLYPH_FLOWER_PLUM,
	MAHJONG_GLYPH_FLOWER_ORCHID,
	MAHJONG_GLYPH_FLOWER_CHRYSANTHEMUM,
	MAHJONG_GLYPH_FLOWER_BAMBOO,
	MAHJONG_GLYPH_DRAGON_RED,
	MAHJONG_GLYPH_DRAGON_GREEN,
	MAHJONG_GLYPH_DRAGON_WHITE,
	MAHJONG_GLYPH_COUNT
};

enum MAHJONG_FLAGS
{
	MAHJONG_FLAG_NONE = 0,
	MAHJONG_FLAG_SELECTED = 1 << 0,
	MAHJONG_FLAG_OPEN = 1 << 1,
	MAHJONG_FLAG_VISIBLE = 1 << 2
};

typedef struct mah_data_render_tag_t mah_data_render_t;
typedef struct mah_data_main_tag_t mah_data_main_t;
typedef struct mah_layout_tag_t mah_layout_t;
typedef struct mah_texture_data_tag_t mah_texture_data_t;
typedef struct mah_board_tag_t mah_board_t;
typedef struct mah_tile_tag_t mah_tile_t;

struct mah_tile_tag_t
{
	int32_t m_id;
	int32_t m_flag;
};

struct mah_board_tag_t
{
	mah_tile_t m_tile[MAHJONG_TILE_COUNT];
	int32_t m_tile_selection[MAHJONG_SELECTION_COUNT];
	int32_t m_tile_selection_count;
	int32_t m_hover_tile_id;
};

struct mah_data_render_tag_t
{
	o_world_transform_t m_world_transform;
	o_light_t m_light;
	o_camera_t m_camera;
	ALLEGRO_VERTEX_DECL* m_vertex_decl;
	ALLEGRO_FONT* m_atlas_font[MAHJONG_FONT_COUNT];
	ALLEGRO_BITMAP* m_atlas_texture[MAHJONG_TEXTURE_COUNT];
	mod_model_t* m_atlas_model[MAHJONG_MODEL_COUNT];
	ALLEGRO_SHADER* m_atlas_shader[MAHJONG_SHADER_COUNT];
	ALLEGRO_SAMPLE* m_atlas_sample[MAHJONG_SAMPLE_COUNT];
	ALLEGRO_BITMAP* m_shadow_map;
	mod_model_view_t m_tile_view[MAHJONG_TILE_COUNT];
	mah_board_t m_board;
	bool m_show_debug;
};

struct mah_data_main_tag_t
{
	ALLEGRO_DISPLAY* m_display;
	ALLEGRO_TIMER* m_timer;
	ALLEGRO_EVENT_QUEUE* m_event_queue;
	o_input_t* m_input;
	mah_data_render_t* m_render;
	int32_t m_id;
	int32_t m_id_last;
	float delta_time;
	double m_fps;
	bool m_update;
	bool m_draw_stats;
	bool m_running;
	bool m_draw_mouse;
};

struct mah_layout_tag_t
{
	float m_x;
	float m_y;
	float m_z;
	int16_t m_left;
	int16_t m_right;
	int16_t m_up;
};

struct mah_texture_data_tag_t
{
	ALLEGRO_BITMAP* m_texture;
	int32_t m_glyph_width;
	int32_t m_glyph_height;
};


const char* mah_glyph_get_class_name(int32_t glyph_index);
const char* mah_glyph_get_item_name(int32_t glyph_index);

void mah_data_render_zero(mah_data_render_t* data);
int32_t mah_data_render_initialize(mah_data_render_t* data, ALLEGRO_DISPLAY* display);
void mah_data_render_uninitialize(mah_data_render_t* data);

void mah_main_update(mah_data_main_t* data);
void mah_main_render(const mah_data_main_t* data, bool id_only);

void mah_update_mouse_down(mah_data_main_t* data);
void mah_update_mouse_over(mah_data_main_t* data);

const mah_layout_t* mah_layout_get(void);
const mah_layout_t* mah_layout_get_tile(int32_t index);
int32_t mah_atlas_model_generate(mod_model_view_t* tile_view, mod_model_t** atlas, const mah_texture_data_t* texture_data, const mah_layout_t* layout);
void mah_tile_set_texture(mod_model_t** atlas, const mah_texture_data_t* texture_data, int32_t glyph_index, int32_t tile_index);
void mah_tile_model_set_texture(mod_model_t* tile_model, int32_t index, const mah_texture_data_t* texture_data);
void mah_board_shuffle(mod_model_t** atlas, mah_board_t* board, const mah_texture_data_t* texture_data);
void mah_board_reset_visibility(mod_model_t** atlas, mah_board_t* board);
void mah_board_set_visibility(mod_model_t** atlas, mah_board_t* board, int32_t tile_id, bool visible);
void mah_board_toggle_visibility(mod_model_t** atlas, mah_board_t* board, int32_t tile_id);
int32_t mah_board_moves_remaining(const mah_board_t* board);
int32_t mah_board_tiles_removed(const mah_board_t* board);
bool mah_tile_is_open(const mah_board_t* board, int32_t tile_id);
bool mah_tile_matches(const mah_board_t* board, int32_t first_tile_id, int32_t second_tile_id);


#endif // _HEADER_GUARD_MAHJONG_COMMON_H_