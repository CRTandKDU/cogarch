/***********************************************************************
* flistview.h - Widget nxpFListView and nxpFListViewItem                     *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2024 Markus Gans                                      *
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

/*  Inheritance diagram
 *  ═══════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▏
 * ▕ FVTerm  ▏ ▕ FObject ▏
 * ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▏
 *      ▲           ▲
 *      │           │
 *      └─────┬─────┘
 *            │
 *       ▕▔▔▔▔▔▔▔▔▔▏           ▕▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWidget ▏           ▕ FObject ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏           ▕▁▁▁▁▁▁▁▁▁▏
 *            ▲                     ▲
 *            │                     │
 *      ▕▔▔▔▔▔▔▔▔▔▔▔▏1     *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏1     1▕▔▔▔▔▔▔▔▏
 *      ▕ FListView ▏- - - -▕ FListViewItem ▏- - - -▕ FData ▏
 *      ▕▁▁▁▁▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▏
 */

#ifndef nxpFLISTVIEW_H
#define nxpFLISTVIEW_H

// #if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
//   #error "Only <final/final.h> can be included directly."
// #endif

#include <iterator>
#include <memory>
#include <stack>
#include <unordered_map>
#include <utility>
#include <vector>

// #include "final/ftypes.h"
// #include "final/fwidget.h"
// #include "final/util/fdata.h"
// #include "final/vterm/fvtermbuffer.h"
// #include "final/widget/fscrollbar.h"

namespace finalcut
{

// class forward declaration
class nxpFListView;
class FScrollbar;
class FString;

//----------------------------------------------------------------------
// class nxpFListViewItem
//----------------------------------------------------------------------

class nxpFListViewItem : public FObject
{
  public:
    // Constructor
    explicit nxpFListViewItem (iterator);
    template <typename DT>
    nxpFListViewItem (const FStringList&, DT&&, iterator);

    // copy constructor
    nxpFListViewItem (const nxpFListViewItem&);

    // Destructor
    ~nxpFListViewItem() override;

    // copy assignment operator (=)
    auto operator = (const nxpFListViewItem&) -> nxpFListViewItem&;

    // Accessors
    auto getClassName() const -> FString override;
    auto getColumnCount() const -> uInt;
    auto getSortColumn() const -> int;
    auto getText (int) const -> FString;
    template <typename DT>
    auto getData() const -> clean_fdata_t<DT>&;
    auto getDepth() const -> uInt;

    // Mutators
    void setText (int, const FString&);
    template <typename DT>
    void setData (DT&&);
    void setCheckable (bool = true);
    void setChecked (bool = true);

    // Inquiry
    auto isChecked() const -> bool;
    auto isExpand() const -> bool;

    // Methods
    auto insert (nxpFListViewItem*) -> iterator;
    auto insert (nxpFListViewItem*, iterator) const -> iterator;
    void remove (nxpFListViewItem*) const;
    void expand();
    void collapse();

  private:
    // Using-declaration
    using FDataAccessPtr = std::shared_ptr<FDataAccess>;

    // Inquiry
    auto isExpandable() const -> bool;
    auto isCheckable() const -> bool;

    // Methods
    template <typename Compare>
    void sort (Compare);
    auto appendItem (nxpFListViewItem*) -> iterator;
    void replaceControlCodes();
    auto getVisibleLines() -> std::size_t;
    void resetVisibleLineCounter();

    // Data members
    FStringList     column_list{};
    FDataAccessPtr  data_pointer{};
    iterator        root{};
    std::size_t     visible_lines{1};
    bool            expandable{false};
    bool            is_expand{false};
    bool            checkable{false};
    bool            is_checked{false};

    // Friend class
    friend class nxpFListView;
    friend class nxpFListViewIterator;
};


// nxpFListViewItem inline functions
//----------------------------------------------------------------------
template <typename DT>
inline nxpFListViewItem::nxpFListViewItem ( const FStringList& cols
                                    , DT&& data
                                    , iterator parent_iter )
  : FObject{nullptr}
  , column_list{cols}
  , data_pointer{makeFData(std::forward<DT>(data))}
{
  if ( cols.empty() )
    return;

  replaceControlCodes();
  insert (this, parent_iter);
}

//----------------------------------------------------------------------
inline auto nxpFListViewItem::getClassName() const -> FString
{ return "nxpFListViewItem"; }

//----------------------------------------------------------------------
inline auto nxpFListViewItem::getColumnCount() const -> uInt
{ return static_cast<uInt>(column_list.size()); }

//----------------------------------------------------------------------
template <typename DT>
inline auto nxpFListViewItem::getData() const -> clean_fdata_t<DT>&
{
  return static_cast<FData<clean_fdata_t<DT>>&>(*data_pointer).get();
}

//----------------------------------------------------------------------
template <typename DT>
inline void nxpFListViewItem::setData (DT&& data)
{
  const auto data_obj = makeFData(std::forward<DT>(data));
  data_pointer = data_obj;
}

//----------------------------------------------------------------------
inline void nxpFListViewItem::setChecked (bool checked)
{ is_checked = checked; }

//----------------------------------------------------------------------
inline auto nxpFListViewItem::isChecked() const -> bool
{ return is_checked; }

//----------------------------------------------------------------------
inline auto nxpFListViewItem::isExpand() const -> bool
{ return is_expand; }

//----------------------------------------------------------------------
inline auto nxpFListViewItem::isExpandable() const -> bool
{ return expandable; }

//----------------------------------------------------------------------
inline auto nxpFListViewItem::isCheckable() const -> bool
{ return checkable; }


//----------------------------------------------------------------------
// class nxpFListViewIterator
//----------------------------------------------------------------------

class nxpFListViewIterator
{
  public:
    // Using-declarations
    using FObjectList   = std::vector<FObject*>;
    using Iterator      = FObjectList::iterator;
    using IteratorStack = std::stack<Iterator>;

    // Constructor
    nxpFListViewIterator () = default;
    ~nxpFListViewIterator () = default;
    explicit nxpFListViewIterator (Iterator);
    nxpFListViewIterator (const nxpFListViewIterator&) = default;
    nxpFListViewIterator (nxpFListViewIterator&& i) noexcept
      : iter_path{std::move(i.iter_path)}
      , node{i.node}
      , position{i.position}
    { }

    // Overloaded operators
    auto operator = (const nxpFListViewIterator&) -> nxpFListViewIterator& = default;
    auto operator = (nxpFListViewIterator&&) noexcept -> nxpFListViewIterator& = default;
    auto operator = (Iterator iter) -> nxpFListViewIterator&;
    auto operator ++ () -> nxpFListViewIterator&;    // prefix
    auto operator ++ (int) -> nxpFListViewIterator;  // postfix
    auto operator -- () -> nxpFListViewIterator&;    // prefix
    auto operator -- (int) -> nxpFListViewIterator;  // postfix
    auto operator += (int) -> nxpFListViewIterator&;
    auto operator -= (int) -> nxpFListViewIterator&;
    auto operator * () const -> FObject*&;
    auto operator -> () const -> FObject*;

    friend inline auto operator == ( const nxpFListViewIterator& lhs
                                   , const nxpFListViewIterator& rhs ) -> bool
    {
      return lhs.node == rhs.node;
    }

    friend inline auto operator != ( const nxpFListViewIterator& lhs
                                   , const nxpFListViewIterator& rhs ) -> bool
    {
      return lhs.node != rhs.node;
    }

    friend inline auto operator == ( const nxpFListViewIterator& listview_iter
                                   , const Iterator& iter) -> bool
    {
      return listview_iter.node == iter;
    }

    friend inline auto operator != ( const nxpFListViewIterator& listview_iter
                                   , const Iterator& iter) -> bool
    {
      return listview_iter.node != iter;
    }

    // Accessor
    auto getClassName() const -> FString;
    auto getPosition() -> int&;

    // Methods
    void parentElement();

    // Friend Non-member operator functions
    friend auto operator + (const nxpFListViewIterator& lhs, int n) -> nxpFListViewIterator
    {
      auto tmp = lhs;

      for (int i = n; i > 0 ; i--)
        tmp.nextElement(tmp.node);

      return tmp;
    }

    friend auto operator - (const nxpFListViewIterator& lhs, int n) -> nxpFListViewIterator
    {
      auto tmp = lhs;

      for (int i = n; i > 0 ; i--)
        tmp.prevElement(tmp.node);

      return tmp;
    }

  private:
    // Methods
    void nextElement (Iterator&);
    void prevElement (Iterator&);

    // Data members
    IteratorStack  iter_path{};
    Iterator       node{};
    int            position{0};
};


// nxpFListViewIterator inline functions
//----------------------------------------------------------------------
inline auto nxpFListViewIterator::operator * () const -> FObject*&
{ return *node; }

//----------------------------------------------------------------------
inline auto nxpFListViewIterator::operator -> () const -> FObject*
{ return *node; }

//----------------------------------------------------------------------
inline auto nxpFListViewIterator::getClassName() const -> FString
{ return "nxpFListViewIterator"; }

//----------------------------------------------------------------------
inline auto nxpFListViewIterator::getPosition() -> int&
{ return position; }


//----------------------------------------------------------------------
// class nxpFListView
//----------------------------------------------------------------------

class nxpFListView : public FWidget
{
  public:
    // Using-declaration
    using FWidget::setGeometry;
    using nxpFListViewItems = std::vector<nxpFListViewItem*>;

    // Disable copy constructor
    nxpFListView (const nxpFListView&) = delete;

    // Disable move constructor
    nxpFListView (nxpFListView&&) noexcept = delete;

    // Constructor
    explicit nxpFListView (FWidget* = nullptr);

    // Destructor
    ~nxpFListView() override;

    // Disable copy assignment operator (=)
    auto operator = (const nxpFListView&) -> nxpFListView& = delete;

    // Disable move assignment operator (=)
    auto operator = (nxpFListView&&) noexcept -> nxpFListView& = delete;

  // Tuesday, July 22, 2025
  void drawListLine (const nxpFListViewItem*, bool, bool);  

    // Accessors
    auto getClassName() const -> FString override;
    auto getCount() const -> std::size_t;
    auto getColumnCount() const -> std::size_t;
    auto getColumnAlignment (int) const -> Align;
    auto getColumnText (int) const -> FString;
    auto getColumnSortType (int) const -> SortType;
    auto getSortOrder() const -> SortOrder;
    auto getSortColumn() const -> int;
    auto getCurrentItem() -> nxpFListViewItem*;

    // Mutators
    void setSize (const FSize&, bool = true) override;
    void setGeometry ( const FPoint&, const FSize&
                     , bool = true ) override;
    void setColumnAlignment (int, Align);
    void setColumnText (int, const FString&);
    void setColumnSortType (int, SortType = SortType::Name);
    void setColumnSort (int, SortOrder = SortOrder::Ascending);
    template <typename Compare>
    void setUserAscendingCompare (Compare);
    template <typename Compare>
    void setUserDescendingCompare (Compare);
    void hideSortIndicator (bool = true);
    void showColumn (int);
    void hideColumn (int);
    void setTreeView (bool = true);
    void unsetTreeView();

    // Inquiries
    auto isColumnHidden (int) const -> bool;

    // Methods
    virtual auto addColumn (const FString&, int = USE_MAX_SIZE) -> int;
    virtual auto removeColumn (int) -> int;
    void removeAllColumns();
    void hide() override;
    auto insert (nxpFListViewItem*) -> iterator;
    auto insert (nxpFListViewItem*, iterator) -> iterator;
    template <typename DT = std::nullptr_t>
    auto insert (const FStringList&, DT&& = DT()) -> iterator;
    auto insert (const FStringList&, iterator) -> iterator;
    template <typename DT>
    auto insert (const FStringList&, DT&&, iterator) -> iterator;
    template <typename T
            , typename DT = std::nullptr_t>
    auto insert (const std::initializer_list<T>&, DT&& = DT()) -> iterator;
    template <typename T>
    auto insert (const std::initializer_list<T>&, iterator) -> iterator;
    template <typename T
            , typename DT>
    auto insert (const std::initializer_list<T>&, DT&&, iterator) -> iterator;
    template <typename ColT
            , typename DT = std::nullptr_t>
    auto insert (const std::vector<ColT>&, DT&& = DT()) -> iterator;
    template <typename ColT>
    auto insert (const std::vector<ColT>&, iterator) -> iterator;
    template <typename ColT
            , typename DT>
    auto insert (const std::vector<ColT>&, DT&&, iterator) -> iterator;
    void remove (nxpFListViewItem*);
    void clear();
    auto getData() & -> nxpFListViewItems&;
    auto getData() const & -> const nxpFListViewItems&;

    virtual void sort();

    // Event handlers
    void onKeyPress (FKeyEvent*) override;
    void onMouseDown (FMouseEvent*) override;
    void onMouseUp (FMouseEvent*) override;
    void onMouseMove (FMouseEvent*) override;
    void onMouseDoubleClick (FMouseEvent*) override;
    void onWheel (FWheelEvent*) override;
    void onTimer (FTimerEvent*) override;
    void onFocusOut (FFocusEvent*) override;

  protected:
    // Methods
    void initLayout() override;
    void adjustViewport (const int);
    void adjustScrollbars (const std::size_t) const;
    void adjustSize() override;

  private:
    struct Header;  // forward declaration

    // Using-declaration
    using KeyMap = std::unordered_map<FKey, std::function<void()>, EnumHash<FKey>>;
    using KeyMapResult = std::unordered_map<FKey, std::function<bool()>, EnumHash<FKey>>;
    using HeaderItems = std::vector<Header>;
    using SortTypes = std::vector<SortType>;

    struct ListViewData
    {
      iterator      root{};
      FObjectList   selflist{};
      FObjectList   itemlist{};
      HeaderItems   header;  // GitHub issues #122
      FVTermBuffer  headerline{};
      KeyMap        key_map{};
      KeyMapResult  key_map_result{};
    };

    struct SelectionState
    {
      nxpFListViewIterator     current_iter{};
      const nxpFListViewItem*  clicked_checkbox_item{nullptr};
      FPoint                clicked_expander_pos{-1, -1};
      FPoint                clicked_header_pos{-1, -1};
    };

    struct SortState
    {
      int        column{-1};
      SortTypes  type{};
      SortOrder  order{SortOrder::Unsorted};
      bool       hide_sort_indicator{false};
    };

    struct ScrollingState
    {
      FScrollbarPtr      vbar{nullptr};
      FScrollbarPtr      hbar{nullptr};
      nxpFListViewIterator  first_visible_line{};
      nxpFListViewIterator  last_visible_line{};
      int                first_line_position_before{-1};
      int                xoffset{0};
      bool               timer{false};
      int                repeat{100};
      int                distance{1};
    };

    // Constants
    static constexpr std::size_t checkbox_space = 4;

    // Constants
    static constexpr int USE_MAX_SIZE = -1;

    // Accessors
    static auto getNullIterator() -> iterator&;

    // Mutators
    static void setNullIterator (const iterator&);

    // Inquiry
    auto isHorizontallyScrollable() const -> bool;
    auto isVerticallyScrollable() const -> bool;
    auto canSkipListDrawing() const -> bool;
    auto canSkipDragScrolling() -> bool;

    // Methods
    void init();
    void mapKeyFunctions();
    void processKeyAction (FKeyEvent*);
    template <typename Compare>
    void sort (Compare);
    auto getAlignOffset ( const Align
                        , const std::size_t
                        , const std::size_t ) const -> std::size_t;
    auto getListEnd (const nxpFListViewItem*) -> iterator;
    void draw() override;
    void drawBorder() override;
    void drawScrollbars() const;
    void drawHeadlines();
    void drawList();
    void setInputCursor (const nxpFListViewItem*, int, bool);
    void finalizeListDrawing (int);
    void adjustWidthForTreeView (std::size_t&, std::size_t, bool) const;
    // void drawListLine (const nxpFListViewItem*, bool, bool);
    auto createColumnsString (const nxpFListViewItem*) -> FString;
    void printColumnsString (FString&);
    void clearList();
    void setLineAttributes (bool, bool) const;
    auto getCheckBox (const nxpFListViewItem* item) const -> FString;
    auto getLinePrefix (const nxpFListViewItem*, std::size_t) const -> FString;
    void drawSortIndicator (std::size_t&, std::size_t);
    void drawHeadlineLabel (const HeaderItems::const_iterator&);
    void drawHeaderBorder (std::size_t);
    auto findHeaderStartPos (bool&) -> FVTermBuffer::iterator;
    auto findHeaderEndPos (FVTermBuffer::iterator, bool, bool&) -> FVTermBuffer::iterator;
    void drawBufferedHeadline();
    void drawColumnEllipsis ( const HeaderItems::const_iterator&
                            , const FString& );
    void updateLayout();
    void updateDrawing (bool, bool);
    auto determineLineWidth (nxpFListViewItem*) -> std::size_t;
    void beforeInsertion (nxpFListViewItem*);
    void afterInsertion();
    void adjustListBeforeRemoval (const nxpFListViewItem*);
    void removeItemFromParent (nxpFListViewItem*);
    void updateListAfterRemoval();
    void recalculateHorizontalBar (std::size_t);
    void recalculateVerticalBar (std::size_t) const;
    void mouseHeaderClicked();
    auto getHeaderClickWidth (const Header&, int) const -> int;
    auto isPositionWithinHeader (int, int, int) const -> bool;
    void handleColumnSort (int);
    void handleTreeExpanderClick (const FMouseEvent*);
    void handleCheckboxClick (const FMouseEvent*);
    void wheelUp (int);
    void wheelDown (int);
    void wheelLeft (int);
    void wheelRight (int);
    auto dragScrollUp (int) -> bool;
    auto dragScrollDown (int) -> bool;
    void dragUp (MouseButton);
    void dragDown (MouseButton);
    void stopDragScroll();
    void toggleItemExpandState (nxpFListViewItem*) const;
    void toggleItemCheckState (nxpFListViewItem*) const;
    auto isCheckboxClicked (int, int) const -> bool;
    void resetClickedPositions();
    auto isWithinHeaderBounds (const FPoint&) const -> bool;
    auto isWithinListBounds (const FPoint&) const -> bool;
    auto appendItem (nxpFListViewItem*) -> iterator;
    void handleListEvent (const FMouseEvent*);
    void handleTreeViewEvents (const FMouseEvent*, const nxpFListViewItem*);
    void handleCheckableItemsEvents (const FMouseEvent*, const nxpFListViewItem*);
    void processClick() const;
    void processRowChanged() const;
    void processChanged() const;
    void changeOnResize() const;
    void toggleCheckbox();
    void collapseAndScrollLeft();
    void jumpToParentElement (const nxpFListViewItem*);
    void expandAndScrollRight();
    void firstPos();
    void lastPos();
    auto expandSubtree() -> bool;
    auto collapseSubtree() -> bool;
    void setRelativePosition (int);
    void stepForward();
    void stepBackward();
    void stepForward (int);
    void stepBackward (int);
    void scrollToX (int);
    void scrollToY (int);
    void scrollTo (const FPoint&);
    void scrollTo (int, int);
    void scrollBy (int, int);
    auto isItemListEmpty() const -> bool;
    auto isTreeView() const -> bool;
    auto isColumnIndexInvalid (int) const -> bool;
    auto hasCheckableItems() const -> bool;
    auto getScrollBarMaxHorizontal() const noexcept -> int;
    auto getScrollBarMaxVertical (const std::size_t) const noexcept -> int;
    void updateViewAfterVBarChange (const FScrollbar::ScrollType);
    void updateViewAfterHBarChange (const FScrollbar::ScrollType, const int);
    auto getVerticalScrollDistance (const FScrollbar::ScrollType) const -> int;
    auto getHorizontalScrollDistance (const FScrollbar::ScrollType) const -> int;

    // Callback methods
    void cb_vbarChange (const FWidget*);
    void cb_hbarChange (const FWidget*);

    // Data members
    std::size_t     nf_offset{0};
    std::size_t     max_line_width{1};
    bool            tree_view{false};
    bool            has_checkable_items{false};
    ListViewData    data{};
    SortState       sorting{};
    ScrollingState  scroll{};
    SelectionState  selection{};
    DragScrollMode  drag_scroll{DragScrollMode::None};

    // Function Pointer
    bool (*user_defined_ascending) (const FObject*, const FObject*){nullptr};
    bool (*user_defined_descending) (const FObject*, const FObject*){nullptr};

    // Friend class
    friend class nxpFListViewItem;
};


//----------------------------------------------------------------------
// struct nxpFListView::Header
//----------------------------------------------------------------------

struct nxpFListView::Header
{
  public:
    Header() = default;

    FString name{};
    Align   alignment{Align::Left};
    int     width{0};
    bool    fixed_width{false};
    bool    visible{true};
};


// nxpFListView inline functions
//----------------------------------------------------------------------
inline auto nxpFListView::getClassName() const -> FString
{ return "nxpFListView"; }

//----------------------------------------------------------------------
inline auto nxpFListView::getSortOrder() const -> SortOrder
{ return sorting.order; }

//----------------------------------------------------------------------
inline auto nxpFListView::getSortColumn() const -> int
{ return sorting.column; }

//----------------------------------------------------------------------
inline auto nxpFListView::getCurrentItem() -> nxpFListViewItem*
{ return static_cast<nxpFListViewItem*>(*selection.current_iter); }

//----------------------------------------------------------------------
template <typename Compare>
inline void nxpFListView::setUserAscendingCompare (Compare cmp)
{ user_defined_ascending = cmp; }

//----------------------------------------------------------------------
template <typename Compare>
inline void nxpFListView::setUserDescendingCompare (Compare cmp)
{ user_defined_descending = cmp; }

//----------------------------------------------------------------------
inline void nxpFListView::hideSortIndicator (bool hide)
{ sorting.hide_sort_indicator = hide; }

//----------------------------------------------------------------------
inline void nxpFListView::setTreeView (bool enable)
{ tree_view = enable; }

//----------------------------------------------------------------------
inline void nxpFListView::unsetTreeView()
{ setTreeView(false); }

//----------------------------------------------------------------------
inline auto nxpFListView::insert (nxpFListViewItem* item) -> FObject::iterator
{ return insert (item, data.root); }

//----------------------------------------------------------------------
template <typename DT>
inline auto
    nxpFListView::insert (const FStringList& cols, DT&& d) -> FObject::iterator
{ return insert (cols, std::forward<DT>(d), data.root); }

//----------------------------------------------------------------------
inline auto
    nxpFListView::insert ( const FStringList& cols
                      , iterator parent_iter ) -> FObject::iterator
{ return insert (cols, nullptr, parent_iter); }

//----------------------------------------------------------------------
template <typename DT>
inline auto nxpFListView::insert ( const FStringList& cols
                              , DT&& d
                              , iterator parent_iter ) -> FObject::iterator
{
  nxpFListViewItem* item;

  if ( cols.empty() || parent_iter == getNullIterator() )
    return getNullIterator();

  if ( ! *parent_iter )
    parent_iter = data.root;

  try
  {
    item = new nxpFListViewItem (cols, std::forward<DT>(d), getNullIterator());
  }
  catch (const std::bad_alloc&)
  {
    badAllocOutput ("nxpFListViewItem");
    return getNullIterator();
  }

  item->replaceControlCodes();
  return insert(item, parent_iter);
}

//----------------------------------------------------------------------
template <typename T
        , typename DT>
inline auto
    nxpFListView::insert (const std::initializer_list<T>& list, DT&& d) -> FObject::iterator
{ return insert (list, std::forward<DT>(d), data.root); }

//----------------------------------------------------------------------
template <typename T>
inline auto
    nxpFListView::insert ( const std::initializer_list<T>& list
                      , iterator parent_iter ) -> FObject::iterator
{ return insert (list, 0, parent_iter); }

//----------------------------------------------------------------------
template <typename T
        , typename DT>
auto nxpFListView::insert ( const std::initializer_list<T>& list
                       , DT&& d
                       , iterator parent_iter ) -> FObject::iterator
{
  FStringList str_cols;

  std::transform ( std::begin(list)
                 , std::end(list)
                 , std::back_inserter(str_cols)
                 , [] (const auto& col)
                   {
                     const FString s(FString() << col);
                     return s;
                   }
                 );

  auto item_iter = insert (str_cols, std::forward<DT>(d), parent_iter);
  return item_iter;
}

//----------------------------------------------------------------------
template <typename ColT
        , typename DT>
inline auto
    nxpFListView::insert (const std::vector<ColT>& cols, DT&& d) -> FObject::iterator
{ return insert (cols, std::forward<DT>(d), data.root); }

//----------------------------------------------------------------------
template <typename ColT>
inline auto
    nxpFListView::insert ( const std::vector<ColT>& cols
                      , iterator parent_iter ) -> FObject::iterator
{ return insert (cols, 0, parent_iter); }

//----------------------------------------------------------------------
template <typename ColT
        , typename DT>
auto
    nxpFListView::insert ( const std::vector<ColT>& cols
                      , DT&& d
                      , iterator parent_iter ) -> FObject::iterator
{
  FStringList str_cols;

  std::transform ( std::begin(cols)
                 , std::end(cols)
                 , std::back_inserter(str_cols)
                 , [] (const auto& col)
                   {
                     const FString s(FString() << col);
                     return s;
                   }
                 );

  auto item_iter = insert (str_cols, std::forward<DT>(d), parent_iter);
  return item_iter;
}

//----------------------------------------------------------------------
inline auto nxpFListView::getData() & -> nxpFListViewItems&
{
  FObjectList* ptr = &data.itemlist;
  return *static_cast<nxpFListViewItems*>(static_cast<void*>(ptr));
}

//----------------------------------------------------------------------
inline auto nxpFListView::getData() const & -> const nxpFListViewItems&
{
  const FObjectList* ptr = &data.itemlist;
  return *static_cast<const nxpFListViewItems*>(static_cast<const void*>(ptr));
}

//----------------------------------------------------------------------
inline auto nxpFListView::isHorizontallyScrollable() const -> bool
{ return max_line_width > getClientWidth(); }

//----------------------------------------------------------------------
inline auto nxpFListView::isVerticallyScrollable() const -> bool
{ return getCount() > getClientHeight(); }

//----------------------------------------------------------------------
inline auto nxpFListView::canSkipListDrawing() const -> bool
{ return isItemListEmpty() || getHeight() <= 2 || getWidth() <= 4; }

//----------------------------------------------------------------------
inline auto nxpFListView::getColumnCount() const -> std::size_t
{ return data.header.size(); }

//----------------------------------------------------------------------
inline void nxpFListView::toggleItemCheckState (nxpFListViewItem* item) const
{ item->setChecked(! item->isChecked()); }

//----------------------------------------------------------------------
inline void nxpFListView::scrollTo (const FPoint& pos)
{ scrollTo(pos.getX(), pos.getY()); }

//----------------------------------------------------------------------
inline auto nxpFListView::isItemListEmpty() const -> bool
{ return data.itemlist.empty(); }

//----------------------------------------------------------------------
inline auto nxpFListView::isTreeView() const -> bool
{ return tree_view; }

//----------------------------------------------------------------------
inline auto nxpFListView::isColumnIndexInvalid (int column) const -> bool
{
  return column < 1 || data.header.empty() || column > int(data.header.size());
}

//----------------------------------------------------------------------
inline auto nxpFListView::hasCheckableItems() const -> bool
{ return has_checkable_items; }

}  // namespace finalcut

#endif  // FLISTVIEW_H
