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

#ifndef __MATEKBD_INDICATOR_CONFIG_H__
#define __MATEKBD_INDICATOR_CONFIG_H__

#include <gtk/gtk.h>

#include "libmatekbd/matekbd-keyboard-config.h"

/*
 * Indicator configuration
 */
typedef struct _MatekbdIndicatorConfig MatekbdIndicatorConfig;
struct _MatekbdIndicatorConfig {
	int secondary_groups_mask;
	gboolean show_flags;

	gchar *font_family;
	gchar *foreground_color;
	gchar *background_color;

	/* private, transient */
	GSettings *settings;
	GSList *image_filenames;
	GtkIconTheme *icon_theme;
	int config_listener_id;
	XklEngine *engine;
};

/*
 * MatekbdIndicatorConfig functions -
 * some of them require MatekbdKeyboardConfig as well -
 * for loading approptiate images
 */
extern void matekbd_indicator_config_init (MatekbdIndicatorConfig *
					applet_config,
					XklEngine * engine);
extern void matekbd_indicator_config_term (MatekbdIndicatorConfig *
					applet_config);

extern void matekbd_indicator_config_load_from_gsettings (MatekbdIndicatorConfig
						   * applet_config);
extern void matekbd_indicator_config_save_to_gsettings (MatekbdIndicatorConfig *
						 applet_config);

extern void matekbd_indicator_config_refresh_style (MatekbdIndicatorConfig *
						 applet_config);

extern gchar
    * matekbd_indicator_config_get_images_file (MatekbdIndicatorConfig *
					     applet_config,
					     MatekbdKeyboardConfig *
					     kbd_config, int group);

extern void matekbd_indicator_config_load_image_filenames (MatekbdIndicatorConfig
							* applet_config,
							MatekbdKeyboardConfig
							* kbd_config);
extern void matekbd_indicator_config_free_image_filenames (MatekbdIndicatorConfig
							* applet_config);

/* Should be updated on Indicator/GSettings configuration change */
extern void matekbd_indicator_config_activate (MatekbdIndicatorConfig *
					    applet_config);

extern void matekbd_indicator_config_start_listen (MatekbdIndicatorConfig *
						applet_config,
						GCallback
						func, gpointer user_data);

extern void matekbd_indicator_config_stop_listen (MatekbdIndicatorConfig *
					       applet_config);

#endif
