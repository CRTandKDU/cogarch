#ifndef QUESTION_HPP
#define QUESTION_HPP
#include <final/final.h>

using namespace finalcut;

class QuestionWidget : public FDialog
{
  public:
  explicit QuestionWidget (FWidget* parent = nullptr, unsigned short def = _TRUE )
    : FDialog{parent}
    {
      // Connect the button signal "clicked" with the callback method
      if( def ) button.addCallback ("clicked", this, &QuestionWidget::cb_ok );
    }

  FString current_sign;
  FLineEdit input{"Type answer here", this};
  FButton button{"O&K", this};

  private:
  void initLayout();
  inline void checkMinValue (int& n)
    {
      if ( n < 1 )  // Checks and corrects the minimum value
        n = 1;
    }

  void centerDialog();
  void adjustWidgets();
  void adjustSize() override;
  void draw() override;
  void cb_ok();

};
#endif
