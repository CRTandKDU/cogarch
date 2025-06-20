#include <final/final.h>
#include "agenda.h"
#include "Question.hpp"

extern engine_state_rec_ptr repl_getState();

using namespace finalcut;

void QuestionWidget::cb_ok(){
  FString val_str	= input.getText();
  int val_int		= std::stoi( val_str.c_str() );
  sign_rec_ptr sign	= sign_find( current_sign.c_str(), loadkb_get_allsigns() );
  struct val_rec        val;
  if( sign ){
    val.status = _KNOWN;
    val.type   = sign->val.type;
    switch( sign->val.type ){
    case _VAL_T_STR:
      val.valptr = (char *)malloc( strlen( val_str.c_str() ) );
      strcpy( val.valptr, val_str.c_str() );
      break;
    case _VAL_T_INT:
      val.val_int = val_int;
      break;
    }
    sign_set_default( sign, &val );
    hide();
    engine_resume_knowcess( repl_getState() );
  }
}

void QuestionWidget::initLayout()
{
  setText (L"Question");
  setResizeable();
  button.setGeometry (FPoint{1, 1}, FSize{12, 1}, false);
  input.setGeometry (FPoint{2, 3}, FSize{12, 1}, false);
  // Set dialog geometry and calling adjustSize()
  setGeometry (FPoint{25, 5}, FSize{40, 12});
  setMinimumSize (FSize{25, 9});
  FDialog::initLayout();
}

void QuestionWidget::centerDialog()
{
  auto x = int((getDesktopWidth() - getWidth()) / 2);
  auto y = int((getDesktopHeight() - getHeight()) / 2);
  checkMinValue(x);
  checkMinValue(y);
  setPos (FPoint{x, y}, false);
}

void QuestionWidget::adjustWidgets()
{
  const auto bx = int(getWidth() - button.getWidth() - 3);
  const auto by = int(getHeight() - 4);
  button.setPos (FPoint{bx, by}, false);
  input.setWidth (getWidth() - 4);
  const auto ly = int(getHeight() / 2) - 1;
  input.setY (ly, false);
}

void QuestionWidget::adjustSize() 
{
  // Calling super class method adjustSize()
  FDialog::adjustSize();
  // Centers the dialog in the terminal
  centerDialog();
  // Adjust widgets before drawing
  adjustWidgets();
}

void QuestionWidget::draw() 
{
  // Calling super class method draw()
  FDialog::draw();

  print() << FPoint{3, 3}
    << FColorPair{FColor::Black, FColor::White}
    << "What is the value of:"
    << FPoint{3, 4}
    << FColorPair{FColor::Blue, FColor::White}
    << current_sign;
}

