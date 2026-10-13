#include "m_common.h"

typedef struct mah_render_context_tag_t mah_render_context_t;

struct mah_render_context_tag_t
{
	ALLEGRO_BITMAP* m_target;
	ALLEGRO_VERTEX_DECL* m_vertex_decl;
	mod_model_t* m_big_model;
	ALLEGRO_BITMAP* m_texture;
	o_vector2_t m_resolution;
	float m_aspect_ratio;
	o_vector2_t m_top_left;
	o_vector2_t m_bottom_right;
	o_vector3_t m_scale;
	o_vector3_t m_position;
	o_transform_t m_model_transform;
	ALLEGRO_TRANSFORM m_light_projection_transform;
	ALLEGRO_TRANSFORM m_light_view_transform;
};

static void mah_main_render_depth(const mah_data_main_t* data, mah_render_context_t* context)
{
	if (NULL == data)
	{
		return;
	}

	o_camera_t light_camera =
	{
		data->m_render->m_light.m_position,
		(o_vector3_t) { 0.0f, 0.0f, 0.0f },
		(o_vector3_t) { 0.0f, 1.0f, 0.0f }
	};

#define S_ORTHO_BOUNDS 20.0f
#define S_ORTHO_NEAR_PLANE 30.0f
#define S_ORTHO_FAR_PLANE -30.0f

	context->m_top_left = (o_vector2_t){ -S_ORTHO_BOUNDS, -S_ORTHO_BOUNDS };
	context->m_bottom_right = (o_vector2_t){ S_ORTHO_BOUNDS, S_ORTHO_BOUNDS };

	ogle_transform_identity(&context->m_light_projection_transform);
	ogle_transform_orthographic(&context->m_light_projection_transform, context->m_top_left, context->m_bottom_right, S_ORTHO_NEAR_PLANE, S_ORTHO_FAR_PLANE);
	ogle_transform_camera_build(&context->m_light_view_transform, &light_camera);
	ogle_transform_identity(&data->m_render->m_world_transform.m_model);
	ogle_transform_identity(&context->m_model_transform);
	ogle_transform_scale_3d(&context->m_model_transform, context->m_scale);
	ogle_transform_translate_3d(&context->m_model_transform, context->m_position);
	ogle_transform_compose(&context->m_model_transform, &data->m_render->m_world_transform.m_model);

	al_clear_to_color(al_map_rgb(0xff, 0xff, 0xff));
	al_clear_depth_buffer(1.0f);
	al_use_shader(data->m_render->m_atlas_shader[MAHJONG_SHADER_DEPTH]);
	ogle_light_set_shader("u_light", &data->m_render->m_light);
	ogle_camera_set_shader("u_camera", &data->m_render->m_camera);
	ogle_transform_set_shader("u_projection_matrix", &context->m_light_projection_transform);
	ogle_transform_set_shader("u_view_matrix", &context->m_light_view_transform);
	ogle_transform_identity(&context->m_model_transform);
	ogle_transform_scale_3d(&context->m_model_transform, context->m_scale);
	ogle_transform_translate_3d(&context->m_model_transform, context->m_position);
	ogle_transform_compose(&context->m_model_transform, &data->m_render->m_world_transform.m_model);
	ogle_transform_set_shader("u_model_matrix", &context->m_model_transform);
	mod_model_render(context->m_vertex_decl, context->m_big_model, NULL);

	for (int32_t i = 0; i < data->m_render->m_board.m_tile_selection_count; ++i)
	{
		int32_t index = data->m_render->m_board.m_tile_selection[i] - 1;
		const mah_layout_t* layout = mah_layout_get() + index;
		o_vector3_t position = { 0 };
		mod_model_t* tile_model = data->m_render->m_atlas_model[MAHJONG_MODEL_TILE];

		position.m_x = context->m_position.m_x + layout->m_x * MAHJONG_TILE_WIDTH - 6.0f;
		position.m_y = context->m_position.m_y + layout->m_y * MAHJONG_TILE_HEIGHT - 4.0f;
		position.m_z = context->m_position.m_z + layout->m_z * MAHJONG_TILE_DEPTH;

		ogle_transform_identity(&context->m_model_transform);
		ogle_transform_scale_3d(&context->m_model_transform, context->m_scale);
		ogle_transform_translate_3d(&context->m_model_transform, position);
		ogle_transform_compose(&context->m_model_transform, &data->m_render->m_world_transform.m_model);
		ogle_transform_set_shader("u_model_matrix", &context->m_model_transform);
		mod_model_render(context->m_vertex_decl, tile_model, NULL);
	}

	al_use_shader(NULL);
}

static void mah_main_render_light(const mah_data_main_t* data, mah_render_context_t* context)
{
	if (NULL == data)
	{
		return;
	}

	const o_material_t* material = ogle_material_get(OGLE_MATERIAL_IVORY);

	context->m_resolution = (o_vector2_t){ (float)al_get_bitmap_width(context->m_target), (float)al_get_bitmap_height(context->m_target) };
	context->m_aspect_ratio = context->m_resolution.m_y / context->m_resolution.m_x;
	context->m_top_left = (o_vector2_t){ -1.0f, context->m_aspect_ratio };
	context->m_bottom_right = (o_vector2_t){ 1.0f, -context->m_aspect_ratio };

	ogle_transform_identity(&data->m_render->m_world_transform.m_projection);
	ogle_transform_perspective(&data->m_render->m_world_transform.m_projection, context->m_top_left, context->m_bottom_right, 1.0f, 100.0f);
	ogle_transform_camera_build(&data->m_render->m_world_transform.m_view, &data->m_render->m_camera);
	ogle_transform_identity(&data->m_render->m_world_transform.m_model);

	ogle_transform_identity(&context->m_model_transform);
	ogle_transform_scale_3d(&context->m_model_transform, context->m_scale);
	ogle_transform_translate_3d(&context->m_model_transform, context->m_position);
	ogle_transform_compose(&context->m_model_transform, &data->m_render->m_world_transform.m_model);


	al_clear_to_color(al_map_rgb(0x16, 0x16, 0x21));
	al_clear_depth_buffer(1.0f);
	al_use_shader(data->m_render->m_atlas_shader[MAHJONG_SHADER_LIGHT]);
	ogle_material_set_shader("u_material", material);
	al_set_shader_sampler("u_shadow_map", data->m_render->m_shadow_map, 1);
	ogle_transform_set_shader("u_light_projection", &context->m_light_projection_transform);
	ogle_transform_set_shader("u_light_view", &context->m_light_view_transform);
	ogle_light_set_shader("u_light", &data->m_render->m_light);
	ogle_camera_set_shader("u_camera", &data->m_render->m_camera);
	ogle_transform_set_shader("u_projection_matrix", &data->m_render->m_world_transform.m_projection);
	ogle_transform_set_shader("u_view_matrix", &data->m_render->m_world_transform.m_view);
	ogle_transform_identity(&context->m_model_transform);
	ogle_transform_scale_3d(&context->m_model_transform, context->m_scale);
	ogle_transform_translate_3d(&context->m_model_transform, context->m_position);
	ogle_transform_compose(&context->m_model_transform, &data->m_render->m_world_transform.m_model);
	ogle_transform_set_shader("u_model_matrix", &context->m_model_transform);
	mod_model_render(context->m_vertex_decl, context->m_big_model, context->m_texture);

	mah_texture_data_t texture_data =
	{
		.m_texture = data->m_render->m_atlas_texture[MAHJONG_TEXTURE_TILES],
		.m_glyph_width = 32,
		.m_glyph_height = 44
	};

	for (int32_t i = 0; i < data->m_render->m_board.m_tile_selection_count; ++i)
	{
		int32_t index = data->m_render->m_board.m_tile_selection[i] - 1;
		int32_t tile_id = data->m_render->m_board.m_tile[index].m_id;
		const mah_layout_t* layout = mah_layout_get() + index;
		o_vector3_t position = { 0 };
		mod_model_t* tile_model = data->m_render->m_atlas_model[MAHJONG_MODEL_TILE];

		mah_tile_model_set_texture(tile_model, tile_id, &texture_data);
	
		position.m_x = context->m_position.m_x + layout->m_x * MAHJONG_TILE_WIDTH - 6.0f;
		position.m_y = context->m_position.m_y + layout->m_y * MAHJONG_TILE_HEIGHT - 4.0f;
		position.m_z = context->m_position.m_z + layout->m_z * MAHJONG_TILE_DEPTH;

		const o_material_t* material = ogle_material_get(OGLE_MATERIAL_PEARL);
		ogle_material_set_shader("u_material", material);
		ogle_transform_identity(&context->m_model_transform);
		ogle_transform_scale_3d(&context->m_model_transform, context->m_scale);
		ogle_transform_translate_3d(&context->m_model_transform, position);
		ogle_transform_compose(&context->m_model_transform, &data->m_render->m_world_transform.m_model);
		ogle_transform_set_shader("u_model_matrix", &context->m_model_transform);
		mod_model_render(context->m_vertex_decl, tile_model, context->m_texture);
	}

	al_use_shader(NULL);
}

static void mah_main_render_id(const mah_data_main_t* data, mah_render_context_t* context)
{
	if (NULL == data)
	{
		return;
	}

	context->m_resolution = (o_vector2_t){ (float)al_get_bitmap_width(context->m_target), (float)al_get_bitmap_height(context->m_target) };
	context->m_aspect_ratio = context->m_resolution.m_y / context->m_resolution.m_x;
	context->m_top_left = (o_vector2_t){ -1.0f, context->m_aspect_ratio };
	context->m_bottom_right = (o_vector2_t){ 1.0f, -context->m_aspect_ratio };

	ogle_transform_identity(&data->m_render->m_world_transform.m_projection);
	ogle_transform_perspective(&data->m_render->m_world_transform.m_projection, context->m_top_left, context->m_bottom_right, 1.0f, 100.0f);
	ogle_transform_camera_build(&data->m_render->m_world_transform.m_view, &data->m_render->m_camera);
	ogle_transform_identity(&data->m_render->m_world_transform.m_model);

	ogle_transform_identity(&context->m_model_transform);
	ogle_transform_scale_3d(&context->m_model_transform, context->m_scale);
	ogle_transform_translate_3d(&context->m_model_transform, context->m_position);
	ogle_transform_compose(&context->m_model_transform, &data->m_render->m_world_transform.m_model);

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_clear_depth_buffer(1.0f);
	al_use_shader(data->m_render->m_atlas_shader[MAHJONG_SHADER_ID]);
	ogle_light_set_shader("u_light", &data->m_render->m_light);
	ogle_camera_set_shader("u_camera", &data->m_render->m_camera);
	ogle_transform_set_shader("u_projection_matrix", &data->m_render->m_world_transform.m_projection);
	ogle_transform_set_shader("u_view_matrix", &data->m_render->m_world_transform.m_view);
	ogle_transform_identity(&context->m_model_transform);
	ogle_transform_scale_3d(&context->m_model_transform, context->m_scale);
	ogle_transform_translate_3d(&context->m_model_transform, context->m_position);
	ogle_transform_compose(&context->m_model_transform, &data->m_render->m_world_transform.m_model);
	ogle_transform_set_shader("u_model_matrix", &context->m_model_transform);
	mod_model_render(context->m_vertex_decl, context->m_big_model, context->m_texture);

	for (int32_t i = 0; i < data->m_render->m_board.m_tile_selection_count; ++i)
	{
		int32_t index = data->m_render->m_board.m_tile_selection[i] - 1;
		int32_t tile_id = data->m_render->m_board.m_tile[index].m_id;
		const mah_layout_t* layout = mah_layout_get() + index;
		o_vector3_t position = { 0 };
		mod_model_t* tile_model = data->m_render->m_atlas_model[MAHJONG_MODEL_TILE];

		o_vertex_t* vertices = mod_model_get_vertices(tile_model);
		size_t vertex_count = mod_model_get_vertex_count(tile_model);

		for (size_t j = 0; j < vertex_count; ++j)
		{
			o_vertex_t* vertex = &vertices[j];
			vertex->m_meta = (float)(index + 1);
		}

		position.m_x = context->m_position.m_x + layout->m_x * MAHJONG_TILE_WIDTH - 6.0f;
		position.m_y = context->m_position.m_y + layout->m_y * MAHJONG_TILE_HEIGHT - 4.0f;
		position.m_z = context->m_position.m_z + layout->m_z * MAHJONG_TILE_DEPTH;

		ogle_transform_identity(&context->m_model_transform);
		ogle_transform_scale_3d(&context->m_model_transform, context->m_scale);
		ogle_transform_translate_3d(&context->m_model_transform, position);
		ogle_transform_compose(&context->m_model_transform, &data->m_render->m_world_transform.m_model);
		ogle_transform_set_shader("u_model_matrix", &context->m_model_transform);
		mod_model_render(context->m_vertex_decl, tile_model, context->m_texture);
	}

	al_use_shader(NULL);
}

static void mah_hover_render(const mah_data_main_t* data)
{
	const char* main_message = "Hovering over tile: %d";
	const char* class_message = "Class: %s";
	const char* item_message = "Sub class: %s";
	int32_t id = data->m_render->m_board.m_hover_tile_id;
	ALLEGRO_FONT* font = data->m_render->m_atlas_font[MAHJONG_FONT_DEFAULT];
	ALLEGRO_COLOR color = al_map_rgb(0xff, 0xff, 0xff);
	const mah_tile_t* tile = &data->m_render->m_board.m_tile[id];
	bool left = false;
	bool right = false;
	bool top = false;
	const mah_layout_t* layout = mah_layout_get() + id;
	float by = 10.0f;
	int32_t y = 0;

	al_draw_textf(font, color, 10.0f, by + (float)(y++ * 20), ALLEGRO_ALIGN_LEFT, "Moves Remaining: %d", mah_board_moves_remaining(&data->m_render->m_board));
	al_draw_textf(font, color, 10.0f, by + (float)(y++ * 20), ALLEGRO_ALIGN_LEFT, "Tiles Removed: %d", mah_board_tiles_removed(&data->m_render->m_board));

	if (data->m_render->m_board.m_hover_tile_id >= 0)
	{
		if (layout->m_left >= 0)
		{
			const mah_tile_t* tile = &data->m_render->m_board.m_tile[layout->m_left];
			if ((tile->m_flag & MAHJONG_FLAG_VISIBLE) && mah_tile_is_open(&data->m_render->m_board, layout->m_left))
			{
				left = true;
			}
		}

		if (layout->m_right >= 0)
		{
			const mah_tile_t* tile = &data->m_render->m_board.m_tile[layout->m_right];
			if ((tile->m_flag & MAHJONG_FLAG_VISIBLE) && mah_tile_is_open(&data->m_render->m_board, layout->m_right))
			{
				right = true;
			}
		}
	
		if (layout->m_up >= 0)
		{
			const mah_tile_t* tile = &data->m_render->m_board.m_tile[layout->m_up];
			if ((tile->m_flag & MAHJONG_FLAG_VISIBLE) && mah_tile_is_open(&data->m_render->m_board, layout->m_up))
			{
				top = true;
			}
		}

		al_draw_textf(font, color, 10.0f, by + (float)(y++ * 20), ALLEGRO_ALIGN_LEFT, main_message, id);
		al_draw_textf(font, color, 16.0f, by + (float)(y++ * 20), ALLEGRO_ALIGN_LEFT, class_message, mah_glyph_get_class_name(tile->m_id - 1));
		al_draw_textf(font, color, 16.0f, by + (float)(y++ * 20), ALLEGRO_ALIGN_LEFT, item_message, mah_glyph_get_item_name(tile->m_id - 1));

		if (data->m_render->m_show_debug)
		{
			al_draw_textf(font, color, 16.0f, by + (float)(y++ * 20), ALLEGRO_ALIGN_LEFT, "ID: %d", tile->m_id - 1);
			al_draw_textf(font, color, 16.0f, by + (float)(y++ * 20), ALLEGRO_ALIGN_LEFT, "Left: %s", left ? "Yes" : "No");
			al_draw_textf(font, color, 16.0f, by + (float)(y++ * 20), ALLEGRO_ALIGN_LEFT, "Right: %s", right ? "Yes" : "No");
			al_draw_textf(font, color, 16.0f, by + (float)(y++ * 20), ALLEGRO_ALIGN_LEFT, "Top: %s", top ? "Yes" : "No");
		}
	}
	else
	{
		al_draw_textf(font, color, 10.0f, by + (float)(y++ * 20), ALLEGRO_ALIGN_LEFT, "Hovering over tile: None");
	}

	by = (float)al_get_bitmap_height(al_get_target_bitmap()) - 20.0f;
	y = 0;

	al_draw_textf(font, color, 10.0f, by + (float)(y-- * 20), ALLEGRO_ALIGN_LEFT, "F10: Shuffle");
	al_draw_textf(font, color, 10.0f, by + (float)(y-- * 20), ALLEGRO_ALIGN_LEFT, "F2: Reset");
}

void mah_main_render(const mah_data_main_t* data, bool id_only)
{
	if (NULL == data)
	{
		return;
	}

	mah_render_context_t context=
	{
		.m_target = NULL,
		.m_vertex_decl = data->m_render->m_vertex_decl,
		.m_big_model = data->m_render->m_atlas_model[MAHJONG_MODEL_BOARD],
		.m_texture = data->m_render->m_atlas_texture[MAHJONG_TEXTURE_TILES],
		.m_resolution = { 0.0f, 0.0f },
		.m_aspect_ratio = 0.0f,
		.m_top_left = { 0.0f, 0.0f },
		.m_bottom_right = { 0.0f, 0.0f },
		.m_scale = { 1.0f, 1.0f, 1.0f },
		.m_position = { 0.0f, 0.0f, 0.0f },
		.m_model_transform = { 0 },
	};

	context.m_target = al_get_target_bitmap();

	if (id_only)
	{
		mah_main_render_id(data, &context);
	}
	else
	{
		al_set_target_bitmap(data->m_render->m_shadow_map);
		mah_main_render_depth(data, &context);
		al_set_target_bitmap(context.m_target);
		mah_main_render_light(data, &context);

		mah_hover_render(data);

		al_flip_display();
	}
}
