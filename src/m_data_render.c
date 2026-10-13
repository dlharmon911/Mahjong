#define ALLEGRO_UNSTABLE

#include "m_common.h"

#define MAHJONG_SHADOW_MAP_WIDTH 2048
#define MAHJONG_SHADOW_MAP_HEIGHT 2048

static const o_light_t g_light =
{
    .m_position = { -8.0f, 8.0f, 15.0f },
    .m_ambient = { 0.8f, 0.8f, 0.8f },
    .m_diffuse = { 1.0f, 1.0f, 1.0f },
    .m_specular = { 0.4f, 0.4f, 0.4f }
};

static const o_camera_t g_camera =
{
    .m_position = { 0.0f, -0.25f, 10.0f },
    .m_lookat = { 0.0f, 0.0f, 0.0f },
    .m_up = { 0.0f, 1.0f, 0.0f }
};

static const char* MAHJONG_FONT_FILENAMES[MAHJONG_FONT_COUNT] =
{
	"assets/fonts/anja.ttf",
	"assets/fonts/anja.ttf",
	"assets/fonts/anja.ttf",
	"assets/fonts/anja.ttf",
	"assets/fonts/anja.ttf",
	"assets/fonts/anja.ttf",
	"assets/fonts/foundation.ttf"
};

static const int32_t MAHJONG_FONT_SIZES[MAHJONG_FONT_COUNT] =
{
	14,
	256,
	128,
	64,
	48,
	28,
	32
};

static const char* MAHJONG_SAMPLE_FILENAMES[MAHJONG_SAMPLE_COUNT] =
{
	"assets/samples/chip.wav"
};

static const char* MAHJONG_TEXTURE_FILENAMES[MAHJONG_TEXTURE_COUNT] =
{
	"assets/images/icon.png",
	"assets/images/tiles.png",
	"assets/images/pointer.png"
};

static const bool MAHJONG_TEXTURE_APPLY_MASK[MAHJONG_TEXTURE_COUNT] =
{
	true,
	true,
	true
};

static const char* MAHJONG_SHADER_VERTEX_FILENAME = "assets/shaders/vertex_%s.glsl";
static const char* MAHJONG_SHADER_PIXEL_FILENAME = "assets/shaders/pixel_%s.glsl";

static const char* MAHJONG_SHADER_PREFIXES[MAHJONG_SHADER_COUNT] =
{
	"material",
	"id",
	"light",
	"depth"
};

void mah_data_render_zero(mah_data_render_t* data)
{
	if (!data)
	{
		return;
	}

	memset(data, 0, sizeof(mah_data_render_t));
}

static int32_t _mah_data_render_initialize(mah_data_render_t* data, ALLEGRO_DISPLAY* display)
{
	if (!data)
	{
		return -1;
	}



	data->m_vertex_decl = ogle_vertex_decl_create();
	if (!data->m_vertex_decl)
	{
		ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to create vertex declaration");
		return -1;
	}


	memcpy(&data->m_light, &g_light, sizeof(o_light_t));
	memcpy(&data->m_camera, &g_camera, sizeof(o_camera_t));

	int32_t depth = al_get_new_bitmap_depth();
	int32_t format = al_get_new_bitmap_format();
	al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ABGR_F32);
	al_set_new_bitmap_depth(24);
	data->m_shadow_map = al_create_bitmap(MAHJONG_SHADOW_MAP_WIDTH, MAHJONG_SHADOW_MAP_HEIGHT);
	al_set_new_bitmap_depth(depth);
	al_set_new_bitmap_format(format);
	if (NULL == data->m_shadow_map)
	{
		ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to create shadow map bitmap");
		return -1;
	}

	for (int32_t i = 0; i < MAHJONG_FONT_COUNT; ++i)
	{
		const char* font_filename = MAHJONG_FONT_FILENAMES[i];
		int32_t font_size = MAHJONG_FONT_SIZES[i];
		data->m_atlas_font[i] = al_load_font(font_filename, font_size, ALLEGRO_TTF_NO_KERNING);
		if (!data->m_atlas_font[i])
		{
			ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to load font: %s", font_filename);
			return -1;
		}
	}

	for (int32_t i = 0; i < MAHJONG_TEXTURE_COUNT; ++i)
	{
		const char* texture_filename = MAHJONG_TEXTURE_FILENAMES[i];
		bool apply_mask = MAHJONG_TEXTURE_APPLY_MASK[i];

		data->m_atlas_texture[i] = al_load_bitmap(texture_filename);
		if (!data->m_atlas_texture[i])
		{
			ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to load texture: %s", texture_filename);
			return -1;
		}
		if (apply_mask)
		{
			al_convert_mask_to_alpha(data->m_atlas_texture[i], al_map_rgb(0xff, 0x00, 0xff));
		}
	}

	for (int32_t i = 0; i < MAHJONG_SHADER_COUNT; ++i)
	{
		const char* shader_prefix = MAHJONG_SHADER_PREFIXES[i];
		char vertex_filename[256] = { 0 };
		char pixel_filename[256] = { 0 };
		snprintf(vertex_filename, sizeof(vertex_filename), MAHJONG_SHADER_VERTEX_FILENAME, shader_prefix);
		snprintf(pixel_filename, sizeof(pixel_filename), MAHJONG_SHADER_PIXEL_FILENAME, shader_prefix);
		data->m_atlas_shader[i] = al_create_shader(ALLEGRO_SHADER_GLSL);
		if (!data->m_atlas_shader[i])
		{
			ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to create shader: %s", shader_prefix);
			return -1;
		}
		if (!al_attach_shader_source_file(data->m_atlas_shader[i], ALLEGRO_VERTEX_SHADER, vertex_filename))
		{
			ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to attach vertex shader source file: %s", vertex_filename);
			return -1;
		}
		if (!al_attach_shader_source_file(data->m_atlas_shader[i], ALLEGRO_PIXEL_SHADER, pixel_filename))
		{
			ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to attach pixel shader source file: %s", pixel_filename);
			return -1;
		}
		if (!al_build_shader(data->m_atlas_shader[i]))
		{
			const char* log = al_get_shader_log(data->m_atlas_shader[i]);
			if (log)
			{
				ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to build shader: %s\n%s", shader_prefix, log);
			}
			else
			{
				ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to build shader: %s", shader_prefix);
			}
			return -1;
		}
	}

	for (int32_t i = 0; i < MAHJONG_SAMPLE_COUNT; ++i)
	{
		const char* sample_filename = MAHJONG_SAMPLE_FILENAMES[i];
		data->m_atlas_sample[i] = al_load_sample(sample_filename);
		if (!data->m_atlas_sample[i])
		{
			ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to load sample: %s", sample_filename);
			return -1;
		}
	}

	mah_texture_data_t texture_data =
	{
		.m_texture = data->m_atlas_texture[MAHJONG_TEXTURE_TILES],
		.m_glyph_width = 32,
		.m_glyph_height = 44
	};

	if (mah_atlas_model_generate(data->m_tile_view, data->m_atlas_model, &texture_data, mah_layout_get()) != 0)
	{
		ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to generate atlas models");
		return -1;
	}

	mah_board_shuffle(data->m_atlas_model, &data->m_board, &texture_data);

	return 0;
}

int32_t mah_data_render_initialize(mah_data_render_t* data, ALLEGRO_DISPLAY* display)
{
	if (!data || !display)
	{
		return -1;
	}
	
	mah_data_render_zero(data);
	
	if (_mah_data_render_initialize(data, display) != 0)
	{
		mah_data_render_uninitialize(data);
		return -1;
	}

	return 0;
}

void mah_data_render_uninitialize(mah_data_render_t* data)
{
	if (!data)
	{
		return;
	}

	for (int32_t i = 0; i < MAHJONG_FONT_COUNT; ++i)
	{
		if (data->m_atlas_font[i])
		{
			al_destroy_font(data->m_atlas_font[i]);
			data->m_atlas_font[i] = NULL;
		}
	}

	for (int32_t i = 0; i < MAHJONG_TEXTURE_COUNT; ++i)
	{
		if (data->m_atlas_texture[i])
		{
			al_destroy_bitmap(data->m_atlas_texture[i]);
			data->m_atlas_texture[i] = NULL;
		}
	}

	for (int32_t i = 0; i < MAHJONG_MODEL_COUNT; ++i)
	{
		if (data->m_atlas_model[i])
		{
			mod_model_destroy(data->m_atlas_model[i]);
			data->m_atlas_model[i] = NULL;
		}
	}

	for (int32_t i = 0; i < MAHJONG_SHADER_COUNT; ++i)
	{
		if (data->m_atlas_shader[i])
		{
			al_destroy_shader(data->m_atlas_shader[i]);
			data->m_atlas_shader[i] = NULL;
		}
	}

	for (int32_t i = 0; i < MAHJONG_SAMPLE_COUNT; ++i)
	{
		if (data->m_atlas_sample[i])
		{
			al_destroy_sample(data->m_atlas_sample[i]);
			data->m_atlas_sample[i] = NULL;
		}
	}

	if (data->m_shadow_map)
	{
		al_destroy_bitmap(data->m_shadow_map);
		data->m_shadow_map = NULL;
	}

	if (data->m_vertex_decl)
	{
		ogle_vertex_decl_destroy(data->m_vertex_decl);
		data->m_vertex_decl = NULL;
	}
}
