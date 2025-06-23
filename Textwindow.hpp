#ifndef TEXTWINDOW_HPP
#define TEXTWINDOW_HPP

#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include <final/final.h>

using finalcut::FPoint;
using finalcut::FRect;
using finalcut::FSize;
using finalcut::FStyle;
using FTextHighlight = finalcut::FTextView::FTextHighlight;

//----------------------------------------------------------------------
// class TextWindow
//----------------------------------------------------------------------

class TextWindow final : public finalcut::FDialog
{
  public:
    // Constructor
    explicit TextWindow (finalcut::FWidget* = nullptr);
    // Data members
    finalcut::FTextView scrolltext{this};

  private:
    // Using-declarations
    using MatchList = std::vector<std::size_t>;
    using StringPos = std::wstring::size_type;

    // Method
    void initLayout() override;
    void adjustSize() override;
    auto find_matches ( const finalcut::FString&
                      , const finalcut::FString& ) const -> MatchList;
    template <typename... Args>
    void highlight ( std::size_t
                   , const finalcut::FString&
                   , const finalcut::FString&
                   , Args&&... );
    void highlightText();

    // Event handlers
    void onAccel (finalcut::FAccelEvent*) override;
    void onClose (finalcut::FCloseEvent*) override;

};

#endif
