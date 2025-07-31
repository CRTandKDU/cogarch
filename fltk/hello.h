#pragma once
#include <string>

void repl_free();
void repl_init();
void repl_log(const char* fmt, ...);
std::string repl_val_repr(struct val_rec* val);