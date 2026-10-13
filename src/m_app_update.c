#include "m_common.h"

void mah_main_update(mah_data_main_t* data)
{
	if (NULL == data)
	{
		return;
	}

	if (data->m_render->m_board.m_tile_selection_count == 2)
	{
		int32_t first_tile_id = data->m_render->m_board.m_tile_selection[0] - 1;
		int32_t second_tile_id = data->m_render->m_board.m_tile_selection[1] - 1;

		if (first_tile_id >= 0 && first_tile_id < MAHJONG_TILE_COUNT &&
			second_tile_id >= 0 && second_tile_id < MAHJONG_TILE_COUNT)
		{
			if (mah_tile_matches(&data->m_render->m_board, first_tile_id, second_tile_id))
			{
				if (first_tile_id >= 0 && first_tile_id < MAHJONG_TILE_COUNT)
				{
					data->m_render->m_board.m_tile[first_tile_id].m_flag = MAHJONG_FLAG_NONE;
				}

				if (second_tile_id >= 0 && second_tile_id < MAHJONG_TILE_COUNT)
				{
					data->m_render->m_board.m_tile[second_tile_id].m_flag = MAHJONG_FLAG_NONE;
				}
			}
			else
			{
				if (first_tile_id >= 0 && first_tile_id < MAHJONG_TILE_COUNT)
				{
					mah_board_set_visibility(data->m_render->m_atlas_model, &data->m_render->m_board, first_tile_id, true);
					data->m_render->m_board.m_tile[first_tile_id].m_flag &= ~MAHJONG_FLAG_SELECTED;
				}

				if (second_tile_id >= 0 && second_tile_id < MAHJONG_TILE_COUNT)
				{
					mah_board_set_visibility(data->m_render->m_atlas_model, &data->m_render->m_board, second_tile_id, true);
					data->m_render->m_board.m_tile[second_tile_id].m_flag &= ~MAHJONG_FLAG_SELECTED;
				}
			}
		}

		data->m_render->m_board.m_tile_selection_count = 0;
		data->m_render->m_board.m_tile_selection[0] = -1;
		data->m_render->m_board.m_tile_selection[1] = -1;
		ogle_input_reset(data->m_input);

		return;
	}

	const o_input_keyboard_t* keyboard_input = ogle_input_get_type(data->m_input, OGLE_INPUT_TYPE_KEYBOARD);

	if (ogle_input_keyboard_button_was_pressed(keyboard_input, ALLEGRO_KEY_ESCAPE))
	{
		data->m_running = false;
	}

	if (ogle_input_keyboard_button_was_pressed(keyboard_input, ALLEGRO_KEY_F2))
	{
		mah_board_reset_visibility(data->m_render->m_atlas_model, &data->m_render->m_board);
	}

	if (ogle_input_keyboard_button_was_pressed(keyboard_input, ALLEGRO_KEY_F11))
	{
		data->m_render->m_show_debug = !data->m_render->m_show_debug;
	}

	if (ogle_input_keyboard_button_was_pressed(keyboard_input, ALLEGRO_KEY_F10))
	{
		mah_texture_data_t texture_data =
		{
			.m_texture = data->m_render->m_atlas_texture[MAHJONG_TEXTURE_TILES],
			.m_glyph_width = 32,
			.m_glyph_height = 44
		};

		mah_board_shuffle(data->m_render->m_atlas_model, &data->m_render->m_board, &texture_data);
	}

	if (data->m_id != data->m_id_last)
	{
		data->m_id_last = data->m_id;
		mah_update_mouse_over(data);
	}

	if (ogle_input_mouse_button_was_pressed(ogle_input_get_type(data->m_input, OGLE_INPUT_TYPE_MOUSE), 0))
	{
		mah_update_mouse_down(data);
	}

	if (ogle_input_mouse_button_was_pressed(ogle_input_get_type(data->m_input, OGLE_INPUT_TYPE_MOUSE), 1))
	{
		int32_t first_tile_id = data->m_render->m_board.m_tile_selection[0] - 1;
		int32_t second_tile_id = data->m_render->m_board.m_tile_selection[1] - 1;

		if (data->m_render->m_board.m_tile_selection_count > 0 && first_tile_id >= 0 && first_tile_id < MAHJONG_TILE_COUNT)
		{
			mah_board_set_visibility(data->m_render->m_atlas_model, &data->m_render->m_board, first_tile_id, true);
			data->m_render->m_board.m_tile[first_tile_id].m_flag = MAHJONG_FLAG_VISIBLE;
		}

		if (data->m_render->m_board.m_tile_selection_count > 1 && second_tile_id >= 0 && second_tile_id < MAHJONG_TILE_COUNT)
		{
			mah_board_set_visibility(data->m_render->m_atlas_model, &data->m_render->m_board, second_tile_id, true);
			data->m_render->m_board.m_tile[second_tile_id].m_flag = MAHJONG_FLAG_VISIBLE;
		}

		data->m_render->m_board.m_tile_selection_count = 0;
		data->m_render->m_board.m_tile_selection[0] = -1;
		data->m_render->m_board.m_tile_selection[1] = -1;
	}

	ogle_input_reset(data->m_input);
}
