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
#include <cstdarg>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Display.H>
#include <FL/fl_ask.H>

#include "agenda.h"
#include "Ency.hpp"
#include "QuestionWin.hpp"
#include "hello.h"

#define _UPDATE_ENCYS(a)       (a)->ency[0]->table->redraw(); \
    (a)->ency[1]->table->redraw(); \



//----------------------------------------------------------------------
// Minimal setup and ancillaries for engine
//----------------------------------------------------------------------
engine_state_rec_ptr S_State;


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

  //-------------------------------------------------------------------------------------------------------
  // Menu callbacks to NXP API
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

  static void suggest_cb(Fl_Widget* w, void* v) {
	  Application* app = (Application*)v;
      int rt, cl, rb, cr;
      hypo_rec_ptr hypo;
      EncyTable *table = app->ency[1]->table;
      table->get_selection(rt, cl, rb, cr );
      hypo = (hypo_rec_ptr) table->get_sign_at(rt);
      //
      engine_pushnew_hypo(S_State, hypo);
  }

  static void volunteer_cb(Fl_Widget* w, void* v) {
      Application* app = (Application*)v;
      int rt, cl, rb, cr;
      sign_rec_ptr sign;
      EncyTable* table = app->ency[0]->table;
      table->get_selection(rt, cl, rb, cr);
      sign = table->get_sign_at(rt);
      //
      if (app->qwin) delete app->qwin;
      app->qwin = new QuestionWin(sign,true);
      app->qwin->show();
  }

  static void knowcess_cb(Fl_Widget* w, void* v) {
      Application* app = (Application*)v;
      engine_resume_knowcess(S_State);
      _UPDATE_ENCYS(app);
  }

  // Handle an 'Open' request from the menu
  static void open_cb(Fl_Widget *w, void *v) {
    Application *app = (Application *)v;
    app->fc->title("Load Knowledge Base");
    app->fc->type(Fl_Native_File_Chooser::BROWSE_FILE); // only picks files that exist
    switch (app->fc->show()) {
      case -1:
        break; // Error
      case 1:
        break; // Cancel
      default: // Choice
        app->fc->preset_file(app->fc->filename());
        app->open(app->fc->filename());
        repl_log("Loaded knowledge base '%s'\n", app->fc->filename());
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
    Fl_Text_Buffer* buff;

    EncyWin *ency[2];
    QuestionWin* qwin;


  // CTOR
  Application()
    : Fl_Window(400, 200, "NXP") {
    resizable(this);
    // Menubar
    Fl_Menu_Bar *menu = new Fl_Menu_Bar(0, 0, 400, 25);
    menu->add("&File/&LoadKB", FL_COMMAND + 'o', open_cb, (void *)this);
    menu->add("&File/&Save", FL_COMMAND + 'w', save_cb, (void *)this, FL_MENU_INACTIVE);
    menu->add("&File/&Save As", 0, saveas_cb, (void *)this, FL_MENU_INACTIVE | FL_MENU_DIVIDER);
    menu->add("&File/&Quit", FL_COMMAND + 'q', quit_cb);
    //
    menu->add("&Expert/&Suggest", FL_COMMAND + 's', suggest_cb, (void*)this);
    menu->add("&Expert/&Volunteer", FL_COMMAND + 'v', volunteer_cb, (void*)this, FL_MENU_DIVIDER);
    menu->add("&Expert/&Agenda", FL_COMMAND + 'a', quit_cb, (void*)this);
    menu->add("&Expert/&Knowcess", FL_COMMAND + 'k', knowcess_cb, (void*)this);
    //
    menu->add("&Encylopedia/&Sign", FL_COMMAND + 'd', sign_cb, (void*)this);    
    menu->add("&Encylopedia/&Hypotheses", FL_COMMAND + 'y', hypo_cb, (void*)this);
    menu->add("&Encylopedia/&Rules", FL_COMMAND + 'y', hypo_cb, (void*)this);

    buff = new Fl_Text_Buffer();
    Fl_Text_Display* disp = new Fl_Text_Display(10, 10+25, 380, 180-25);
    disp->buffer(buff);                 // attach text buffer to display widget
    resizable(disp);
    buff->text("NXP Architecture\nSession\n");
    
    // Initialize the file chooser
    fc = new Fl_Native_File_Chooser();
    fc->filter("Org Files\t*.org\nAll Files\t*.*");
    fc->preset_file(untitled_default());
    end();
    
    // Encyclopedia
    ency[1] = new EncyWin((sign_rec_ptr)loadkb_get_allhypos(), EncyTable::item::HYPO);
    ency[0] = new EncyWin((sign_rec_ptr)loadkb_get_allsigns(), EncyTable::item::SIGN);
    // Question window
    qwin = new QuestionWin(NULL,false);

  }
};

Application* S_app = (Application*)0;
Application* repl_getApp() { return S_app; }


engine_state_rec_ptr repl_getState() {
    return S_State;
}

void repl_log(const char* fmt, ...) {
    char buf[255];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, 255, fmt, args);
    va_end(args);
    S_app->buff->append(buf, -1);
}

const char* S_Color[] = { "\x1b[38;5;46m", "\x1b[38;5;160m", "\x1b[38;5;15m" };

char* S_val_color(unsigned short val) {
    char* esc;
    switch (val) {
    case _TRUE:
        esc = (char*)S_Color[0];
        break;
    case _FALSE:
        esc = (char*)S_Color[1];
        break;
    default:
        esc = (char*)S_Color[2];
    }
    return esc;
}

std::string repl_val_repr(struct val_rec* val) {
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


//----------------------------------------------------------------------
// Callbacks from QuestionWin
//----------------------------------------------------------------------
void cb_qwok(Fl_Widget* but, void* userdata) {
    sign_rec_ptr sign = ((QuestionWin*)userdata)->sign;
    Fl_Input* resp = ((QuestionWin*)userdata)->qresp;
    struct val_rec val;
    if (sign) {
        val.status = _KNOWN;
        val.type = sign->val.type;
        switch (sign->val.type) {
        case _VAL_T_STR:
            val.valptr = (char*)malloc(sizeof(char) * strlen(resp->value()));
            strcpy(val.valptr, resp->value());
            break;
        case _VAL_T_BOOL:
            val.val_bool = std::stoi(resp->value());
            break;
        case _VAL_T_INT:
            val.val_int = std::stoi(resp->value());
            break;
            // TODO: float
        }

        sign_set_default(sign, &val);
        engine_pushnew_signdata(repl_getState(), sign, &val);
        repl_log("Volunteered %s = %s.\n", sign->str, resp->value());
        ((QuestionWin*)userdata)->hide();
        S_app->ency[0]->table->redraw();
    }
}

void cb_qwok_knowcess(Fl_Widget* but, void* userdata) {
    sign_rec_ptr sign = ((QuestionWin*)userdata)->sign;
    Fl_Input* resp = ((QuestionWin*)userdata)->qresp;
    struct val_rec val;
    if (sign) {
        val.status = _KNOWN;
        val.type = sign->val.type;
        switch (sign->val.type) {
        case _VAL_T_STR:
            val.valptr = (char*)malloc(sizeof(char) * strlen(resp->value()));
            strcpy(val.valptr, resp->value());
            break;
        case _VAL_T_BOOL:
            val.val_bool = std::stoi(resp->value());
            break;
        case _VAL_T_INT:
            val.val_int = std::stoi(resp->value());
            break;
            // TODO: float
        }

        sign_set_default(sign, &val);
        repl_log("Answered %s = %s.\n", sign->str, resp->value());
        ((QuestionWin*)userdata)->hide();
        engine_resume_knowcess(S_State);
        _UPDATE_ENCYS(S_app);
    }
}

void cb_qwcancel(Fl_Widget* but, void* userdata) {
    ((QuestionWin*)userdata)->hide();
}

//----------------------------------------------------------------------
// Getter callbacks from the engine
//----------------------------------------------------------------------
void getter_sign(sign_rec_ptr sign, int* suspend) {
    if (S_app->qwin) delete S_app->qwin;
    S_app->qwin = new QuestionWin(sign, false);
    *suspend = _TRUE;

    S_app->qwin->show();
}

static  struct val_rec v_true = { _KNOWN, _VAL_T_BOOL, (char*)0, _TRUE, 0, 0.0, 0 };
static  struct val_rec v_false = { _KNOWN, _VAL_T_BOOL, (char*)0, _FALSE, 0, 0.0, 0 };

void engine_dsl_getter_compound(compound_rec_ptr compound, int* suspend) {
#ifdef ENGINE_DSL_HOWERJFORTH
    if (_KNOWN == compound->val.status)
        return;
    //
    int  err;
    repl_log("Getter compound %s (%d)\n", compound->str, *suspend);
    //fprintf(stderr, "Getter for %s\n|%s|\n", compound->str, compound->dsl_expression);
    int r = engine_dsl_eval_async((const char*)compound->dsl_expression, &err, suspend);
    repl_log("FORTH Res %d Err %d Susp %d\n", r, err, *suspend);
    repl_log("Post-eval compound %s (%d)\n", compound->str, *suspend);
    switch (err) {
    case 0:
        // Ignore DSL evaluation if a question is pending! Re-evaluation will happen later.
        if (_FALSE == *suspend) {
            // sprintf( buf, "Getter compound %s (%d)\n", compound->str,
            // 	       // (char *) (compound->dsl_expression)
            // 	       *suspend
            // 	       );
            // printf( buf );
            sign_set_default((sign_rec_ptr)compound, r ? &v_true : &v_false);
            // sprintf( buf, "Compound Status %d Type %d\n", compound->val.status, compound->val.type );
            // printf( "%s", buf );
        }
        break;
    }
#endif
}

int main(int argc, char* argv[]) {
    int res;
    repl_init();
    //
    Fl::scheme("gtk+");
    Application* app = new Application();
    S_app = app;
    app->show(argc, argv);
    res = Fl::run();
    //
    repl_free();
    return res;
}

//
// End of "$Id$".
//