/*
 * Copyright (C) 2006 Sergey V. Udaltsov <svu@gnome.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <config.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <X11/keysym.h>

#include <pango/pango.h>

#include <glib/gi18n-lib.h>
#include <gdk/gdkx.h>

#include <matekbd-keyboard-config.h>
#include <matekbd-indicator-config.h>

#include <matekbd-config-private.h>

/**
 * MatekbdIndicatorConfig:
 */
#define MATEKBD_INDICATOR_CONFIG_SCHEMA  MATEKBD_CONFIG_SCHEMA ".indicator"

const gchar MATEKBD_INDICATOR_CONFIG_KEY_SHOW_FLAGS[] = "show-flags";
const gchar MATEKBD_INDICATOR_CONFIG_KEY_SECONDARIES[] = "secondary";
const gchar MATEKBD_INDICATOR_CONFIG_KEY_FONT_FAMILY[] = "font-family";
const gchar MATEKBD_INDICATOR_CONFIG_KEY_FOREGROUND_COLOR[] = "foreground-color";
const gchar MATEKBD_INDICATOR_CONFIG_KEY_BACKGROUND_COLOR[] = "background-color";

#define SYSTEM_FONT_SCHEMA "org.mate.interface"
#define SYSTEM_FONT_KEY "font-name"

/*
 * static applet config functions
 */
static void
matekbd_indicator_config_load_font (MatekbdIndicatorConfig * ind_config)
{
	ind_config->font_family =
	    g_settings_get_string (ind_config->settings,
				   MATEKBD_INDICATOR_CONFIG_KEY_FONT_FAMILY);

	if (ind_config->font_family == NULL ||
	    ind_config->font_family[0] == '\0') {
		PangoFontDescription *fd = NULL;
		GtkWidgetPath *widget_path = gtk_widget_path_new ();
		GtkStyleContext *context = gtk_style_context_new ();

		gtk_widget_path_append_type (widget_path, GTK_TYPE_WINDOW);
		gtk_widget_path_iter_set_name (widget_path, -1 , "PanelWidget");

		gtk_style_context_set_path (context, widget_path);
		gtk_style_context_set_screen (context, gdk_screen_get_default ());
		gtk_style_context_set_state (context, GTK_STATE_FLAG_NORMAL);
		gtk_style_context_add_class (context, GTK_STYLE_CLASS_DEFAULT);
		gtk_style_context_add_class (context, "gnome-panel-menu-bar");
		gtk_style_context_add_class (context, "mate-panel-menu-bar");

		gtk_style_context_get (context, GTK_STATE_FLAG_NORMAL,
		                       GTK_STYLE_PROPERTY_FONT, &fd, NULL);

		if (fd != NULL) {
			ind_config->font_family =
			    g_strdup (pango_font_description_to_string(fd));
		}

		g_object_unref (G_OBJECT (context));
		gtk_widget_path_unref (widget_path);
	}
	xkl_debug (150, "font: [%s]\n", ind_config->font_family);

}

static void
matekbd_indicator_config_load_colors (MatekbdIndicatorConfig * ind_config)
{
	ind_config->foreground_color =
	    g_settings_get_string (ind_config->settings,
	                           MATEKBD_INDICATOR_CONFIG_KEY_FOREGROUND_COLOR);

	if (ind_config->foreground_color == NULL ||
	    ind_config->foreground_color[0] == '\0') {
		GtkWidgetPath *widget_path = gtk_widget_path_new ();
		GtkStyleContext *context = gtk_style_context_new ();
		GdkRGBA fg_color;

		gtk_widget_path_append_type (widget_path, GTK_TYPE_WINDOW);
		gtk_widget_path_iter_set_name (widget_path, -1 , "PanelWidget");

		gtk_style_context_set_path (context, widget_path);
		gtk_style_context_set_screen (context, gdk_screen_get_default ());
		gtk_style_context_set_state (context, GTK_STATE_FLAG_NORMAL);
		gtk_style_context_add_class (context, GTK_STYLE_CLASS_DEFAULT);
		gtk_style_context_add_class (context, "gnome-panel-menu-bar");
		gtk_style_context_add_class (context, "mate-panel-menu-bar");

		gtk_style_context_get_color (context,
		                             GTK_STATE_FLAG_NORMAL, &fg_color);
		ind_config->foreground_color =
		    g_strdup_printf ("%g %g %g",
		                     fg_color.red,
		                     fg_color.green,
		                     fg_color.blue);

		g_object_unref (G_OBJECT (context));
		gtk_widget_path_unref (widget_path);
	}

	ind_config->background_color =
	    g_settings_get_string (ind_config->settings,
				     MATEKBD_INDICATOR_CONFIG_KEY_BACKGROUND_COLOR);
}

void
matekbd_indicator_config_refresh_style (MatekbdIndicatorConfig * ind_config)
{
	g_free (ind_config->font_family);
	g_free (ind_config->foreground_color);
	g_free (ind_config->background_color);
	matekbd_indicator_config_load_font (ind_config);
	matekbd_indicator_config_load_colors (ind_config);
}

gchar *
matekbd_indicator_config_get_images_file (MatekbdIndicatorConfig *
				       ind_config,
				       MatekbdKeyboardConfig *
				       kbd_config, int group)
{
	char *image_file = NULL;
	GtkIconInfo *icon_info = NULL;

	if (!ind_config->show_flags)
		return NULL;

	if ((kbd_config->layouts_variants != NULL) &&
	    (g_strv_length (kbd_config->layouts_variants) > group)) {
		char *full_layout_name =
		    kbd_config->layouts_variants[group];

		if (full_layout_name != NULL) {
			char *l, *v;
			matekbd_keyboard_config_split_items (full_layout_name,
							  &l, &v);
			if (l != NULL) {
				/* probably there is something in theme? */
				icon_info = gtk_icon_theme_lookup_icon
				    (ind_config->icon_theme, l, 48, 0);

				/* Unbelievable but happens */
				if (icon_info != NULL &&
				    gtk_icon_info_get_filename (icon_info) == NULL) {
					g_object_unref (icon_info);
					icon_info = NULL;
				}
			}
		}
	}
	/* fallback to the default value */
	if (icon_info == NULL) {
		icon_info = gtk_icon_theme_lookup_icon
		    (ind_config->icon_theme, "stock_dialog-error", 48, 0);
	}
	if (icon_info != NULL) {
		image_file =
		    g_strdup (gtk_icon_info_get_filename (icon_info));
		g_object_unref (icon_info);
	}

	return image_file;
}

void
matekbd_indicator_config_load_image_filenames (MatekbdIndicatorConfig *
					    ind_config,
					    MatekbdKeyboardConfig *
					    kbd_config)
{
	int i;
	ind_config->image_filenames = NULL;

	if (!ind_config->show_flags)
		return;

	for (i = xkl_engine_get_max_num_groups (ind_config->engine);
	     --i >= 0;) {
		gchar *image_file =
		    matekbd_indicator_config_get_images_file (ind_config,
							   kbd_config,
							   i);
		ind_config->image_filenames =
		    g_slist_prepend (ind_config->image_filenames,
				     image_file);
	}
}

void
matekbd_indicator_config_free_image_filenames (MatekbdIndicatorConfig *
					    ind_config)
{
	while (ind_config->image_filenames) {
		if (ind_config->image_filenames->data)
			g_free (ind_config->image_filenames->data);
		ind_config->image_filenames =
		    g_slist_delete_link (ind_config->image_filenames,
					 ind_config->image_filenames);
	}
}

void
matekbd_indicator_config_init (MatekbdIndicatorConfig * ind_config,
			       XklEngine * engine)
{
	gchar *sp;

	memset (ind_config, 0, sizeof (*ind_config));
	ind_config->settings = g_settings_new (MATEKBD_INDICATOR_CONFIG_SCHEMA);
	ind_config->engine = engine;

	ind_config->icon_theme = gtk_icon_theme_get_default ();

	gtk_icon_theme_append_search_path (ind_config->icon_theme, sp =
					   g_build_filename (g_get_home_dir
							     (),
							     ".icons/flags",
							     NULL));
	g_free (sp);

	gtk_icon_theme_append_search_path (ind_config->icon_theme,
					   sp =
					   g_build_filename (DATADIR,
							     "pixmaps/flags",
							     NULL));
	g_free (sp);

	gtk_icon_theme_append_search_path (ind_config->icon_theme,
					   sp =
					   g_build_filename (DATADIR,
							     "icons/flags",
							     NULL));
	g_free (sp);
}

void
matekbd_indicator_config_term (MatekbdIndicatorConfig * ind_config)
{
	g_free (ind_config->font_family);
	ind_config->font_family = NULL;

	g_free (ind_config->foreground_color);
	ind_config->foreground_color = NULL;

	g_free (ind_config->background_color);
	ind_config->background_color = NULL;

	ind_config->icon_theme = NULL;

	matekbd_indicator_config_free_image_filenames (ind_config);

	g_object_unref (ind_config->settings);
	ind_config->settings = NULL;
}

void
matekbd_indicator_config_load_from_gsettings (MatekbdIndicatorConfig * ind_config)
{
	ind_config->secondary_groups_mask =
	    g_settings_get_int (ind_config->settings,
				MATEKBD_INDICATOR_CONFIG_KEY_SECONDARIES);

	ind_config->show_flags =
	    g_settings_get_boolean (ind_config->settings,
				 MATEKBD_INDICATOR_CONFIG_KEY_SHOW_FLAGS);

	matekbd_indicator_config_load_font (ind_config);
	matekbd_indicator_config_load_colors (ind_config);

}

void
matekbd_indicator_config_save_to_gsettings (MatekbdIndicatorConfig * ind_config)
{
	g_settings_delay (ind_config->settings);

	g_settings_set_int (ind_config->settings,
				  MATEKBD_INDICATOR_CONFIG_KEY_SECONDARIES,
				  ind_config->secondary_groups_mask);
	g_settings_set_boolean (ind_config->settings,
				   MATEKBD_INDICATOR_CONFIG_KEY_SHOW_FLAGS,
				   ind_config->show_flags);

	g_settings_apply (ind_config->settings);
}

void
matekbd_indicator_config_activate (MatekbdIndicatorConfig * ind_config)
{
	xkl_engine_set_secondary_groups_mask (ind_config->engine,
					      ind_config->secondary_groups_mask);
}

/**
 * matekbd_indicator_config_start_listen:
 * @func: (scope notified): a function to call when settings are changed
 */
void
matekbd_indicator_config_start_listen (MatekbdIndicatorConfig *
				    ind_config,
				    GCallback func,
				    gpointer user_data)
{
	ind_config->config_listener_id =
	    g_signal_connect (ind_config->settings, "changed", func,
			      user_data);
}

void
matekbd_indicator_config_stop_listen (MatekbdIndicatorConfig * ind_config)
{
	g_signal_handler_disconnect (ind_config->settings,
				     ind_config->config_listener_id);
	ind_config->config_listener_id = 0;
}
