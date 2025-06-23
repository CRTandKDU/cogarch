/***********************************************************************
* highlight-text.cpp - Example of FTextView with highlighted text      *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2022-2024 Markus Gans                                      *
*                                                                      *
* FINAL CUT is free software; you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as       *
* published by the Free Software Foundation; either version 3 of       *
* the License, or (at your option) any later version.                  *
*                                                                      *
* FINAL CUT is distributed in the hope that it will be useful, but     *
* WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include <final/final.h>

#include "Textwindow.hpp"

using finalcut::FColor;
using finalcut::FColorPair;
using finalcut::FPoint;
using finalcut::FRect;
using finalcut::FSize;
using finalcut::FStyle;
using FTextHighlight = finalcut::FTextView::FTextHighlight;

constexpr char init_text[] = \
R"(                   Session inited
)";



//----------------------------------------------------------------------
TextWindow::TextWindow (finalcut::FWidget* parent)
  : finalcut::FDialog{parent}
{
  setResizeable();
  scrolltext.ignorePadding();
  scrolltext.setFocus();
  scrolltext.append( init_text );
}

//----------------------------------------------------------------------
void TextWindow::initLayout()
{
  FDialog::setText("Session");
  setMinimumSize (FSize{51, 6});
  auto x = 1 + int((getDesktopWidth() - 72) / 2);
  auto y = int(getDesktopHeight() / 11);
  auto window_size = FSize{72, getDesktopHeight() * 7 / 8};
  FDialog::setGeometry(FPoint{x, y}, window_size);
  scrolltext.setGeometry (FPoint{1, 2}, FSize{getWidth(), getHeight() - 1});
  FDialog::initLayout();
}

//----------------------------------------------------------------------
void TextWindow::adjustSize()
{
  finalcut::FDialog::adjustSize();

  if ( ! isZoomed() )
  {
    auto desktop_size = FSize{getDesktopWidth(), getDesktopHeight()};
    FRect screen(FPoint{1, 1}, desktop_size);
    bool center = false;

    if ( getWidth() > getDesktopWidth() )
    {
      setWidth(getDesktopWidth() * 9 / 10);
      center = true;
    }

    if ( getHeight() > getDesktopHeight() )
    {
      setHeight(getDesktopHeight() * 7 / 8);
      center = true;
    }

    // Centering the window when it is no longer
    // in the visible area of the terminal
    if ( ! screen.contains(getPos()) || center )
    {
      int x = 1 + int((getDesktopWidth() - getWidth()) / 2);
      int y = 1 + int((getDesktopHeight() - getHeight()) / 2);
      setPos(FPoint{x, y});
    }
  }

  scrolltext.setGeometry (FPoint{1, 2}, FSize(getWidth(), getHeight() - 1));
}

//----------------------------------------------------------------------
auto TextWindow::find_matches ( const finalcut::FString& string
                              , const finalcut::FString& search ) const -> TextWindow::MatchList
{
  MatchList matches{};
  const auto search_length = search.getLength();
  StringPos end = std::wstring::npos;
  StringPos first = 0;
  StringPos last;

  while ( (last = string.toWString().find(search.toWString(), first)) != end )
  {
    matches.emplace_back(last);
    first = last + search_length;
  }

  return matches;
}

//----------------------------------------------------------------------
template <typename... Args>
void TextWindow::highlight ( std::size_t line_number
                           , const finalcut::FString& string
                           , const finalcut::FString& search
                           , Args&&... args )
{
  const auto len = search.getLength();

  for (const auto& found_pos : find_matches(string, search))
  {
    FTextHighlight hgl{found_pos, len, std::forward<Args>(args)...};
    scrolltext.addHighlight (line_number, hgl);
  }
}

//----------------------------------------------------------------------
void TextWindow::highlightText()
{
  std::size_t num{0};  // Line number

  for (const auto& line : scrolltext.getLines())
  {
    highlight (num, line.text, L"GNU", FColorPair{FColor::White, FColor::Red});
    highlight (num, line.text, L"Free Software Foundation", FColor::Green);
    FStyle style;
    style.setStyle (finalcut::Style::Underline + finalcut::Style::Italic);
    highlight (num, line.text, L"GPL", style);
    num++;
  }

  scrolltext.addHighlight
  (
    0,
    FTextHighlight { 19, 33, FColorPair{FColor::Blue, FColor::LightGray}
                           , FStyle(finalcut::Style::Underline) }
  );
}

//----------------------------------------------------------------------
void TextWindow::onAccel (finalcut::FAccelEvent* ev)
{
  close();
  ev->accept();
}

//----------------------------------------------------------------------
void TextWindow::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

// auto main (int argc, char* argv[]) -> int
// {
//   // Create the application object app
//   finalcut::FApplication app{argc, argv};

//   // Force initialization of the terminal without calling show() so that
//   // the color theme is already available at the initialization time
//   app.initTerminal();

//   // Create main dialog object text_window
//   TextWindow text_window{&app};
//   text_window.addAccelerator(finalcut::FKey('q'));

//   // Show the dialog text_window
//   text_window.show();

//   // Set dialog text_window as main widget
//   finalcut::FWidget::setMainWidget(&text_window);

//   // Show and start the application
//   text_window.show();
//   return app.exec();
// }
