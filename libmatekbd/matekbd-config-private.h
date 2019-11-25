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

#ifndef __MATEKBD_CONFIG_PRIVATE_H__
#define __MATEKBD_CONFIG_PRIVATE_H__

#include "libmatekbd/matekbd-desktop-config.h"
#include "libmatekbd/matekbd-keyboard-config.h"

#define MATEKBD_CONFIG_SCHEMA "org.mate.peripherals-keyboard-xkb"

extern const gchar MATEKBD_PREVIEW_CONFIG_KEY_X[];
extern const gchar MATEKBD_PREVIEW_CONFIG_KEY_Y[];
extern const gchar MATEKBD_PREVIEW_CONFIG_KEY_WIDTH[];
extern const gchar MATEKBD_PREVIEW_CONFIG_KEY_HEIGHT[];

/**
 * General config functions (private)
 */
extern void matekbd_keyboard_config_model_set (MatekbdKeyboardConfig *
					    kbd_config,
					    const gchar * model_name);

extern void matekbd_keyboard_config_options_set (MatekbdKeyboardConfig *
					      kbd_config,
					      gint idx,
					      const gchar * group_name,
					      const gchar * option_name);
extern gboolean matekbd_keyboard_config_options_is_set (MatekbdKeyboardConfig *
						     kbd_config,
						     const gchar *
						     group_name,
						     const gchar *
						     option_name);

extern gboolean matekbd_keyboard_config_dump_settings (MatekbdKeyboardConfig *
						    kbd_config,
						    const char *file_name);

extern gboolean matekbd_keyboard_config_get_lv_descriptions (XklConfigRegistry
							  *
							  config_registry,
							  const gchar *
							  layout_name,
							  const gchar *
							  variant_name,
							  gchar **
							  layout_short_descr,
							  gchar **
							  layout_descr,
							  gchar **
							  variant_short_descr,
							  gchar **
							  variant_descr);

#endif
