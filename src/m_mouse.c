#include "m_common.h"

void mah_update_mouse_down(mah_data_main_t* data)
{
	// TODO: Implement mouse down logic for Mahjong tiles.
	o_vertex_t* vertices = mod_model_get_vertices(data->m_render->m_atlas_model[MAHJONG_MODEL_BOARD]);
	size_t vertex_count = mod_model_get_vertex_count(data->m_render->m_atlas_model[MAHJONG_MODEL_BOARD]);

	if (data->m_render->m_board.m_tile_selection_count == 2 ||
		data->m_id == data->m_render->m_board.m_tile_selection[0] ||
		data->m_id == data->m_render->m_board.m_tile_selection[1])
	{
		return;
	}

	int32_t id = data->m_id;

	if (!mah_tile_is_open(&data->m_render->m_board, id))
	{
		return;
	}

	if (data->m_render->m_board.m_tile_selection_count == 1 &&
		data->m_render->m_board.m_tile_selection[0] == 1 + id)
	{
		return;
	}

	data->m_render->m_board.m_tile_selection[data->m_render->m_board.m_tile_selection_count] = 1 + id;
	data->m_render->m_board.m_tile[id].m_flag |= MAHJONG_FLAG_SELECTED;

	++data->m_render->m_board.m_tile_selection_count;

	for (size_t i = 0; i < vertex_count; ++i)
	{
		o_vertex_t* vertex = &vertices[i];
		int32_t meta_id = (int32_t)vertex->m_meta;

		if (data->m_render->m_board.m_tile_selection_count > 0 && meta_id == data->m_render->m_board.m_tile_selection[0] ||
			data->m_render->m_board.m_tile_selection_count == 2 && meta_id == data->m_render->m_board.m_tile_selection[1])
		{
			vertex->m_meta = -vertex->m_meta;
		}
	}
}

void mah_update_mouse_over(mah_data_main_t* data)
{
	// TODO: Implement mouse over logic for Mahjong tiles.

	if (data->m_id < 0)
	{
		data->m_render->m_board.m_hover_tile_id = -1;
		return;
	}

	data->m_render->m_board.m_hover_tile_id = data->m_id;
}
