#include "m_common.h"

static mod_model_t* mah_model_generate_tile_model()
{
	mod_model_t* mod_model = mod_tile_generate(MAHJONG_TILE_WIDTH, MAHJONG_TILE_HEIGHT, MAHJONG_TILE_DEPTH, MAHJONG_TILE_RADIUS, true);

	if (NULL == mod_model)
	{
		ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to generate tile model");
	}

	return mod_model;
}

static void _mah_tile_set_texture(int32_t index, const mah_texture_data_t* texture_data, o_vertex_t* vertices)
{
	if (index < 1 || index > MAHJONG_GLYPH_COUNT || NULL == texture_data || NULL  == texture_data->m_texture || NULL == vertices)
	{
		return;
	}

	o_vector2_t tl = { 0.0f, 0.0f };
	o_vector2_t br = { 0.0f, 0.0f };
	int32_t bitmap_width = al_get_bitmap_width(texture_data->m_texture);
	int32_t bitmap_height = al_get_bitmap_height(texture_data->m_texture);

	int32_t tiles_per_row = bitmap_width / texture_data->m_glyph_width;
	int32_t row = index / tiles_per_row;
	int32_t col = index % tiles_per_row;

	tl.m_x = (float)(col * texture_data->m_glyph_width) / (float)bitmap_width;
	tl.m_y = 1.0f - (float)((row + 1) * texture_data->m_glyph_height) / (float)bitmap_height;
	br.m_x = (float)((col + 1) * texture_data->m_glyph_width) / (float)bitmap_width;
	br.m_y = 1.0f - (float)(row * texture_data->m_glyph_height) / (float)bitmap_height;

	vertices[0].m_uv = (o_vector2_t){ tl.m_x, tl.m_y }; // Top-left
	vertices[1].m_uv = (o_vector2_t){ br.m_x, tl.m_y }; // Top-right
	vertices[2].m_uv = (o_vector2_t){ br.m_x, br.m_y }; // Bottom-right
	vertices[3].m_uv = (o_vector2_t){ tl.m_x, br.m_y }; // Bottom-left
}

void mah_tile_set_texture(mod_model_t** atlas, const mah_texture_data_t* texture_data, int32_t glyph_index, int32_t tile_index)
{
	if (NULL == atlas || NULL == texture_data || NULL == texture_data->m_texture)
	{
		return;
	}

	if (tile_index < 0 || tile_index >= MAHJONG_TILE_COUNT)
	{
		return;
	}

	size_t tile_vertex_count = mod_model_get_vertex_count(atlas[MAHJONG_MODEL_TILE]);
	o_vertex_t* board_vertices = mod_model_get_vertices(atlas[MAHJONG_MODEL_BOARD]);
	o_vertex_t* tile_vertices = board_vertices + (tile_index * tile_vertex_count);

	_mah_tile_set_texture(glyph_index, texture_data, tile_vertices);
}

static o_vertex_t* mah_find_first_vertex_by_id(mod_model_t* model, int32_t id)
{
	if (NULL == model)
	{
		return NULL;
	}
	size_t vertex_count = mod_model_get_vertex_count(model);
	o_vertex_t* vertices = mod_model_get_vertices(model);
	for (size_t i = 0; i < vertex_count; ++i)
	{
		if (vertices[i].m_meta == id)
		{
			return &vertices[i];
		}
	}

	return NULL;
}

void mah_tile_model_set_texture(mod_model_t* tile_model, int32_t index, const mah_texture_data_t* texture_data)
{
	if (NULL == tile_model || NULL == texture_data || NULL == texture_data->m_texture)
	{
		return;
	}
	
	o_vertex_t* vertices = mod_model_get_vertices(tile_model);
	size_t vertex_count = mod_model_get_vertex_count(tile_model);

	for (size_t i = 0; i < vertex_count; ++i)
	{
		(vertices + i)->m_meta = (float)(1 + index);
	}
	
	if (NULL == vertices)
	{
		return;
	}

	_mah_tile_set_texture(index, texture_data, vertices);
}

static mod_model_t* mah_model_generate_board_model(mod_model_view_t* tile_view, mod_model_t* tile_model, const mah_texture_data_t* texture_data, const mah_layout_t* layout)
{
	mod_model_t* big_model = mod_model_create_empty();
	if (!big_model)
	{
		return NULL;
	}

	mod_transform_t model_transform =
	{
		.m_translation = { 0.0f, 0.0f, 0.0f },
		.m_scale = { 1.0f, 1.0f, 1.0f },
		.m_rotation = { 0.0f, 0.0f, 0.0f },
		.m_color = { 1.0f, 1.0f, 1.0f, 1.0f }
	};

	for (int32_t i = 0; i < MAHJONG_TILE_COUNT; ++i)
	{
		const mah_layout_t* tile_layout = mah_layout_get_tile(i);
		int32_t tile_index = 1 + (rand() % MAHJONG_GLYPH_COUNT);

		mah_tile_model_set_texture(tile_model, tile_index, texture_data);

		model_transform.m_translation.m_x = tile_layout->m_x * MAHJONG_TILE_WIDTH - 6.0f;
		model_transform.m_translation.m_y = tile_layout->m_y * MAHJONG_TILE_HEIGHT - 4.0f;
		model_transform.m_translation.m_z = tile_layout->m_z * MAHJONG_TILE_DEPTH;
		mod_model_set_id(tile_model, 1 + i);

		size_t vertex_count = mod_model_get_vertex_count(big_model);
		size_t index_count = mod_model_get_index_count(big_model);

		if (!mod_model_add_model_with_meta_data(big_model, tile_model, &model_transform, false))
		{
			fprintf(stderr, "failed to add model to big model!\n");
			mod_model_destroy(big_model);
			return NULL;
		}

		size_t next_vertex_count = mod_model_get_vertex_count(big_model);
		size_t next_index_count = mod_model_get_index_count(big_model);

		tile_view[i].m_vertices = mod_model_get_vertices(tile_model) + vertex_count;
		tile_view[i].m_vertex_count = next_vertex_count - vertex_count;
		tile_view[i].m_indices = mod_model_get_indices(big_model) + index_count;
		tile_view[i].m_index_count = next_index_count - index_count;
	}

	return big_model;
}

static int32_t _mah_atlas_model_generate(mod_model_view_t* tile_view, mod_model_t** atlas, const mah_texture_data_t* texture_data, const mah_layout_t* layout)
{
	if (NULL == atlas || NULL == layout)
	{
		return -1;
	}

	atlas[MAHJONG_MODEL_TILE] = mah_model_generate_tile_model();
	if (NULL == atlas[MAHJONG_MODEL_TILE])
	{
		return -1;
	}

	atlas[MAHJONG_MODEL_BOARD] = mah_model_generate_board_model(tile_view, atlas[MAHJONG_MODEL_TILE], texture_data, layout);
	if (NULL == atlas[MAHJONG_MODEL_BOARD])
	{
		return -1;
	}

	return 0;
}

static void mah_atlas_zero(mod_model_t** atlas)
{
	if (NULL == atlas)
	{
		return;
	}

	for (int32_t i = 0; i < MAHJONG_MODEL_COUNT; ++i)
	{
		atlas[i] = NULL;
	}
}

static void mah_atlas_destroy(mod_model_t** atlas)
{
	if (NULL == atlas)
	{
		return;
	}

	for (int32_t i = 0; i < MAHJONG_MODEL_COUNT; ++i)
	{
		if (NULL == atlas[i])
		{
			continue;
		}
		
		mod_model_destroy(atlas[i]);
		atlas[i] = NULL;		
	}
}

int32_t mah_atlas_model_generate(mod_model_view_t* tile_view, mod_model_t** atlas, const mah_texture_data_t* texture_data, const mah_layout_t* layout)
{
	if (NULL == atlas || NULL == layout)
	{
		return -1;
	}

	mah_atlas_zero(atlas);

	if (_mah_atlas_model_generate(tile_view, atlas, texture_data, layout) != 0)
	{
		mah_atlas_destroy(atlas);
		return -1;
	}

	return 0;
}

bool mah_tile_is_open(const mah_board_t* board, int32_t tile_id)
{
	if (NULL == board || tile_id < 0 || tile_id >= MAHJONG_TILE_COUNT)
	{
		return false;
	}

	const mah_layout_t* layout = mah_layout_get_tile(tile_id);
	if (NULL == layout)
	{
		return false;
	}

	bool left_open = true;
	bool right_open = true;
	bool top_open = true;
	
	if (layout->m_left >= 0)
	{
		const mah_tile_t* left_tile = &board->m_tile[layout->m_left];
		if (left_tile->m_flag & MAHJONG_FLAG_VISIBLE)
		{
			left_open = false;
		}
	}
	
	if (layout->m_right >= 0)
	{
		const mah_tile_t* right_tile = &board->m_tile[layout->m_right];
		if (right_tile->m_flag & MAHJONG_FLAG_VISIBLE)
		{
			right_open = false;
		}
	}
	
	if (layout->m_up >= 0)
	{
		const mah_tile_t* top_tile = &board->m_tile[layout->m_up];
		if (top_tile->m_flag & MAHJONG_FLAG_VISIBLE)
		{
			top_open = false;
		}
	}

	return (left_open || right_open) && top_open;
}

static bool mah_tile_is_a_flower(int32_t id)
{
	--id;

	return (id >= MAHJONG_GLYPH_FLOWER_PLUM && id <= MAHJONG_GLYPH_FLOWER_BAMBOO);
}

static bool mah_tile_is_a_season(int32_t id)
{
	--id;

	return (id >= MAHJONG_GLYPH_SEASON_SPRING && id <= MAHJONG_GLYPH_SEASON_WINTER);
}

bool mah_tile_matches(const mah_board_t* board, int32_t first_tile_id, int32_t second_tile_id)
{
	if (NULL == board || first_tile_id < 0 || first_tile_id >= MAHJONG_TILE_COUNT || second_tile_id < 0 || second_tile_id >= MAHJONG_TILE_COUNT)
	{
		return false;
	}

	const mah_tile_t* first_tile = &board->m_tile[first_tile_id];
	const mah_tile_t* second_tile = &board->m_tile[second_tile_id];

	if (mah_tile_is_a_season(first_tile->m_id) && mah_tile_is_a_season(second_tile->m_id))
	{
		return true;
	}

	if (mah_tile_is_a_flower(first_tile->m_id) && mah_tile_is_a_flower(second_tile->m_id))
	{
		return true;
	}

	return first_tile->m_id == second_tile->m_id;
}
