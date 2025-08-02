
#include <string>
#include <vector>
#include <format>

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>

#include "agenda.h"
#include "hello.h"


class QuestionWin : public Fl_Window {
	Fl_Box *qtext;
	Fl_Button *qdone, *qcancel=NULL;

public:
	sign_rec_ptr sign;
	Fl_Input* qresp;

	QuestionWin( sign_rec_ptr data, bool cancelable,
		int W=340, int H=180, const char* label = 0) : Fl_Window(W, H, "Question") {
		int mx = 10, my = 10;
		static char buf[80] = { 0 };
		sign = data;
		if (sign) sprintf(buf, "What is the value of %s", sign->str);
		qtext = new Fl_Box(mx, my, 300, 20, sign ? buf : label);
		qresp = new Fl_Input(mx+100, my+20+my, 200, 20, "Type Value ");
		qdone = new Fl_Button(mx+240, my+my+my+20+20, 60, 20, "OK");
		
		if (cancelable) {
			qcancel = new Fl_Button(mx, my + my + my + 20 + 20, 60, 20, "Cancel");
			qcancel->callback(cb_qwcancel, (void*)this);
			qdone->callback(cb_qwok, (void*)this);
		}
		else {
			qdone->callback(cb_qwok_knowcess, (void*)this);
		}
		end();
	}
	~QuestionWin() { 
		delete qtext;
		delete qresp;
		delete qdone;
		if (qcancel) delete qcancel;
	}
};



