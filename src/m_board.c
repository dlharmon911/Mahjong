#include "m_common.h"


typedef struct m_glyph_group_tag_t m_glyph_group_t;

struct m_glyph_group_tag_t
{
	int32_t m_glyph_count;
	int32_t m_multiple;
};

static const m_glyph_group_t g_glyph_groups[MAHJONG_GLYPH_COUNT] =
{
	{ 9, 4 }, // Characters
	{ 9, 4 }, // Bamboos
	{ 9, 4 }, // Circles
	{ 4, 4 }, // Winds
	{ 4, 1 }, // Seasons
	{ 4, 1 }, // Flowers
	{ 3, 4 }, // Dragons
	{ -1, -1 }
};

static void mah_board_reset_zero(mod_model_t** atlas, mah_board_t* board, const mah_texture_data_t* texture_data)
{
	const m_glyph_group_t* glyph_group = g_glyph_groups;
	int32_t base_index = 0;

	size_t board_vertex_count = mod_model_get_vertex_count(atlas[MAHJONG_MODEL_BOARD]);
	size_t tile_vertex_count = board_vertex_count / MAHJONG_TILE_COUNT;
	o_vertex_t* vertices = mod_model_get_vertices(atlas[MAHJONG_MODEL_BOARD]);
	int32_t tile_index = 0;

	while (glyph_group->m_glyph_count != -1)
	{
		int32_t glyph_count = glyph_group->m_glyph_count;
		int32_t multiple = glyph_group->m_multiple;

		for (int32_t i = 0; i < glyph_count; ++i)
		{
			int32_t glyph_index = 1 + base_index + i;

			for (int32_t j = 0; j < multiple; ++j)
			{
				board->m_tile[tile_index].m_id = glyph_index;

				++tile_index;
			}
		}

		base_index += glyph_count;
		++glyph_group;
	}

	board->m_hover_tile_id = -1;
}

void mah_board_shuffle(mod_model_t** atlas, mah_board_t* board, const mah_texture_data_t* texture_data)
{
	if (!board || !texture_data)
	{
		return;
	}
	
	mah_board_reset_zero(atlas, board, texture_data);
	
	// Shuffle the tile IDs using Fisher-Yates algorithm
	for (int32_t i = MAHJONG_TILE_COUNT - 1; i > 0; --i)
	{
		int32_t j = rand() % (i + 1);
		int32_t temp = board->m_tile[i].m_id;
		board->m_tile[i].m_id = board->m_tile[j].m_id;
		board->m_tile[j].m_id = temp;
	}

	o_vertex_t* vertices = mod_model_get_vertices(atlas[MAHJONG_MODEL_BOARD]);

	// Update the texture coordinates for each tile based on the shuffled IDs
	for (int32_t i = 0; i < MAHJONG_TILE_COUNT; ++i)
	{
		int32_t glyph_index = board->m_tile[i].m_id;
		mah_tile_set_texture(atlas, texture_data, glyph_index, i);
	}

	mah_board_reset_visibility(atlas, board);
}

void mah_board_reset_visibility(mod_model_t** atlas, mah_board_t* board)
{
	if (!board)
	{
		return;
	}
	o_vertex_t* vertices = mod_model_get_vertices(atlas[MAHJONG_MODEL_BOARD]);
	size_t vertex_count = mod_model_get_vertex_count(atlas[MAHJONG_MODEL_BOARD]);
	for (size_t i = 0; i < vertex_count; ++i)
	{
		o_vertex_t* vertex = &vertices[i];
		vertex->m_meta = fabs(vertex->m_meta);
	}

	for (int32_t i = 0; i < MAHJONG_TILE_COUNT; ++i)
	{
		board->m_tile[i].m_flag = MAHJONG_FLAG_VISIBLE;
	}

	board->m_tile_selection_count = 0;
	board->m_tile_selection[0] = -1;
	board->m_tile_selection[1] = -1;
}

void mah_board_set_visibility(mod_model_t** atlas, mah_board_t* board, int32_t tile_id, bool visible)
{
	if (NULL == board || tile_id < 0 || tile_id >= MAHJONG_TILE_COUNT)
	{
		return;
	}

	board->m_tile[tile_id].m_flag = visible ? MAHJONG_FLAG_VISIBLE : MAHJONG_FLAG_NONE;

	o_vertex_t* vertices = mod_model_get_vertices(atlas[MAHJONG_MODEL_BOARD]);
	size_t vertex_count = mod_model_get_vertex_count(atlas[MAHJONG_MODEL_BOARD]);
	for (size_t i = 0; i < vertex_count; ++i)
	{
		o_vertex_t* vertex = &vertices[i];
		int32_t id = (int32_t)fabs(vertex->m_meta);
		if (id == tile_id + 1)
		{
			vertex->m_meta = visible ? fabs(vertex->m_meta) : -fabs(vertex->m_meta);
		}
	}
}

void mah_board_toggle_visibility(mod_model_t** atlas, mah_board_t* board, int32_t tile_id)
{
	if (!board)
	{
		return;
	}
	o_vertex_t* vertices = mod_model_get_vertices(atlas[MAHJONG_MODEL_BOARD]);
	size_t vertex_count = mod_model_get_vertex_count(atlas[MAHJONG_MODEL_BOARD]);

	board->m_tile[tile_id].m_flag ^= MAHJONG_FLAG_VISIBLE;

	for (size_t i = 0; i < vertex_count; ++i)
	{
		o_vertex_t* vertex = &vertices[i];
		int32_t id = (int32_t)fabs(vertex->m_meta);
		if (id == tile_id + 1)
		{
			vertex->m_meta = -vertex->m_meta;
		}
	}
}

int32_t mah_board_moves_remaining(const mah_board_t* board)
{
	int32_t moves_remaining = 0;

	if (NULL == board)
	{
		return 0;
	}

	for (int32_t i = 0; i < MAHJONG_TILE_COUNT; ++i)
	{
		bool match = false;

		if ((board->m_tile[i].m_flag & MAHJONG_FLAG_VISIBLE) &&
			mah_tile_is_open(board, i))
		{
			for (int32_t j = i + 1; j < MAHJONG_TILE_COUNT; ++j)
			{
				if ((board->m_tile[j].m_flag & MAHJONG_FLAG_VISIBLE) &&
					mah_tile_is_open(board, j))
				{
					if (mah_tile_matches(board, i, j))
					{
						match = true;
					}
				}
			}
		}

		if (match)
		{
			++moves_remaining;
		}
	}

	return moves_remaining;
}

int32_t mah_board_tiles_removed(const mah_board_t* board)
{
	int32_t tiles_removed = 0;
	if (NULL == board)
	{
		return 0;
	}
	for (int32_t i = 0; i < MAHJONG_TILE_COUNT; ++i)
	{
		if (!(board->m_tile[i].m_flag == MAHJONG_FLAG_NONE))
		{
			++tiles_removed;
		}
	}

	return MAHJONG_TILE_COUNT - tiles_removed;
}