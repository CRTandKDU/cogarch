//
// "$Id$"
//
//	An example of how to use Fl_Native_File_Chooser to open & save files.
//
// Copyright 2010 Greg Ercolano.
// Copyright 1998-2010 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     http://www.fltk.org/COPYING.php
//
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//
#include <stdio.h>  // printf
#include <stdlib.h> // exit,malloc
#include <string> // strerror
#include <errno.h>  // errno

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Box.H>
#include <FL/fl_ask.H>

#include "agenda.h"
#include "hello.h"

#include "Ency.cpp"

//----------------------------------------------------------------------
// Minimal setup and ancillaries for engine
//----------------------------------------------------------------------
engine_state_rec_ptr S_State;

engine_state_rec_ptr repl_getState() {
  return S_State;
}

void repl_log(const char *s) {}

void engine_dsl_getter_compound(compound_rec_ptr compound, int *suspend) {
#ifdef ENGINE_DSL_HOWERJFORTH
  if (_KNOWN == compound->val.status)
    return;
#endif
}

const char *S_Color[] = {"\x1b[38;5;46m", "\x1b[38;5;160m", "\x1b[38;5;15m"};

char *S_val_color(unsigned short val) {
  char *esc;
  switch (val) {
    case _TRUE:
      esc = (char *)S_Color[0];
      break;
    case _FALSE:
      esc = (char *)S_Color[1];
      break;
    default:
      esc = (char *)S_Color[2];
  }
  return esc;
}

std::string repl_val_repr(struct val_rec *val) {
  if (_UNKNOWN == val->status)
    return std::string("UNKNOWN");
  switch (val->type) {
    case _VAL_T_BOOL:
      return (_FALSE == val->val_bool) ? std::string("FALSE") : std::string("TRUE");
      break;
    case _VAL_T_INT:
      return (std::to_string(val->val_int));
      break;
    case _VAL_T_FLOAT:
      break;
    case _VAL_T_STR:
      if (val->valptr)
        return (std::string(val->valptr));
      else
        return (std::string("error"));
      break;
  }
  return std::string("error");
}

//----------------------------------------------------------------------
// Getter callbacks from the engine
//----------------------------------------------------------------------
void getter_sign(sign_rec_ptr sign, int *suspend) {

}

void repl_init() {
  // New state
  S_State = (engine_state_rec_ptr)malloc(sizeof(struct engine_state_rec));
  S_State->current_sign = (sign_rec_ptr)0;
  S_State->agenda = (cell_rec_ptr)0;
  // Set up DSL
#ifdef ENGINE_DSL
  engine_dsl_init();
#endif
}

void repl_free() {
#ifdef ENGINE_DSL
  engine_dsl_free();
#endif
  loadkb_reset();
  engine_free_state(S_State);
}

//------------------------------------------------------------------------------------------------------
class Application : public Fl_Window {
  Fl_Native_File_Chooser *fc;
  // Does file exist?
  int exist(const char *filename) {
    FILE *fp = fl_fopen(filename, "r");
    if (fp) {
      fclose(fp);
      return (1);
    } else {
      return (0);
    }
  }
  // 'Open' the file
  void open(const char *filename) { 
      int res;
      printf("Open '%s'\n", filename);
      res = loadkb_file(filename);
      if (res)
        printf("Failed to load '%s'\n", filename);
  }

  // 'Save' the file
  //    Create the file if it doesn't exist
  //    and save something in it.
  //
  void save(const char *filename) {
    printf("Saving '%s'\n", filename);
    if (!exist(filename)) {
      FILE *fp = fl_fopen(filename, "w"); // create file if it doesn't exist
      if (fp) {
        // A real app would do something useful here.
        fprintf(fp, "Hello world.\n");
        fclose(fp);
      } else {
        fl_message("Error: %s: %s", filename, strerror(errno));
      }
    } else {
      // A real app would do something useful here.
    }
  }

  static void sign_cb(Fl_Widget* w, void* v) {
      Application* app = (Application*)v;
      if (app->ency[0]->visible()) { app->ency[0]->hide(); }
      else { app->ency[0]->show(); }
  }
  static void hypo_cb(Fl_Widget* w, void* v) {
      Application* app = (Application*)v;
      if (app->ency[1]->visible()) { app->ency[1]->hide(); }
      else { app->ency[1]->show(); }
  }


  // Handle an 'Open' request from the menu
  static void open_cb(Fl_Widget *w, void *v) {
    Application *app = (Application *)v;
    app->fc->title("Load Knowledge Base");
    app->fc->filter("Org Files\t*.org\nAll Files\t*.*");
    app->fc->type(Fl_Native_File_Chooser::BROWSE_FILE); // only picks files that exist
    switch (app->fc->show()) {
      case -1:
        break; // Error
      case 1:
        break; // Cancel
      default: // Choice
        app->fc->preset_file(app->fc->filename());
        app->open(app->fc->filename());
        app->ency[0]->table->fill(loadkb_get_allsigns());
        app->ency[1]->table->fill(loadkb_get_allhypos());
        break;
    }
  }

  // Handle a 'Save as' request from the menu
  static void saveas_cb(Fl_Widget *w, void *v) {
    Application *app = (Application *)v;
    app->fc->title("Save As");
    app->fc->type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE); // need this if file doesn't exist yet
    switch (app->fc->show()) {
      case -1:
        break; // Error
      case 1:
        break; // Cancel
      default: // Choice
        app->fc->preset_file(app->fc->filename());
        app->save(app->fc->filename());
        break;
    }
  }
  // Handle a 'Save' request from the menu
  static void save_cb(Fl_Widget *w, void *v) {
    Application *app = (Application *)v;
    if (strlen(app->fc->filename()) == 0) {
      saveas_cb(w, v);
    } else {
      app->save(app->fc->filename());
    }
  }
  static void quit_cb(Fl_Widget *w, void *v) { exit(0); }
  // Return an 'untitled' default pathname
  const char *untitled_default() {
    static char *filename = 0;
    if (!filename) {
      const char *home = getenv("HOME") ? getenv("HOME") : // unix
                             getenv("HOME_PATH") ? getenv("HOME_PATH")
                                                 : // windows
                             ".";                  // other
      filename = (char *)malloc(strlen(home) + 20);
      sprintf(filename, "%s/untitled.txt", home);
    }
    return (filename);
  }

public:
    EncyWin *ency[2];


  // CTOR
  Application()
    : Fl_Window(400, 200, "Native File Chooser Example") {
    Fl_Menu_Bar *menu = new Fl_Menu_Bar(0, 0, 400, 25);
    menu->add("&File/&LoadKB", FL_COMMAND + 'o', open_cb, (void *)this);
    menu->add("&File/&Save", FL_COMMAND + 'w', save_cb, (void *)this, FL_MENU_INACTIVE);
    menu->add("&File/&Save As", 0, saveas_cb, (void *)this, FL_MENU_INACTIVE | FL_MENU_DIVIDER);
    menu->add("&File/&Quit", FL_COMMAND + 'q', quit_cb);
    //
    menu->add("&Expert/&Suggest", FL_COMMAND + 's', quit_cb, (void*)this);
    menu->add("&Expert/&Volunteer", FL_COMMAND + 'v', quit_cb, (void*)this, FL_MENU_DIVIDER);
    menu->add("&Expert/&Agenda", FL_COMMAND + 'a', quit_cb, (void*)this);
    menu->add("&Expert/&Knowcess", FL_COMMAND + 'k', quit_cb, (void*)this);
    //
    menu->add("&Encylopedia/&Sign", FL_COMMAND + 'd', sign_cb, (void*)this);    
    menu->add("&Encylopedia/&Hypotheses", FL_COMMAND + 'y', hypo_cb, (void*)this);
    menu->add("&Encylopedia/&Rules", FL_COMMAND + 'y', hypo_cb, (void*)this);

    // Describe the demo..
    Fl_Box *box = new Fl_Box(20, 25 + 20, w() - 40, h() - 40 - 25);
    box->color(45);
    box->box(FL_FLAT_BOX);
    box->align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
    box->label("This demo shows an example of implementing "
               "common 'File' menu operations like:\n"
               "    File/Open, File/Save, File/Save As\n"
               "..using the Fl_Native_File_Chooser widget.\n\n"
               "Note 'Save' and 'Save As' really *does* create files! "
               "This is to show how behavior differs when "
               "files exist vs. do not.");
    box->labelsize(12);
    // Initialize the file chooser
    fc = new Fl_Native_File_Chooser();
    fc->filter("Text\t*.txt\n");
    fc->preset_file(untitled_default());
    end();
    // Encyclopedia
    ency[1] = new EncyWin((sign_rec_ptr)loadkb_get_allhypos(), EncyTable::item::HYPO);
    ency[0] = new EncyWin((sign_rec_ptr)loadkb_get_allsigns(), EncyTable::item::SIGN);

  }
};

int main(int argc, char *argv[]) {
  int res;
  repl_init();
  //
  Fl::scheme("gtk+");
  Application *app = new Application();
  app->show(argc, argv);
  res = Fl::run();
  //
  repl_free();
  return res;
}

//
// End of "$Id$".
//