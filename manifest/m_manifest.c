/*
 * File "m_manifest.c" generated from compiled manifest
. * 2026-09-22 19:06:30
 * Do not modify this file.
 */

#include "m_manifest.h"

const char* MANIFEST_ERROR_STRING_FAILURE = "Failed to %s %s.";
const char* MANIFEST_ERROR_STRING_NULL_POINTER = "NULL pointer error: \"%s\".";

int32_t m_manifest_install_allegro(void)
{
	if (!al_init())
	{
		fprintf(stderr, MANIFEST_ERROR_STRING_FAILURE, "initialize", "Allegro library");
		return -1;
	}

	if (!al_install_keyboard())
	{
		fprintf(stderr, MANIFEST_ERROR_STRING_FAILURE, "install", "keyboard");
		return -1;
	}

	if (!al_install_mouse())
	{
		fprintf(stderr, MANIFEST_ERROR_STRING_FAILURE, "install", "mouse");
		return -1;
	}

	if (!al_install_audio())
	{
		fprintf(stderr, MANIFEST_ERROR_STRING_FAILURE, "install", "audio");
		return -1;
	}

	if (!al_init_acodec_addon())
	{
		fprintf(stderr, MANIFEST_ERROR_STRING_FAILURE, "initialize", "acodec");
		return -1;
	}

	if (!al_init_font_addon())
	{
		fprintf(stderr, MANIFEST_ERROR_STRING_FAILURE, "initialize", "font");
		return -1;
	}

	if (!al_init_image_addon())
	{
		fprintf(stderr, MANIFEST_ERROR_STRING_FAILURE, "initialize", "image");
		return -1;
	}

	if (!al_init_primitives_addon())
	{
		fprintf(stderr, MANIFEST_ERROR_STRING_FAILURE, "initialize", "primitives");
		return -1;
	}

	if (!al_init_ttf_addon())
	{
		fprintf(stderr, MANIFEST_ERROR_STRING_FAILURE, "initialize", "ttf");
		return -1;
	}

	return 0;
}

void m_manifest_uninstall_allegro(void)
{
	if (al_is_audio_installed())
	{
		al_uninstall_audio();
	}

	if (al_is_font_addon_initialized())
	{
		al_shutdown_font_addon();
	}

	if (al_is_image_addon_initialized())
	{
		al_shutdown_image_addon();
	}

	if (al_is_primitives_addon_initialized())
	{
		al_shutdown_primitives_addon();
	}

	if (al_is_ttf_addon_initialized())
	{
		al_shutdown_ttf_addon();
	}

	if (al_is_keyboard_installed())
	{
		al_uninstall_keyboard();
	}

	if (al_is_mouse_installed())
	{
		al_uninstall_mouse();
	}

	if (al_is_system_installed())
	{
		al_uninstall_system();
	}
}

void m_manifest_zero_data(m_manifest_data_t* data)
{
	if (NULL == data)
	{
		fprintf(stderr, MANIFEST_ERROR_STRING_NULL_POINTER, "data");
		return;
	}

	data->m_display = NULL;
	data->m_timer = NULL;
	data->m_event_queue = NULL;
}

int32_t m_manifest_initialize_data(m_manifest_data_t* data)
{
	if (NULL == data)
	{
		fprintf(stderr, MANIFEST_ERROR_STRING_NULL_POINTER, "data");
		return -1;
	}

	al_set_new_display_flags(ALLEGRO_WINDOWED|ALLEGRO_OPENGL|ALLEGRO_RESIZABLE|ALLEGRO_PROGRAMMABLE_PIPELINE);
	al_set_new_window_title(MANIFEST_TITLE);
	al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 16, ALLEGRO_SUGGEST);
	data->m_display = al_create_display(MANIFEST_DISPLAY_WIDTH, MANIFEST_DISPLAY_HEIGHT);
	if (NULL == data->m_display)
	{
		fprintf(stderr, MANIFEST_ERROR_STRING_FAILURE, "create", "display");
		return -1;
	}

	data->m_timer = al_create_timer(1.0/MANIFEST_TIMER_SPEED);
	if (NULL == data->m_timer)
	{
		fprintf(stderr, MANIFEST_ERROR_STRING_FAILURE, "create", "timer");
		return -1;
	}

	data->m_event_queue = al_create_event_queue();
	if (NULL == data->m_event_queue)
	{
		fprintf(stderr, MANIFEST_ERROR_STRING_FAILURE, "create", "event queue");
		return -1;
	}

	al_register_event_source(data->m_event_queue, al_get_display_event_source(data->m_display));
	al_register_event_source(data->m_event_queue, al_get_timer_event_source(data->m_timer));
	al_register_event_source(data->m_event_queue, al_get_keyboard_event_source());
	al_register_event_source(data->m_event_queue, al_get_mouse_event_source());

	return 0;
}

void m_manifest_uninitialize_data(m_manifest_data_t* data)
{
	if (NULL == data)
	{
		fprintf(stderr, MANIFEST_ERROR_STRING_NULL_POINTER, "data");
		return;
	}

	if (NULL != data->m_event_queue)
	{
		al_destroy_event_queue(data->m_event_queue);
		data->m_event_queue = NULL;
	}

	if (NULL != data->m_timer)
	{
		al_destroy_timer(data->m_timer);
		data->m_timer = NULL;
	}

	if (NULL != data->m_display)
	{
		al_destroy_display(data->m_display);
		data->m_display = NULL;
	}
}
