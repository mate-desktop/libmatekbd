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

#ifndef __MATEKBD_DESKTOP_CONFIG_H__
#define __MATEKBD_DESKTOP_CONFIG_H__

#include <X11/Xlib.h>
#include <glib.h>
#include <gio/gio.h>
#include <libxklavier/xklavier.h>

extern const gchar MATEKBD_DESKTOP_CONFIG_KEY_DEFAULT_GROUP[];
extern const gchar MATEKBD_DESKTOP_CONFIG_KEY_GROUP_PER_WINDOW[];
extern const gchar MATEKBD_DESKTOP_CONFIG_KEY_HANDLE_INDICATORS[];
extern const gchar MATEKBD_DESKTOP_CONFIG_KEY_LAYOUT_NAMES_AS_GROUP_NAMES[];

/*
 * General configuration
 */
typedef struct _MatekbdDesktopConfig MatekbdDesktopConfig;
struct _MatekbdDesktopConfig {
	gint default_group;
	gboolean group_per_app;
	gboolean handle_indicators;
	gboolean layout_names_as_group_names;
	gboolean load_extra_items;

	/* private, transient */
	GSettings *settings;
	gulong config_listener_id;
	XklEngine *engine;
};

/*
 * MatekbdDesktopConfig functions
 */
extern void matekbd_desktop_config_init (MatekbdDesktopConfig * config,
				      XklEngine * engine);
extern void matekbd_desktop_config_term (MatekbdDesktopConfig * config);

extern void matekbd_desktop_config_load_from_gsettings (MatekbdDesktopConfig *
						 config);

extern void matekbd_desktop_config_save_to_gsettings (MatekbdDesktopConfig * config);

extern gboolean matekbd_desktop_config_activate (MatekbdDesktopConfig * config);

extern gboolean
matekbd_desktop_config_load_group_descriptions (MatekbdDesktopConfig
					     * config,
					     XklConfigRegistry *
					     registry,
					     const gchar **
					     layout_ids,
					     const gchar **
					     variant_ids,
					     gchar ***
					     short_group_names,
					     gchar *** full_group_names);

extern void matekbd_desktop_config_lock_next_group (MatekbdDesktopConfig *
						 config);

extern void matekbd_desktop_config_lock_prev_group (MatekbdDesktopConfig *
						 config);

extern void matekbd_desktop_config_restore_group (MatekbdDesktopConfig * config);

extern void matekbd_desktop_config_start_listen (MatekbdDesktopConfig * config,
					      GCallback func,
					      gpointer user_data);

extern void matekbd_desktop_config_stop_listen (MatekbdDesktopConfig * config);

#endif
