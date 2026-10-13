#include "m_common.h"
#include "m_manifest.h"

static int32_t mah_main_initialize(mah_data_main_t* data)
{
	if (!data)
	{
		return -1;
	}

	if (m_manifest_install_allegro() != 0)
	{
		return -1;
	}

#ifdef _DEBUG
	ogle_log_open("output_log.txt");
#endif

	al_reserve_samples(8);

	if (m_manifest_initialize_data((m_manifest_data_t*)data) != 0)
	{
		return -1;
	}

	ALLEGRO_BITMAP* icon = al_load_bitmap("assets/images/icon.png");
	if (icon)
	{
		al_convert_mask_to_alpha(icon, al_map_rgb(0xff, 0x00, 0xff));
		al_set_display_icon(data->m_display, icon);
		al_destroy_bitmap(icon);
	}
	al_clear_to_color(al_map_rgb(0x16, 0x16, 0x21));
	al_flip_display();

	srand((unsigned int)time(NULL));

	data->m_input = ogle_object_create(ogle_input_size(), &ogle_input_initializer, NULL);
	if (!data->m_input)
	{
		ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to create input");
		return -1;
	}

	mah_data_render_zero(data->m_render);
	if (mah_data_render_initialize(data->m_render, data->m_display) != 0)
	{
		ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to initialize render data");
		return -1;
	}

	data->m_id = -1;
	data->m_id_last = -1;

	data->m_render->m_show_debug = false;

	return 0;
}

static void mah_main_cleanup(mah_data_main_t* data)
{
	if (!data)
	{
		return;
	}

	mah_data_render_uninitialize(data->m_render);

	if (data->m_input)
	{
		ogle_object_destroy(data->m_input, &ogle_input_uninitializer);
		data->m_input = NULL;
	}

	m_manifest_uninitialize_data((m_manifest_data_t*)data);

	m_manifest_uninstall_allegro();

	ogle_log_print("\nTotal manual memory data:\n");
	ogle_log_printf("\tAllocations: %d (%zu)\n", ogle_object_total_allocations(), ogle_object_memory_allocated());
	ogle_log_printf("\tDeallocations: %d (%zu)\n", ogle_object_total_deallocations(), ogle_object_memory_freed());
	ogle_log_printf("\tNet: %zu\n", ogle_object_memory_allocated() - ogle_object_memory_freed());
	ogle_log_printf("\nGoodbye!\n\n");
}

static void mah_main_input(mah_data_main_t* data)
{
	if (!data)
	{
		return;
	}

	ALLEGRO_EVENT event = { 0 };
	o_input_mouse_t* mouse_input = ogle_input_get_type(data->m_input, OGLE_INPUT_TYPE_MOUSE);
	o_input_keyboard_t* keyboard_input = ogle_input_get_type(data->m_input, OGLE_INPUT_TYPE_KEYBOARD);

	while (!al_is_event_queue_empty(data->m_event_queue))
	{
		al_get_next_event(data->m_event_queue, &event);

		switch (event.type)
		{
		case ALLEGRO_EVENT_TIMER:
		{
			data->m_update = true;
		} break;
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
		{
			data->m_running = false;
		} break;
		case ALLEGRO_EVENT_DISPLAY_RESIZE:
		{
			al_acknowledge_resize(data->m_display);
		} break;
		case ALLEGRO_EVENT_MOUSE_AXES:
		{
			mouse_input->m_position = (o_vector2_t){ (float)event.mouse.x, (float)event.mouse.y };
			mouse_input->m_changed = true;

			mah_main_render(data, true);
			ALLEGRO_COLOR c = al_get_pixel(al_get_target_bitmap(), (int)mouse_input->m_position.m_x, (int)mouse_input->m_position.m_y);
			data->m_id = (int32_t)(c.r * 255.0f) - 1;
		} break;
		case ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY:
		{
			data->m_draw_mouse = false;
		} break;
		case ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY:
		{
			data->m_draw_mouse = true;
		} break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
		{
			int32_t button_index = event.mouse.button - 1;

			if (button_index >= 0 && button_index < OGLE_INPUT_MOUSE_MAX_BUTTONS)
			{
				mouse_input->m_button[button_index] = OGLE_INPUT_BUTTON_FLAG_CHANGED;
				mouse_input->m_changed = true;
			}
		} break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
		{
			int32_t button_index = event.mouse.button - 1;
			
			if (button_index >= 0 && button_index < OGLE_INPUT_MOUSE_MAX_BUTTONS)
			{
				mouse_input->m_button[button_index] = (OGLE_INPUT_BUTTON_FLAG_PRESSED | OGLE_INPUT_BUTTON_FLAG_CHANGED);
				mouse_input->m_changed = true;
			}
		} break;
		case ALLEGRO_EVENT_KEY_UP:
		{
			int32_t button_index = event.keyboard.keycode;

			if (button_index >= 0 && button_index < OGLE_INPUT_KEYBOARD_MAX_BUTTONS)
			{
				keyboard_input->m_button[button_index] = OGLE_INPUT_BUTTON_FLAG_PRESSED;
				keyboard_input->m_changed = true;
			}
		} break;
		case ALLEGRO_EVENT_KEY_DOWN:
		{
			int32_t button_index = event.keyboard.keycode;

			if (button_index >= 0 && button_index < OGLE_INPUT_KEYBOARD_MAX_BUTTONS)
			{
				keyboard_input->m_button[button_index] = (OGLE_INPUT_BUTTON_FLAG_PRESSED | OGLE_INPUT_BUTTON_FLAG_CHANGED);
				keyboard_input->m_changed = true;
			}

		} break;
		default: break;
		}
	}
}

static void mah_main_loop(mah_data_main_t* data)
{
	if (!data)
	{
		return;
	}

	al_set_render_state(ALLEGRO_DEPTH_TEST, 1);

	al_start_timer(data->m_timer);

	double current_time = 0.0;
	double elapsed_time = 0;


	while (data->m_running)
	{
		current_time = al_get_time();

		mah_main_input(data);

		if (data->m_update)
		{
			mah_main_update(data);
		}

		mah_main_render(data, false);

		al_rest(0.01);

		elapsed_time = al_get_time() - current_time;

		data->m_fps = 1.0 / elapsed_time;
	}

	al_stop_timer(data->m_timer);
	al_set_render_state(ALLEGRO_DEPTH_TEST, 0);
}

int32_t main(int32_t argc, const char** argv)
{
	(void)argc;
	(void)argv;

	int32_t result = 0;

	mah_data_render_t render_data =
	{
		.m_world_transform = { 0 },
		.m_light = { 0 },
		.m_camera = { 0 },
		.m_atlas_font = { NULL },
		.m_atlas_texture = { NULL },
		.m_atlas_model = { NULL },
		.m_atlas_shader = { NULL },
		.m_atlas_sample = { NULL }
	};

	mah_data_main_t data =
	{
		.m_display = NULL,
		.m_event_queue = NULL,
		.m_timer = NULL,
		.m_update = true,
		.m_draw_stats = false,
		.m_draw_mouse = true,
		.m_fps = 0.0,
		.m_running = true,
		.m_render = &render_data
	};

	if ((result = mah_main_initialize(&data)) == 0)
	{
		mah_main_loop(&data);
	}

	mah_main_cleanup(&data);

	return result;
}

