/**
 * nxpiup_menu.c -- Main operations menu
 *
 * Written on 2026-05-06.
 */

#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <iup.h>
#include <cd.h>
#include <cdiup.h>
#include <wd.h>

#include "agenda.h"
#include "nxpiup.h"
#include "netw.h"

sign_rec_ptr S_current_sign = NULL;

int qbut_cb( Ihandle *ih ){
  char buf[NXPIUP_TEMP_BUFSIZE];
  Ihandle *qtext = IupGetHandle( "question_text" );
  printf( "Answer: %s for %s\n", IupGetAttribute( qtext, "VALUE" ), S_current_sign->str );
  IupHide( IupGetHandle( "question" ) );
  // Handle sign value's expected type
  struct val_rec val;
  sign_rec_ptr sign = S_current_sign;
  S_current_sign = NULL;
  if( sign ){
    val.status = _KNOWN;
    val.type   = sign->val.type;
    switch( sign->val.type ){
    case _VAL_T_STR:
      if( val.valptr ) free( val.valptr );
      val.valptr = (char *)malloc( strlen( IupGetAttribute( qtext, "VALUE" ) ) );
      strcpy( val.valptr, IupGetAttribute( qtext, "VALUE" ) );
      //
      sprintf( buf, "[SESSION] Answer (%s): %s DSL: %d", sign->str, val.valptr, sign->val.val_forth );
      repl_log( buf );
      break;
    case _VAL_T_INT:
      val.val_int = atoi( IupGetAttribute( qtext, "VALUE" ) );
      sprintf( buf, "[SESSION] Answer (%s): %d DSL: %d", sign->str, val.val_int, sign->val.val_forth );
      repl_log( buf );
      break;
    }
    sign_set_default( sign, &val );
    engine_resume_knowcess( repl_getState() );
  }  
  return IUP_DEFAULT;
}

void nxpiup_dlgquestion( sign_rec_ptr sign ){
  Ihandle *qlabel;
  Ihandle *dlg = IupGetHandle( "question" );
  char buf[NXPIUP_TEMP_BUFSIZE];
  S_current_sign = sign;
  sprintf( buf, "What is the value of %s?", sign->str );
  if( dlg ){
    IupSetAttribute( IupGetHandle("question_label"), "TITLE", buf );
    IupSetAttribute( IupGetHandle("question_text"), "VALUE", "" );
  }
  else{
    qlabel = IupLabel( buf );
    IupSetHandle( "question_label", qlabel );
    IupSetAttribute( qlabel, "ALIGNMENT", "ACENTER:ACENTER" );
    //
    Ihandle *qtext  = IupText( NULL );
    IupSetHandle( "question_text", qtext );
    IupSetAttribute( qtext, "VISIBLECOLUMNS", "16" );
    Ihandle *qbut   = IupButton( "OK", "qbut" );
    IupSetCallback( qbut, "ACTION", (Icallback) qbut_cb );
    Ihandle *qtext_box = IupHbox( qtext, qbut, NULL );
    IupSetAttribute( qtext_box, "ALIGNMENT", "ACENTER" );
    IupSetAttribute( qtext_box, "GAP", "20" );
    IupSetAttribute( qtext_box, "MARGIN", "20x20" );
    //
    Ihandle *qvbox = IupVbox( qlabel, qtext_box, NULL );
    IupSetAttribute( qvbox, "EXPANDCHILDREN", "YES" );

    dlg = IupDialog( qvbox  );
    IupSetAttributes( dlg, "EXPAND = YES, TITLE = Question, RESIZE = NO" );
    IupSetAttributes( dlg, "MENUBOX = NO, MAXBOX = NO, MINBOX = NO" );
    IupSetAttribute( dlg, "SIZE", "QUARTERxQUARTER" );
    IupSetHandle( "question", dlg );
  }
  //
  NXPIUP_UPDATES
  /* Shows dialog on the center of the screen */
  IupShowXY( dlg, IUP_CENTER, IUP_CENTER );
}

// Loading knowledge bases. (Specially formatted Org-mode files.)
int nxpiup_dlgloadkb( void ){
  int res = 0;
  Ihandle *dlg = IupFileDlg(); 

  IupSetAttribute(dlg, "DIALOGTYPE", "OPEN");
  IupSetAttribute(dlg, "TITLE", "Load Knowledge Base");
  IupSetAttributes(dlg, "FILTER = \"*.org\", FILTERINFO = \"Org-mode Files\"");
  /* IupSetCallback(dlg, "HELP_CB", (Icallback)help_cb); */

  IupPopup(dlg, IUP_CURRENT, IUP_CURRENT); 

  if (IupGetInt(dlg, "STATUS") != -1)
    {
      printf("OK\n");
      printf("  VALUE(%s)\n", IupGetAttribute(dlg, "VALUE"));
      res = loadkb_file( IupGetAttribute(dlg, "VALUE") );
      char buf[64];
      sprintf( buf, "[KB] Loaded KB: %s - %s", IupGetAttribute(dlg, "VALUE"), res ? "Failed" : "OK" );
      repl_log( buf );
    }
  else
    printf("CANCEL\n");

  IupDestroy(dlg);
  return res;
}

// Main window w. menubar
int exit_cb(void) { return IUP_CLOSE; }

int item_browse_cb(void){
  Ihandle *ih_item = IupGetHandle( "item_browse" );
  IupSetAttribute( ih_item, "ACTIVE", "NO" );
  CanvasScrollbarTest();
  return IUP_DEFAULT;
}

int item_open_cb( void ){
  int res = nxpiup_dlgloadkb();
  return IUP_DEFAULT;
}

int item_knowcess_cb( void ){
  //
  Ihandle *ih_item = IupGetHandle( "item_knowcess" );
  IupSetAttribute( ih_item, "ACTIVE", "NO" );
  // For tests
  hypo_rec_ptr h = (hypo_rec_ptr)sign_find( "POSSIBLE_LEAK", loadkb_get_allhypos() );
  engine_pushnew_hypo( repl_getState(), h );
  repl_msg( "[SESSION] Suggest %s", h->str );
  //
  engine_resume_knowcess( repl_getState() );
  
  return IUP_DEFAULT;
}

int item_hypos_cb( void ){
  nxpiup_dlgency( NXPIUP_ENCY_HYPOS_TITLE, NXPIUP_ENCY_HYPOS, (sign_rec_ptr) loadkb_get_allhypos() );
  return IUP_DEFAULT;
}

int item_signs_cb( void ){
  nxpiup_dlgency( NXPIUP_ENCY_SIGNS_TITLE, NXPIUP_ENCY_SIGNS, (sign_rec_ptr) loadkb_get_allsigns() );
  return IUP_DEFAULT;
}

int item_rules_cb( void ){
  nxpiup_dlgency_rules( NXPIUP_ENCY_RULES_TITLE, NXPIUP_ENCY_RULES, (rule_rec_ptr) loadkb_get_allrules() );
  return IUP_DEFAULT;
}

void nxpiup_dlgmenu( void ){
  Ihandle *item_open, *item_exit,
    *item_suggest, *item_volunteer, *item_reset, *item_agenda, *item_knowcess,
    *item_rules, *item_signs, *item_hypos,
    *item_browse;
  Ihandle *file_menu, *edit_menu, *expert_menu, *ency_menu, *netw_menu;
  Ihandle *menu, *sub1, *sub3, *sub4, *sub5;

  item_open = IupItem ("Open...", NULL);
  IupSetAttribute(item_open, "KEY", "O");
  IupSetCallback(item_open, "ACTION", (Icallback)item_open_cb);
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
  IupSetAttribute(item_reset, "KEY", "Z");
  item_agenda		= IupItem ("Agenda", NULL);
  IupSetAttribute(item_agenda, "KEY", "A");
  item_knowcess		= IupItem ("Knowcess", NULL);
  IupSetAttribute(item_knowcess, "KEY", "K");
  IupSetCallback(item_knowcess, "ACTION", (Icallback)item_knowcess_cb);
  IupSetHandle( "item_knowcess", item_knowcess );
  //
  expert_menu = IupMenu( item_suggest, item_volunteer, item_reset,
			 IupSeparator(), item_agenda, item_knowcess, NULL );
  //
  item_rules = IupItem ("Rules", NULL);
  IupSetAttribute(item_rules, "KEY", "R");
  IupSetCallback(item_rules, "ACTION", (Icallback)item_rules_cb);
  item_signs = IupItem ("Signs", NULL);
  IupSetAttribute(item_signs, "KEY", "G");
  IupSetCallback(item_signs, "ACTION", (Icallback)item_signs_cb);
  item_hypos = IupItem ("Hypotheses", NULL);
  IupSetAttribute(item_hypos, "KEY", "H");
  IupSetCallback(item_hypos, "ACTION", (Icallback)item_hypos_cb);
  //
  ency_menu = IupMenu( item_rules, item_signs, item_hypos, NULL );
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
  char log_version[192];
  sprintf( log_version, VERSION_LOG, __DATE__, VERSION_GUI, VERSION_DSL, VERSION_NXP );
  Ihandle *log = IupMultiLine( NULL );
  IupSetAttribute( log, "READONLY", "YES" );
  IupSetAttribute( log, "VISIBLELINES", "20" );
  IupSetAttribute( log, "VISIBLECOLUMNS", "32" );
  IupSetAttribute( log, "EXPAND", "YES" );
  IupSetAttribute( log, "VALUE", log_version );
  IupSetHandle( "ih_log", log );
  /* Ihandle *vbox_log = IupVbox( log, NULL ); */
  /* IupSetAttribute( vbox_log, "EXPANDCHILDREN", "YES" );  */

  Ihandle *dlg = IupDialog( log );
  IupSetAttribute( dlg, "MENU", "mymenu" );
  IupSetAttribute( dlg, "TITLE", "NXPIUP");
  IupSetAttribute( dlg, "EXPAND", "YES");
  IupSetAttribute( dlg, "SIZE", "HALFxHALF" );

  IupShow(dlg);
}

