/********************************************************************\
 * window-help.c -- a help window for hypertext help.               *
 * Copyright (C) 1997 Robin D. Clark                                *
 * Copyright (C) 1998 Linas Vepstas                                 *
 * Copyright (C) 1999 Jeremy Collins ( gtk-xmhtml port )            *
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
 *                                                                  *
 *   Author: Rob Clark                                              *
 * Internet: rclark@cs.hmc.edu                                      *
 *  Address: 609 8th Street                                         *
 *           Huntington Beach, CA 92648-4632                        *
\********************************************************************/

#include <top-level.h>

#include <gnome.h>

#include "window-help.h"
#include "window-html.h"
#include "Sheet.h"
#include "File.h"
#include "messages.h"
#include "util.h"

static short module = MOD_HTML; 

static HTMLWindow *helpwindow = NULL;


typedef struct _HelpData HelpData;
struct _HelpData
{
  gchar *htmlfile;
  gchar *title;
  gchar *label;
  gchar *text;
};


static HelpData *
help_data_new()
{
  HelpData *help_data;
  
  help_data = g_new0(HelpData, 1);

  return help_data;
}

static void
help_data_destroy(HTMLUserData history_data)
{
  HelpData *help_data = history_data;

  g_free(help_data->htmlfile);
  help_data->htmlfile = NULL;

  g_free(help_data->title);
  help_data->title = NULL;

  g_free(help_data->label);
  help_data->label = NULL;

  g_free(help_data->text);
  help_data->text = NULL;

  g_free(help_data);
}

static void
help_data_set_file(HelpData *help_data, const gchar *htmlfile)
{
  g_free(help_data->htmlfile);
  help_data->htmlfile = g_strdup(htmlfile);
}

static void
help_data_set_title(HelpData *help_data, const gchar *title)
{
  g_free(help_data->title);
  help_data->title = g_strdup(title);
}

static void
help_data_set_label(HelpData *help_data, const gchar *label)
{
  g_free(help_data->label);
  help_data->label = g_strdup(label);
}

static void
help_data_set_text(HelpData *help_data, const gchar *text)
{
  g_free(help_data->text);
  help_data->text = g_strdup(text);
}


static HTMLData *
helpAnchorCB(XmHTMLAnchorCallbackStruct *acbs, HTMLUserData user_data)
{
  HelpData *user = user_data;
  HTMLData *html_data;
  HelpData *help_data;

  switch(acbs->url_type)
  {
    /* a local file with a possible jump to label */
    case ANCHOR_FILE_LOCAL:
      help_data = help_data_new();
      help_data_set_file(help_data, acbs->href);
      help_data_set_title(help_data, user->title);

      html_data = gnc_html_data_new(user->title, help_data,
                                    help_data_destroy,
                                    NULL, 0);

      return html_data;

    /* other types are unsupported, but it would be fun if they were ... */
    case ANCHOR_FTP:
      PERR(" this help window doesn't support ftp: %s\n", acbs->href);
      break;
    case ANCHOR_HTTP:
      PERR (" this help window doesn't support http: %s\n", acbs->href);
      break;
    case ANCHOR_MAILTO:
      PERR(" this help window doesn't support email: %s\n", acbs->href);
      break;
    case ANCHOR_UNKNOWN:
    default:
      PERR(" don't know this type of url: %s\n", acbs->href);
      break;
  }

  return NULL;
}

static void
helpJumpCB(HTMLUserData user_data, char **set_text, char **set_label)
{
  HelpData *help_data = user_data;
  char *text = NULL;
  char *label = NULL;

  *set_text = NULL;
  *set_label = NULL;

  if (help_data->text != NULL)
  {
    *set_text = help_data->text;
    *set_label = help_data->label;
    return;
  }

  if (help_data->htmlfile == NULL)
    return;

  /* see if this anchor contains a jump */
  label = strpbrk(help_data->htmlfile, "#?");
  if (label != NULL)
  {
    help_data_set_label(help_data, label);

    /* truncate # from name */
    help_data->htmlfile[label - help_data->htmlfile] = 0x0;
  }

  /* see if the anchor is an "active gnucash page" */
  if (strstr(help_data->htmlfile, ".phtml"))
    text = gncReport(help_data->htmlfile);

  /* if text to display wasn't specified, use the truncated name to read */
  if (text == NULL)
    text = gncReadFile(help_data->htmlfile);

  if (text != NULL)
  {
    help_data_set_text(help_data, text);
    free(text);
  }

  *set_text = help_data->text;
  *set_label = help_data->label;
}


/********************************************************************\
 * helpWindow                                                       * 
 *   opens up a help window, and displays html                      * 
 *                                                                  * 
 * Args:   parent   - the parent widget                             * 
 *         title    - the title of the window                       * 
 *         htmlfile - the file name of the help file to display     * 
 * Return: none                                                     * 
\********************************************************************/
void
helpWindow(GtkWidget *parent, const char *title, const char *htmlfile)
{
  HTMLData *html_data;
  HelpData *help_data;

  if (helpwindow == NULL)
    helpwindow = gnc_html_window_new(helpAnchorCB, helpJumpCB);

  help_data = help_data_new();
  help_data_set_file(help_data, htmlfile);
  help_data_set_title(help_data, title);

  html_data = gnc_html_data_new(title, help_data, help_data_destroy, NULL, 0);

  htmlWindow(parent, &helpwindow, html_data);
}


/********************************************************************\
 * gnc_ui_destroy_help_windows                                      * 
 *   destroys any open help windows                                 * 
 *                                                                  * 
 * Args:   none                                                     * 
 * Return: none                                                     * 
\********************************************************************/
void
gnc_ui_destroy_help_windows()
{
  gnc_html_window_destroy(helpwindow);
  helpwindow = NULL;

  DEBUG("help windows destroyed.\n");
}

/* ----------------------- END OF FILE ---------------------  */
