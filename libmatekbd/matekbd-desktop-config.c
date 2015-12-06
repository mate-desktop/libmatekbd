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

#include <glib/gi18n-lib.h>
#include <gio/gio.h>
#include <matekbd-desktop-config.h>
#include <matekbd-config-private.h>

/**
 * MatekbdDesktopConfig:
 */
#define MATEKBD_DESKTOP_CONFIG_SCHEMA  MATEKBD_CONFIG_SCHEMA ".general"

const gchar MATEKBD_DESKTOP_CONFIG_KEY_DEFAULT_GROUP[] = "default-group";
const gchar MATEKBD_DESKTOP_CONFIG_KEY_GROUP_PER_WINDOW[] = "group-per-window";
const gchar MATEKBD_DESKTOP_CONFIG_KEY_HANDLE_INDICATORS[] = "handle-indicators";
const gchar MATEKBD_DESKTOP_CONFIG_KEY_LAYOUT_NAMES_AS_GROUP_NAMES[] = "layout-names-as-group-names";
const gchar MATEKBD_DESKTOP_CONFIG_KEY_LOAD_EXTRA_ITEMS[] = "load-extra-items";

/*
 * static common functions
 */

static gboolean
    matekbd_desktop_config_get_lv_descriptions
    (MatekbdDesktopConfig * config,
     XklConfigRegistry * registry,
     const gchar ** layout_ids,
     const gchar ** variant_ids,
     gchar *** short_layout_descriptions,
     gchar *** long_layout_descriptions,
     gchar *** short_variant_descriptions,
     gchar *** long_variant_descriptions) {
	const gchar **pl, **pv;
	guint total_layouts;
	gchar **sld, **lld, **svd, **lvd;
	XklConfigItem *item = xkl_config_item_new ();

	if (!
	    (xkl_engine_get_features (config->engine) &
	     XKLF_MULTIPLE_LAYOUTS_SUPPORTED))
		return FALSE;

	pl = layout_ids;
	pv = variant_ids;
	total_layouts = g_strv_length ((char **) layout_ids);
	sld = *short_layout_descriptions =
	    g_new0 (gchar *, total_layouts + 1);
	lld = *long_layout_descriptions =
	    g_new0 (gchar *, total_layouts + 1);
	svd = *short_variant_descriptions =
	    g_new0 (gchar *, total_layouts + 1);
	lvd = *long_variant_descriptions =
	    g_new0 (gchar *, total_layouts + 1);

	while (pl != NULL && *pl != NULL) {

		xkl_debug (100, "ids: [%s][%s]\n", *pl,
			   pv == NULL ? NULL : *pv);

		g_snprintf (item->name, sizeof item->name, "%s", *pl);
		if (xkl_config_registry_find_layout (registry, item)) {
			*sld = g_strdup (item->short_description);
			*lld = g_strdup (item->description);
		} else {
			*sld = g_strdup ("");
			*lld = g_strdup ("");
		}

		if (pv != NULL && *pv != NULL) {
			g_snprintf (item->name, sizeof item->name, "%s",
				    *pv);
			if (xkl_config_registry_find_variant
			    (registry, *pl, item)) {
				*svd = g_strdup (item->short_description);
				*lvd = g_strdup (item->description);
			} else {
				*svd = g_strdup ("");
				*lvd = g_strdup ("");
			}
		} else {
			*svd = g_strdup ("");
			*lvd = g_strdup ("");
		}

		xkl_debug (100, "description: [%s][%s][%s][%s]\n",
			   *sld, *lld, *svd, *lvd);
		sld++;
		lld++;
		svd++;
		lvd++;

		pl++;

		if (pv != NULL && *pv != NULL)
			pv++;
	}

	g_object_unref (item);
	return TRUE;
}

/*
 * extern MatekbdDesktopConfig config functions
 */
void
matekbd_desktop_config_init (MatekbdDesktopConfig * config,
			  XklEngine * engine)
{
	memset (config, 0, sizeof (*config));
	config->settings = g_settings_new (MATEKBD_DESKTOP_CONFIG_SCHEMA);
	config->engine = engine;
}

void
matekbd_desktop_config_term (MatekbdDesktopConfig * config)
{
	g_object_unref (config->settings);
	config->settings = NULL;
}

void
matekbd_desktop_config_load_from_gsettings (MatekbdDesktopConfig * config)
{
	config->group_per_app =
	    g_settings_get_boolean (config->settings,
				 MATEKBD_DESKTOP_CONFIG_KEY_GROUP_PER_WINDOW);
	xkl_debug (150, "group_per_app: %d\n", config->group_per_app);

	config->handle_indicators =
	    g_settings_get_boolean (config->settings,
				 MATEKBD_DESKTOP_CONFIG_KEY_HANDLE_INDICATORS);
	xkl_debug (150, "handle_indicators: %d\n",
		   config->handle_indicators);

	config->layout_names_as_group_names =
	    g_settings_get_boolean (config->settings,
				   MATEKBD_DESKTOP_CONFIG_KEY_LAYOUT_NAMES_AS_GROUP_NAMES);
	xkl_debug (150, "layout_names_as_group_names: %d\n",
		   config->layout_names_as_group_names);

	config->load_extra_items =
	    g_settings_get_boolean (config->settings,
				   MATEKBD_DESKTOP_CONFIG_KEY_LOAD_EXTRA_ITEMS);
	xkl_debug (150, "load_extra_items: %d\n",
		   config->load_extra_items);

	config->default_group =
	    g_settings_get_int (config->settings,
				  MATEKBD_DESKTOP_CONFIG_KEY_DEFAULT_GROUP);

	if (config->default_group < -1
	    || config->default_group >=
	    xkl_engine_get_max_num_groups (config->engine))
		config->default_group = -1;
	xkl_debug (150, "default_group: %d\n", config->default_group);
}

void
matekbd_desktop_config_save_to_gsettings (MatekbdDesktopConfig * config)
{
	g_settings_delay (config->settings);

	g_settings_set_boolean (config->settings,
			     MATEKBD_DESKTOP_CONFIG_KEY_GROUP_PER_WINDOW,
			     config->group_per_app);
	g_settings_set_boolean (config->settings,
			     MATEKBD_DESKTOP_CONFIG_KEY_HANDLE_INDICATORS,
			     config->handle_indicators);
	g_settings_set_boolean (config->settings,
			     MATEKBD_DESKTOP_CONFIG_KEY_LAYOUT_NAMES_AS_GROUP_NAMES,
			     config->layout_names_as_group_names);
	g_settings_set_boolean (config->settings,
			     MATEKBD_DESKTOP_CONFIG_KEY_LOAD_EXTRA_ITEMS,
			     config->load_extra_items);
	g_settings_set_int (config->settings,
			    MATEKBD_DESKTOP_CONFIG_KEY_DEFAULT_GROUP,
			    config->default_group);

	g_settings_apply (config->settings);
}

gboolean
matekbd_desktop_config_activate (MatekbdDesktopConfig * config)
{
	gboolean rv = TRUE;

	xkl_engine_set_group_per_toplevel_window (config->engine,
						  config->group_per_app);
	xkl_engine_set_indicators_handling (config->engine,
					    config->handle_indicators);
	xkl_engine_set_default_group (config->engine,
				      config->default_group);

	return rv;
}

void
matekbd_desktop_config_lock_next_group (MatekbdDesktopConfig * config)
{
	int group = xkl_engine_get_next_group (config->engine);
	xkl_engine_lock_group (config->engine, group);
}

void
matekbd_desktop_config_lock_prev_group (MatekbdDesktopConfig * config)
{
	int group = xkl_engine_get_prev_group (config->engine);
	xkl_engine_lock_group (config->engine, group);
}

void
matekbd_desktop_config_restore_group (MatekbdDesktopConfig * config)
{
	int group = xkl_engine_get_current_window_group (config->engine);
	xkl_engine_lock_group (config->engine, group);
}

/**
 * matekbd_desktop_config_start_listen:
 * @func: (scope notified): a function to call when settings are changed
 */
void
matekbd_desktop_config_start_listen (MatekbdDesktopConfig * config,
				  GCallback func,
				  gpointer user_data)
{
	config->config_listener_id =
	    g_signal_connect (config->settings, "changed", func,
			      user_data);
}

void
matekbd_desktop_config_stop_listen (MatekbdDesktopConfig * config)
{
	g_signal_handler_disconnect (config->settings,
				    config->config_listener_id);
	config->config_listener_id = 0;
}

gboolean
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
					     gchar *** full_group_names)
{
	gchar **sld, **lld, **svd, **lvd;
	gchar **psld, **plld, **plvd;
	gchar **psgn, **pfgn, **psvd;
	gint total_descriptions;

	if (!matekbd_desktop_config_get_lv_descriptions
	    (config, registry, layout_ids, variant_ids, &sld, &lld, &svd,
	     &lvd)) {
		return False;
	}

	total_descriptions = g_strv_length (sld);

	*short_group_names = psgn =
	    g_new0 (gchar *, total_descriptions + 1);
	*full_group_names = pfgn =
	    g_new0 (gchar *, total_descriptions + 1);

	plld = lld;
	psld = sld;
	plvd = lvd;
	psvd = svd;
	while (plld != NULL && *plld != NULL) {
		gchar *sd = (*psvd[0] == '\0') ? *psld : *psvd;
		psld++, psvd++;
		*psgn++ = g_strdup (sd);
		*pfgn++ = g_strdup (matekbd_keyboard_config_format_full_layout
				    (*plld++, *plvd++));
	}
	g_strfreev (sld);
	g_strfreev (lld);
	g_strfreev (svd);
	g_strfreev (lvd);

	return True;
}
