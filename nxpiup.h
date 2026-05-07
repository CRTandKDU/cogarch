#ifndef NXPIUP_H
#define NXPIUP_H

#define VERSION_NXP "2025, Neuron Data"
#define VERSION_GUI "IUP 3.32"
#define VERSION_DSL "Embed Forth VM, 2018, Richard J. Howe"
#define VERSION_LOG "NXP Architecture (40 years)\n© 2025-2026 -- %s\nGUI: %s\nDSL: %s\nNXP: %s\n"

#define _NXP_CURRENT ((unsigned short)0xFD)

#define NXPIUP_TEMP_BUFSIZE 64

void			repl_log( const char* );
engine_state_rec_ptr	repl_getState();
int  nxpiup_inagendap( sign_rec_ptr sign );

void nxpiup_dlgmenu( void );
int  nxpiup_dlgloadkb( void );
void nxpiup_dlgquestion( sign_rec_ptr );

#endif
