/********************************************************************\
 * option-util.h -- GNOME<->guile option interface                  *
 * Copyright (C) 1998,1999 Linas Vepstas                            *
 *                                                                  *
 * This program is free software; you can redistribute it and/or    *
 * modify it under the terms of the GNU General Public License as   *
 * published by the Free Software Foundation; either version 2 of   *
 * the License, or (at your option) any later version.              *
 *                                                                  *
 * This program is distributed in the hope that it will be useful,  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
 * GNU General Public License for more details.                     *
 *                                                                  *
 * You should have received a copy of the GNU General Public License*
 * along with this program; if not, write to the Free Software      *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.        *
\********************************************************************/

#ifndef __OPTION_UTIL_H__
#define __OPTION_UTIL_H__

#include <gnome.h>
#include <guile/gh.h>

typedef struct _GNCOption GNCOption;
struct _GNCOption
{
  /* Handle to the scheme-side option */
  SCM guile_option;

  /* Identifier for unregistering */
  SCM guile_option_id;

  /* Flag to indicate change by the UI */
  gboolean changed;

  /* The widget which is holding this option */
  GtkWidget *widget;
};

typedef struct _GNCOptionSection GNCOptionSection;
typedef struct _GNCOptionDB GNCOptionDB;

typedef int GNCOptionDBHandle;

typedef void (*OptionChangeCallback)(gpointer user_data);

/***** Prototypes ********************************************************/

GNCOptionDB * gnc_option_db_new();
void          gnc_option_db_init(GNCOptionDB *odb, SCM options);
void          gnc_option_db_destroy(GNCOptionDB *odb);

void gnc_option_db_register_change_callback(GNCOptionDB *odb,
                                            OptionChangeCallback callback,
                                            gpointer data);

char * gnc_option_section(GNCOption *option);
char * gnc_option_name(GNCOption *option);
char * gnc_option_type(GNCOption *option);
char * gnc_option_sort_tag(GNCOption *option);
char * gnc_option_documentation(GNCOption *option);
SCM    gnc_option_getter(GNCOption *option);
SCM    gnc_option_setter(GNCOption *option);
SCM    gnc_option_default_getter(GNCOption *option);
SCM    gnc_option_value_validator(GNCOption *option);
int    gnc_option_num_permissible_values(GNCOption *option);
int    gnc_option_permissible_value_index(GNCOption *option, SCM value);
SCM    gnc_option_permissible_value(GNCOption *option, int index);
char * gnc_option_permissible_value_name(GNCOption *option, int index);
char * gnc_option_permissible_value_description(GNCOption *option, int index);
gboolean gnc_option_show_time(GNCOption *option);
gboolean gnc_option_multiple_selection(GNCOption *option);

guint gnc_option_db_num_sections(GNCOptionDB *odb);

char * gnc_option_section_name(GNCOptionSection *section);
guint  gnc_option_section_num_options(GNCOptionSection *section);

GNCOptionSection * gnc_option_db_get_section(GNCOptionDB *odb, gint i);

GNCOption * gnc_get_option_section_option(GNCOptionSection *section, int i);

GNCOption * gnc_option_db_get_option_by_name(GNCOptionDB *odb,
                                             char *section_name, char *name);

GNCOption * gnc_option_db_get_option_by_SCM(GNCOptionDB *odb,
                                            SCM guile_option);

gboolean gnc_option_db_dirty(GNCOptionDB *odb);
void     gnc_option_db_clean(GNCOptionDB *odb);

void gnc_option_db_commit(GNCOptionDB *odb);

gboolean gnc_option_db_lookup_boolean_option(GNCOptionDB *odb,
                                             char *section, char *name,
                                             gboolean default_value);

char * gnc_option_db_lookup_string_option(GNCOptionDB *odb, char *section,
                                          char *name, char *default_value);

char * gnc_option_db_lookup_multichoice_option(GNCOptionDB *odb,
                                               char *section, char *name,
                                               char *default_value);

/* private */

void _gnc_option_db_register_option(GNCOptionDBHandle handle,
                                    SCM guile_option);

/* These should be in src/guile or src/g-wrap, but they use glib */

SCM     gnc_account_list_to_scm(GList *account_list);
GList * gnc_scm_to_account_list(SCM scm_list);

SCM _gnc_get_current_accounts();

#endif /* __OPTION_UTIL_H__ */
