/**
 * nxpiup_menu.c -- Main operations menu
 *
 * Written on 2026-05-06.
 */

#include <stdlib.h>
#include <stdio.h>
#include <iup.h>
#include <cd.h>
#include <cdiup.h>
#include <wd.h>

#include "agenda.h"
#include "nxpiup.h"
#include "netw.h"

int exit_cb(void) { return IUP_CLOSE; }

int item_browse_cb(void){
  Ihandle *ih_item = IupGetHandle( "item_browse" );
  IupSetAttribute( ih_item, "ACTIVE", "NO" );
  CanvasScrollbarTest();
  return IUP_DEFAULT;
}

void nxpiup_dlgmenu( void ){
  Ihandle *item_open, *item_exit,
    *item_suggest, *item_volunteer, *item_reset, *item_agenda, *item_knowcess,
    *item_signs, *item_hypos,
    *item_browse;
  Ihandle *file_menu, *edit_menu, *expert_menu, *ency_menu, *netw_menu;
  Ihandle *menu, *sub1, *sub3, *sub4, *sub5;

  item_open = IupItem ("Open...", NULL);
  IupSetAttribute(item_open, "KEY", "O");
  item_exit = IupItem ("Quit", NULL);
  IupSetAttribute(item_exit, "KEY", "Q");
  IupSetCallback(item_exit, "ACTION", (Icallback)exit_cb);
  //
  file_menu = IupMenu(item_open, IupSeparator(), item_exit, NULL);
  printf( "1 " );
  //
  item_suggest		= IupItem ("Suggest", NULL);
  IupSetAttribute(item_suggest, "KEY", "S");
  item_volunteer	= IupItem ("Volunteer", NULL);
  IupSetAttribute(item_volunteer, "KEY", "V");
  item_reset		= IupItem ("Reset", NULL);
  IupSetAttribute(item_reset, "KEY", "R");
  item_agenda		= IupItem ("Agenda", NULL);
  IupSetAttribute(item_agenda, "KEY", "A");
  item_knowcess		= IupItem ("Knowcess", NULL);
  IupSetAttribute(item_knowcess, "KEY", "K");
  //
  expert_menu = IupMenu( item_suggest, item_volunteer, item_reset,
			 IupSeparator(), item_agenda, item_knowcess, NULL );
  //
  item_signs = IupItem ("Signs", NULL);
  IupSetAttribute(item_signs, "KEY", "G");
  item_hypos = IupItem ("Hypotheses", NULL);
  IupSetAttribute(item_hypos, "KEY", "H");
  //
  ency_menu = IupMenu( item_signs, item_hypos, NULL );
  //
  item_browse = IupItem ("Browse", NULL);
  IupSetAttribute(item_browse, "KEY", "B");
  IupSetCallback(item_browse, "ACTION", (Icallback)item_browse_cb);
  IupSetHandle( "item_browse", item_browse );
  //
  netw_menu = IupMenu( item_browse, NULL );
  //
  sub1 = IupSubmenu( "File", file_menu );
  sub3 = IupSubmenu( "Expert", expert_menu );
  sub4 = IupSubmenu( "Encyclopedia", ency_menu );
  sub5 = IupSubmenu( "Network", netw_menu );
  menu = IupMenu( sub1, sub3, sub4, sub5, NULL );
  IupSetHandle("mymenu", menu);
  //
  Ihandle *dlg = IupDialog( IupCanvas( "" ) );
  IupSetAttribute( dlg, "MENU", "mymenu" );
  IupSetAttribute( dlg, "TITLE", "NXPIUP");
  IupSetAttribute( dlg, "SIZE", "QUARTERxQUARTER" );

  IupShow(dlg);
}

