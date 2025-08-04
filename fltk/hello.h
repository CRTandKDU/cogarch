#pragma once
#include <string>


void repl_free();
void repl_init();
void repl_log(const char* fmt, ...);
std::string repl_val_repr(struct val_rec* val);
engine_state_rec_ptr repl_getState();
std::string repl_getLastdir();

void cb_qwcancel(Fl_Widget* but, void* userdata);
void cb_qwok(Fl_Widget* but, void* userdata);
void cb_qwok_knowcess(Fl_Widget* but, void* userdata);