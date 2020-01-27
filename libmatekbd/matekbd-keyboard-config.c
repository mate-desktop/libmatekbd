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

#include <matekbd-keyboard-config.h>
#include <matekbd-config-private.h>
#include <matekbd-util.h>

/*
 * MatekbdKeyboardConfig
 */
#define MATEKBD_KEYBOARD_CONFIG_SCHEMA MATEKBD_CONFIG_SCHEMA ".kbd"

#define GROUP_SWITCHERS_GROUP "grp"
#define DEFAULT_GROUP_SWITCH "grp:shift_caps_toggle"

const gchar MATEKBD_KEYBOARD_CONFIG_KEY_MODEL[] = "model";
const gchar MATEKBD_KEYBOARD_CONFIG_KEY_LAYOUTS[] = "layouts";
const gchar MATEKBD_KEYBOARD_CONFIG_KEY_OPTIONS[] = "options";

const gchar *MATEKBD_KEYBOARD_CONFIG_ACTIVE[] = {
	MATEKBD_KEYBOARD_CONFIG_KEY_MODEL,
	MATEKBD_KEYBOARD_CONFIG_KEY_LAYOUTS,
	MATEKBD_KEYBOARD_CONFIG_KEY_OPTIONS
};

/*
 * static common functions
 */

static gboolean
matekbd_strv_equal (gchar ** l1, gchar ** l2)
{
	if (l1 == l2)
		return TRUE;
	if (l1 == NULL)
		return g_strv_length (l2) == 0;
	if (l2 == NULL)
		return g_strv_length (l1) == 0;

	while ((*l1 != NULL) && (*l2 != NULL)) {
		if (*l1 != *l2) {
			if (*l1 && *l2) {
				if (g_ascii_strcasecmp (*l1, *l2))
					return FALSE;
			} else
				return FALSE;
		}

		l1++;
		l2++;
	}
	return (*l1 == NULL) && (*l2 == NULL);
}

gboolean
matekbd_keyboard_config_get_lv_descriptions (XklConfigRegistry *
					  config_registry,
					  const gchar * layout_name,
					  const gchar * variant_name,
					  gchar ** layout_short_descr,
					  gchar ** layout_descr,
					  gchar ** variant_short_descr,
					  gchar ** variant_descr)
{
	/* TODO make it not static */
	static XklConfigItem *litem = NULL;
	static XklConfigItem *vitem = NULL;

	if (litem == NULL)
		litem = xkl_config_item_new ();
	if (vitem == NULL)
		vitem = xkl_config_item_new ();

	layout_name = g_strdup (layout_name);

	g_snprintf (litem->name, sizeof litem->name, "%s", layout_name);
	if (xkl_config_registry_find_layout (config_registry, litem)) {
		*layout_short_descr = litem->short_description;
		*layout_descr = litem->description;
	} else
		*layout_short_descr = *layout_descr = NULL;

	if (variant_name != NULL) {
		variant_name = g_strdup (variant_name);
		g_snprintf (vitem->name, sizeof vitem->name, "%s",
			    variant_name);
		if (xkl_config_registry_find_variant
		    (config_registry, layout_name, vitem)) {
			*variant_short_descr = vitem->short_description;
			*variant_descr = vitem->description;
		} else
			*variant_short_descr = *variant_descr = NULL;

		g_free ((char *) variant_name);
	} else
		*variant_descr = NULL;

	g_free ((char *) layout_name);
	return *layout_descr != NULL;
}

/*
 * extern common functions
 */
const gchar *
matekbd_keyboard_config_merge_items (const gchar * parent,
				  const gchar * child)
{
	static gchar buffer[XKL_MAX_CI_NAME_LENGTH * 2 - 1];
	*buffer = '\0';
	if (parent != NULL) {
		if (strlen (parent) >= XKL_MAX_CI_NAME_LENGTH)
			return NULL;
		strcat (buffer, parent);
	}
	if (child != NULL && *child != 0) {
		if (strlen (child) >= XKL_MAX_CI_NAME_LENGTH)
			return NULL;
		strcat (buffer, "\t");
		strcat (buffer, child);
	}
	return buffer;
}

gboolean
matekbd_keyboard_config_split_items (const gchar * merged, gchar ** parent,
				  gchar ** child)
{
	static gchar pbuffer[XKL_MAX_CI_NAME_LENGTH];
	static gchar cbuffer[XKL_MAX_CI_NAME_LENGTH];
	int plen;
	const gchar *pos;
	*parent = *child = NULL;

	if (merged == NULL)
		return FALSE;

	pos = strchr (merged, '\t');
	if (pos == NULL) {
		plen = strlen (merged);
	} else {
		plen = pos - merged;
		if (strlen (pos + 1) >= XKL_MAX_CI_NAME_LENGTH)
			return FALSE;
		strcpy (*child = cbuffer, pos + 1);
	}
	if (plen >= XKL_MAX_CI_NAME_LENGTH)
		return FALSE;
	memcpy (*parent = pbuffer, merged, plen);
	pbuffer[plen] = '\0';
	return TRUE;
}

/*
 * static MatekbdKeyboardConfig functions
 */
static void
matekbd_keyboard_config_copy_from_xkl_config (MatekbdKeyboardConfig * kbd_config,
					   XklConfigRec * pdata)
{
	char **p, **p1;
	int i;
	matekbd_keyboard_config_model_set (kbd_config, pdata->model);
	xkl_debug (150, "Loaded Kbd model: [%s]\n", pdata->model);

	/* Layouts */
	g_strfreev (kbd_config->layouts_variants);
	kbd_config->layouts_variants = NULL;
	if (pdata->layouts != NULL) {
		p = pdata->layouts;
		p1 = pdata->variants;
		kbd_config->layouts_variants =
		    g_new0 (gchar *, g_strv_length (pdata->layouts) + 1);
		i = 0;
		while (*p != NULL) {
			const gchar *full_layout =
			    matekbd_keyboard_config_merge_items (*p, *p1);
			xkl_debug (150,
				   "Loaded Kbd layout (with variant): [%s]\n",
				   full_layout);
			kbd_config->layouts_variants[i++] =
			    g_strdup (full_layout);
			p++;
			p1++;
		}
	}

	/* Options */
	g_strfreev (kbd_config->options);
	kbd_config->options = NULL;

	if (pdata->options != NULL) {
		p = pdata->options;
		kbd_config->options =
		    g_new0 (gchar *, g_strv_length (pdata->options) + 1);
		i = 0;
		while (*p != NULL) {
			char group[XKL_MAX_CI_NAME_LENGTH];
			char *option = *p;
			char *delim =
			    (option != NULL) ? strchr (option, ':') : NULL;
			int len;
			if ((delim != NULL) &&
			    ((len =
			      (delim - option)) <
			     XKL_MAX_CI_NAME_LENGTH)) {
				strncpy (group, option, len);
				group[len] = 0;
				xkl_debug (150,
					   "Loaded Kbd option: [%s][%s]\n",
					   group, option);
				matekbd_keyboard_config_options_set
				    (kbd_config, i++, group, option);
			}
			p++;
		}
	}
}

static void
matekbd_keyboard_config_copy_to_xkl_config (MatekbdKeyboardConfig * kbd_config,
					 XklConfigRec * pdata)
{
	int i;
	int num_layouts, num_options;
	pdata->model =
	    (kbd_config->model ==
	     NULL) ? NULL : g_strdup (kbd_config->model);

	num_layouts =
	    (kbd_config->layouts_variants ==
	     NULL) ? 0 : g_strv_length (kbd_config->layouts_variants);
	num_options =
	    (kbd_config->options ==
	     NULL) ? 0 : g_strv_length (kbd_config->options);

	xkl_debug (150, "Taking %d layouts\n", num_layouts);
	if (num_layouts != 0) {
		gchar **the_layout_variant = kbd_config->layouts_variants;
		char **p1 = pdata->layouts =
		    g_new0 (char *, num_layouts + 1);
		char **p2 = pdata->variants =
		    g_new0 (char *, num_layouts + 1);
		for (i = num_layouts; --i >= 0;) {
			char *layout, *variant;
			if (matekbd_keyboard_config_split_items
			    (*the_layout_variant, &layout, &variant)
			    && variant != NULL) {
				*p1 =
				    (layout ==
				     NULL) ? g_strdup ("") :
				    g_strdup (layout);
				*p2 =
				    (variant ==
				     NULL) ? g_strdup ("") :
				    g_strdup (variant);
			} else {
				*p1 =
				    (*the_layout_variant ==
				     NULL) ? g_strdup ("") :
				    g_strdup (*the_layout_variant);
				*p2 = g_strdup ("");
			}
			xkl_debug (150, "Adding [%s]/%p and [%s]/%p\n",
				   *p1 ? *p1 : "(nil)", *p1,
				   *p2 ? *p2 : "(nil)", *p2);
			p1++;
			p2++;
			the_layout_variant++;
		}
	}

	if (num_options != 0) {
		gchar **the_option = kbd_config->options;
		char **p = pdata->options =
		    g_new0 (char *, num_options + 1);
		for (i = num_options; --i >= 0;) {
			char *group, *option;
			if (matekbd_keyboard_config_split_items
			    (*the_option, &group, &option)
			    && option != NULL)
				*(p++) = g_strdup (option);
			else {
				*(p++) = g_strdup ("");
				xkl_debug (150, "Could not split [%s]\n",
					   *the_option);
			}
			the_option++;
		}
	}
}

static void
matekbd_keyboard_config_load_params (MatekbdKeyboardConfig * kbd_config,
				  const gchar * param_names[])
{
	gchar *pc;

	pc = g_settings_get_string (kbd_config->settings, param_names[0]);
	if (pc == NULL) {
		matekbd_keyboard_config_model_set (kbd_config, NULL);
	} else {
		matekbd_keyboard_config_model_set (kbd_config, pc);
		g_free (pc);
	}
	xkl_debug (150, "Loaded Kbd model: [%s]\n",
		   kbd_config->model ? kbd_config->model : "(null)");

	g_strfreev (kbd_config->layouts_variants);

	kbd_config->layouts_variants =
	    g_settings_get_strv (kbd_config->settings, param_names[1]);

	if (kbd_config->layouts_variants != NULL
	    && kbd_config->layouts_variants[0] == NULL) {
		g_strfreev (kbd_config->layouts_variants);
		kbd_config->layouts_variants = NULL;
	}

	g_strfreev (kbd_config->options);

	kbd_config->options =
	    g_settings_get_strv (kbd_config->settings, param_names[2]);

	if (kbd_config->options != NULL && kbd_config->options[0] == NULL) {
		g_strfreev (kbd_config->options);
		kbd_config->options = NULL;
	}
}

static void
matekbd_keyboard_config_save_params (MatekbdKeyboardConfig * kbd_config,
				  const gchar * param_names[])
{
	gchar **pl;

	if (kbd_config->model)
		g_settings_set_string (kbd_config->settings, param_names[0],
				       kbd_config->model);
	else
		g_settings_set_string (kbd_config->settings, param_names[0],
				       NULL);
	xkl_debug (150, "Saved Kbd model: [%s]\n",
		   kbd_config->model ? kbd_config->model : "(null)");

	if (kbd_config->layouts_variants) {
		pl = kbd_config->layouts_variants;
		while (*pl != NULL) {
			xkl_debug (150, "Saved Kbd layout: [%s]\n", *pl);
			pl++;
		}
		g_settings_set_strv (kbd_config->settings,
				     param_names[1],
				     (const gchar * const *)
				     kbd_config->layouts_variants);
	} else {
		xkl_debug (150, "Saved Kbd layouts: []\n");
		g_settings_set_strv (kbd_config->settings,
				     param_names[1], NULL);
	}

	if (kbd_config->options) {
		pl = kbd_config->options;
		while (*pl != NULL) {
			xkl_debug (150, "Saved Kbd option: [%s]\n", *pl);
			pl++;
		}
		g_settings_set_strv (kbd_config->settings,
				     param_names[2],
				     (const gchar *
				      const *) kbd_config->options);
	} else {
		xkl_debug (150, "Saved Kbd options: []\n");
		g_settings_set_strv (kbd_config->settings,
				     param_names[2], NULL);
	}
}

/*
 * extern MatekbdKeyboardConfig config functions
 */
void
matekbd_keyboard_config_init (MatekbdKeyboardConfig * kbd_config,
			      XklEngine * engine)
{
	memset (kbd_config, 0, sizeof (*kbd_config));
	kbd_config->settings = g_settings_new (MATEKBD_KEYBOARD_CONFIG_SCHEMA);
	kbd_config->engine = engine;
}

void
matekbd_keyboard_config_term (MatekbdKeyboardConfig * kbd_config)
{
	matekbd_keyboard_config_model_set (kbd_config, NULL);

	g_strfreev (kbd_config->layouts_variants);
	kbd_config->layouts_variants = NULL;
	g_strfreev (kbd_config->options);
	kbd_config->options = NULL;

	g_object_unref (kbd_config->settings);
	kbd_config->settings = NULL;
}

void
matekbd_keyboard_config_load_from_gsettings (MatekbdKeyboardConfig * kbd_config,
				      MatekbdKeyboardConfig *
				      kbd_config_default)
{
	matekbd_keyboard_config_load_params (kbd_config,
					  MATEKBD_KEYBOARD_CONFIG_ACTIVE);

	if (kbd_config_default != NULL) {

		if (kbd_config->model == NULL)
			kbd_config->model =
			    g_strdup (kbd_config_default->model);

		if (kbd_config->layouts_variants == NULL) {
			kbd_config->layouts_variants =
			    g_strdupv
			    (kbd_config_default->layouts_variants);
		}

		if (kbd_config->options == NULL) {
			kbd_config->options =
			    g_strdupv (kbd_config_default->options);
		}
	}
}

void
matekbd_keyboard_config_load_from_x_current (MatekbdKeyboardConfig * kbd_config,
					  XklConfigRec * data)
{
	gboolean own_data = data == NULL;
	xkl_debug (150, "Copying config from X(current)\n");
	if (own_data)
		data = xkl_config_rec_new ();
	if (xkl_config_rec_get_from_server (data, kbd_config->engine))
		matekbd_keyboard_config_copy_from_xkl_config (kbd_config,
							   data);
	else
		xkl_debug (150,
			   "Could not load keyboard config from server: [%s]\n",
			   xkl_get_last_error ());
	if (own_data)
		g_object_unref (G_OBJECT (data));
}

void
matekbd_keyboard_config_load_from_x_initial (MatekbdKeyboardConfig * kbd_config,
					  XklConfigRec * data)
{
	gboolean own_data = data == NULL;
	xkl_debug (150, "Copying config from X(initial)\n");
	if (own_data)
		data = xkl_config_rec_new ();
	if (xkl_config_rec_get_from_backup (data, kbd_config->engine))
		matekbd_keyboard_config_copy_from_xkl_config (kbd_config,
							   data);
	else
		xkl_debug (150,
			   "Could not load keyboard config from backup: [%s]\n",
			   xkl_get_last_error ());
	if (own_data)
		g_object_unref (G_OBJECT (data));
}

static gboolean
matekbd_keyboard_config_options_equals (MatekbdKeyboardConfig * kbd_config1,
				     MatekbdKeyboardConfig * kbd_config2)
{
	int num_options, num_options2;

	num_options =
	    (kbd_config1->options ==
	     NULL) ? 0 : g_strv_length (kbd_config1->options);
	num_options2 =
	    (kbd_config2->options ==
	     NULL) ? 0 : g_strv_length (kbd_config2->options);

	if (num_options != num_options2)
		return False;

	if (num_options != 0) {
		int i;
		char *group1, *option1;

		for (i = 0; i < num_options; i++) {
			int j;
			char *group2, *option2;
			gboolean are_equal = FALSE;

			if (!matekbd_keyboard_config_split_items
			    (kbd_config1->options[i], &group1, &option1))
				continue;

			option1 = g_strdup (option1);

			for (j = 0; j < num_options && !are_equal; j++) {
				if (matekbd_keyboard_config_split_items
				    (kbd_config2->options[j], &group2,
				     &option2)) {
					are_equal =
					    strcmp (option1, option2) == 0;
				}
			}

			g_free (option1);

			if (!are_equal)
				return False;
		}
	}

	return True;
}

gboolean
matekbd_keyboard_config_equals (MatekbdKeyboardConfig * kbd_config1,
			     MatekbdKeyboardConfig * kbd_config2)
{
	if (kbd_config1 == kbd_config2)
		return True;
	if ((kbd_config1->model != kbd_config2->model) &&
	    (kbd_config1->model != NULL) &&
	    (kbd_config2->model != NULL) &&
	    g_ascii_strcasecmp (kbd_config1->model, kbd_config2->model))
		return False;
	if (!matekbd_strv_equal (kbd_config1->layouts_variants,
			   kbd_config2->layouts_variants))
		return False;

	if (!matekbd_keyboard_config_options_equals
	    (kbd_config1, kbd_config2))
		return False;

	return True;
}

void
matekbd_keyboard_config_save_to_gsettings (MatekbdKeyboardConfig * kbd_config)
{
	g_settings_delay (kbd_config->settings);

	matekbd_keyboard_config_save_params (kbd_config,
					     MATEKBD_KEYBOARD_CONFIG_ACTIVE);

	g_settings_apply (kbd_config->settings);
}

void
matekbd_keyboard_config_model_set (MatekbdKeyboardConfig * kbd_config,
				const gchar * model_name)
{
	if (kbd_config->model != NULL)
		g_free (kbd_config->model);
	kbd_config->model =
	    (model_name == NULL
	     || model_name[0] == '\0') ? NULL : g_strdup (model_name);
}

void
matekbd_keyboard_config_options_set (MatekbdKeyboardConfig * kbd_config,
				  gint idx,
				  const gchar * group_name,
				  const gchar * option_name)
{
	const gchar *merged;
	if (group_name == NULL || option_name == NULL)
		return;
	merged =
	    matekbd_keyboard_config_merge_items (group_name, option_name);
	if (merged == NULL)
		return;
	kbd_config->options[idx] = g_strdup (merged);
}

gboolean
matekbd_keyboard_config_options_is_set (MatekbdKeyboardConfig * kbd_config,
				     const gchar * group_name,
				     const gchar * option_name)
{
	gchar **p = kbd_config->options;
	const gchar *merged =
	    matekbd_keyboard_config_merge_items (group_name, option_name);
	if (merged == NULL)
		return FALSE;

	while (p && *p) {
		if (!g_ascii_strcasecmp (merged, *p++))
			return TRUE;
	}
	return FALSE;
}

gboolean
matekbd_keyboard_config_activate (MatekbdKeyboardConfig * kbd_config)
{
	gboolean rv;
	XklConfigRec *data = xkl_config_rec_new ();

	matekbd_keyboard_config_copy_to_xkl_config (kbd_config, data);
	rv = xkl_config_rec_activate (data, kbd_config->engine);
	g_object_unref (G_OBJECT (data));

	return rv;
}

/**
 * matekbd_keyboard_config_start_listen:
 * @func: (scope notified): a function to call when settings are changed
 */
void
matekbd_keyboard_config_start_listen (MatekbdKeyboardConfig * kbd_config,
				   GCallback func,
				   gpointer user_data)
{
	kbd_config->config_listener_id =
	    g_signal_connect (kbd_config->settings, "changed", func,
			      user_data);
}

void
matekbd_keyboard_config_stop_listen (MatekbdKeyboardConfig * kbd_config)
{
	g_signal_handler_disconnect (kbd_config->settings,
				     kbd_config->config_listener_id);
	kbd_config->config_listener_id = 0;
}

gboolean
matekbd_keyboard_config_get_descriptions (XklConfigRegistry * config_registry,
				       const gchar * name,
				       gchar ** layout_short_descr,
				       gchar ** layout_descr,
				       gchar ** variant_short_descr,
				       gchar ** variant_descr)
{
	char *layout_name = NULL, *variant_name = NULL;
	if (!matekbd_keyboard_config_split_items
	    (name, &layout_name, &variant_name))
		return FALSE;
	return matekbd_keyboard_config_get_lv_descriptions (config_registry,
							 layout_name,
							 variant_name,
							 layout_short_descr,
							 layout_descr,
							 variant_short_descr,
							 variant_descr);
}

const gchar *
matekbd_keyboard_config_format_full_layout (const gchar * layout_descr,
					 const gchar * variant_descr)
{
	static gchar full_descr[XKL_MAX_CI_DESC_LENGTH * 2];
	if (variant_descr == NULL || variant_descr[0] == 0)
		g_snprintf (full_descr, sizeof (full_descr), "%s",
			    layout_descr);
	else
		g_snprintf (full_descr, sizeof (full_descr), "%s %s",
			    layout_descr, variant_descr);
	return full_descr;
}

gchar *
matekbd_keyboard_config_to_string (const MatekbdKeyboardConfig * config)
{
	gchar *layouts = NULL, *options = NULL;
	GString *buffer = g_string_new (NULL);

	gchar **iter;
	gint count;
	gchar *result;

	if (config->layouts_variants) {
		/* g_slist_length is "expensive", so we determinate the length on the fly */
		for (iter = config->layouts_variants, count = 0; *iter;
		     iter++, ++count) {
			if (buffer->len)
				g_string_append (buffer, " ");

			g_string_append (buffer, *iter);
		}

		/* Translators: The count is related to the number of options. The %s
		 * format specifier should not be modified, left "as is". */
		layouts =
		    g_strdup_printf (ngettext
				     ("layout \"%s\"", "layouts \"%s\"",
				      count), buffer->str);
		g_string_truncate (buffer, 0);
	}
	if (config->options) {
		/* g_slist_length is "expensive", so we determinate the length on the fly */
		for (iter = config->options, count = 0; *iter;
		     iter++, ++count) {
			if (buffer->len)
				g_string_append (buffer, " ");

			g_string_append (buffer, *iter);
		}

		/* Translators: The count is related to the number of options. The %s
		 * format specifier should not be modified, left "as is". */
		options =
		    g_strdup_printf (ngettext
				     ("option \"%s\"", "options \"%s\"",
				      count), buffer->str);
		g_string_truncate (buffer, 0);
	}

	g_string_free (buffer, TRUE);

	result =
	    g_strdup_printf (_("model \"%s\", %s and %s"), config->model,
			     layouts ? layouts : _("no layout"),
			     options ? options : _("no options"));

	g_free (options);
	g_free (layouts);

	return result;
}

/**
 * matekbd_keyboard_config_add_default_switch_option_if_necessary:
 *
 * Returns: (transfer full) (array zero-terminated=1): List of options
 */
gchar **
matekbd_keyboard_config_add_default_switch_option_if_necessary (gchar **
							        layouts_list,
							        gchar **
							        options_list,
							        gboolean *was_appended)
{
	*was_appended = FALSE;
	if (g_strv_length (layouts_list) >= 2) {
		gboolean any_switcher = False;
		if (*options_list != NULL) {
			gchar **option = options_list;
			while (*option != NULL) {
				char *g, *o;
				if (matekbd_keyboard_config_split_items
				    (*option, &g, &o)) {
					if (!g_ascii_strcasecmp
					    (g, GROUP_SWITCHERS_GROUP)) {
						any_switcher = True;
						break;
					}
				}
				option++;
			}
		}
		if (!any_switcher) {
			const gchar *id =
			    matekbd_keyboard_config_merge_items
			    (GROUP_SWITCHERS_GROUP,
			     DEFAULT_GROUP_SWITCH);
			options_list =
			    matekbd_strv_append (options_list, g_strdup (id));
			*was_appended = TRUE;
		}
	}
	return options_list;
}
