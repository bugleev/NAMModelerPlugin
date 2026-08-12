#pragma once

#include <algorithm>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

#include "Colors.h"
#include "IControls.h"
#include "NAMFavoritesStore.h"

using namespace iplug;
using namespace igraphics;

// Always-visible model browser panel (TONEX-style list at the bottom of the UI).
class NAMModelListPanel : public IContainerBase
{
public:
  using LoadFunc = std::function<void(const std::string& path)>;
  using BrowseFunc = std::function<void()>;

  NAMModelListPanel(const IRECT& bounds, const IVStyle& style, const ISVG& starOutlineSVG, const ISVG& starFilledSVG,
                    const ISVG& folderSVG)
  : IContainerBase(bounds)
  , mStyle(style)
  , mStarOutlineSVG(starOutlineSVG)
  , mStarFilledSVG(starFilledSVG)
  , mFolderSVG(folderSVG)
  {
    mIgnoreMouse = false;
  }

  void SetLoadFunc(LoadFunc onLoad) { mOnLoad = std::move(onLoad); }
  void SetBrowseFunc(BrowseFunc onBrowse) { mOnBrowse = std::move(onBrowse); }

  void SetDirectoryFiles(const std::vector<std::string>& directoryFiles, const std::string& selectedPath)
  {
    mDirectoryFiles = directoryFiles;
    mSelectedPath = selectedPath;
    mFavorites.Load();
    RebuildVisibleList();
    UpdateHeaderLabels();
    UpdateEmptyStateControls();
    SetDirty(true);
  }

  void SetSelectedPath(const std::string& selectedPath)
  {
    mSelectedPath = selectedPath;
    SetDirty(false);
  }

  void OnAttached() override
  {
    const float pad = 8.f;
    const float headerH = 26.f;
    const float toolH = 26.f;
    IRECT inner = mRECT.GetPadded(-pad);
    mHeaderBounds = inner.GetFromTop(headerH);
    mToolBounds = inner.GetFromTop(headerH + 4.f + toolH).GetFromBottom(toolH);
    mListBounds = inner.GetReducedFromTop(headerH + toolH + 10.f);
    mEmptyIconBounds = mListBounds.GetCentredInside(56.f, 56.f).GetVShifted(-12.f);

    const auto favBounds = mToolBounds.GetFromRight(92.f);
    const auto searchBounds = mToolBounds.GetReducedFromRight(100.f);

    const IText searchText(14.f, PluginColors::OFF_WHITE, "Roboto-Regular", EAlign::Near, EVAlign::Middle);
    mSearchControl = new SearchFieldControl(searchBounds, searchText, [this](const std::string& q) {
      mSearchQuery = q;
      mScrollOffset = 0.f;
      RebuildVisibleList();
      UpdateHeaderLabels();
      SetDirty(true);
    });
    AddChildControl(mSearchControl);

    mFavToggle = new IVButtonControl(
      favBounds, DefaultClickActionFunc, "Favorites",
      mStyle.WithDrawFrame(true).WithValueText(IText(13.f, PluginColors::OFF_WHITE, "Roboto-Regular")));
    mFavToggle->SetAnimationEndActionFunction([this](IControl* /*pCaller*/) {
      mFavoritesOnly = !mFavoritesOnly;
      mScrollOffset = 0.f;
      RebuildVisibleList();
      UpdateHeaderLabels();
      SetDirty(true);
    });
    AddChildControl(mFavToggle);

    mTitleLabel =
      new IVLabelControl(mHeaderBounds.GetFromLeft(140.f), "MODEL LIST",
                         mStyle.WithDrawFrame(false).WithValueText(
                           IText(15.f, PluginColors::OFF_WHITE, "Roboto-Regular", EAlign::Near)));
    AddChildControl(mTitleLabel);

    mCountLabel =
      new IVLabelControl(mHeaderBounds.GetReducedFromLeft(150.f), "0 models",
                         mStyle.WithDrawFrame(false).WithValueText(
                           IText(13.f, PluginColors::NAM_THEMECOLOR, "Roboto-Regular", EAlign::Near)));
    AddChildControl(mCountLabel);

    mFavorites.Load();
    RebuildVisibleList();
    UpdateHeaderLabels();
    UpdateEmptyStateControls();
    OnResize();
  }

  void Draw(IGraphics& g) override
  {
    g.FillRoundRect(IColor(255, 22, 22, 26), mRECT, 6.f);
    g.DrawRoundRect(PluginColors::NAM_THEMECOLOR.WithOpacity(0.45f), mRECT, 6.f, nullptr, 1.f);

    if (IsEmptyDirectory())
    {
      DrawEmptyState(g);
      return;
    }

    g.PathClipRegion(mListBounds);
    const int first = static_cast<int>(mScrollOffset / kRowHeight);
    const int visibleCount = static_cast<int>(std::ceil(mListBounds.H() / kRowHeight)) + 1;
    const int last = std::min(static_cast<int>(mVisible.size()), first + visibleCount);

    const IText rowText(15.f, PluginColors::OFF_WHITE, "Roboto-Regular", EAlign::Near, EVAlign::Middle);
    const IText selectedText(15.f, COLOR_WHITE, "Roboto-Regular", EAlign::Near, EVAlign::Middle);

    for (int i = first; i < last; ++i)
    {
      const IRECT row = RowBounds(i);
      const auto& path = mVisible[static_cast<size_t>(i)];
      const bool selected = NAMFavoritesStore::NormalizePath(path) == NAMFavoritesStore::NormalizePath(mSelectedPath);
      const bool hovered = (i == mHoverRow);

      if (selected)
        g.FillRect(PluginColors::NAM_THEMECOLOR.WithOpacity(0.45f), row);
      else if (hovered)
        g.FillRect(PluginColors::NAM_THEMECOLOR.WithOpacity(0.18f), row);

      g.DrawLine(IColor(255, 48, 48, 54), row.L, row.B, row.R, row.B);

      const IRECT starBounds = row.GetFromRight(kStarColW).GetCentredInside(16.f, 16.f);
      const IRECT nameBounds = row.GetPadded(-10.f, 0.f, -kStarColW, 0.f);
      g.DrawText(selected ? selectedText : rowText, FileNameFromPath(path).c_str(), nameBounds);

      const bool fav = mFavorites.IsFavorite(path);
      g.DrawSVG(fav ? mStarFilledSVG : mStarOutlineSVG, starBounds);
    }
    g.PathClipRegion(IRECT());

    if (mVisible.size() * kRowHeight > mListBounds.H() + 0.5f)
    {
      const float contentH = static_cast<float>(mVisible.size()) * kRowHeight;
      const float thumbH = std::max(18.f, mListBounds.H() * (mListBounds.H() / contentH));
      const float maxScroll = std::max(0.f, contentH - mListBounds.H());
      const float t = maxScroll > 0.f ? (mScrollOffset / maxScroll) : 0.f;
      const float thumbY = mListBounds.T + t * (mListBounds.H() - thumbH);
      const IRECT thumb(mListBounds.R - 3.f, thumbY, mListBounds.R - 1.f, thumbY + thumbH);
      g.FillRoundRect(PluginColors::NAM_THEMECOLOR.WithOpacity(0.85f), thumb, 1.f);
    }
  }

  void OnMouseOver(float x, float y, const IMouseMod& mod) override
  {
    if (IsEmptyDirectory())
    {
      const bool over = mEmptyIconBounds.Contains(x, y) || mListBounds.Contains(x, y);
      if (over != mEmptyHover)
      {
        mEmptyHover = over;
        SetDirty(false);
      }
      GetUI()->SetMouseCursor(over ? ECursor::HAND : ECursor::ARROW);
      IContainerBase::OnMouseOver(x, y, mod);
      return;
    }

    const int row = HitTestRow(x, y);
    if (row != mHoverRow)
    {
      mHoverRow = row;
      SetDirty(false);
    }
    IContainerBase::OnMouseOver(x, y, mod);
  }

  void OnMouseOut() override
  {
    if (mHoverRow != -1 || mEmptyHover)
    {
      mHoverRow = -1;
      mEmptyHover = false;
      SetDirty(false);
    }
    if (GetUI())
      GetUI()->SetMouseCursor(ECursor::ARROW);
    IContainerBase::OnMouseOut();
  }

  void OnMouseWheel(float x, float y, const IMouseMod& /*mod*/, float d) override
  {
    if (IsEmptyDirectory() || !mRECT.Contains(x, y))
      return;
    mScrollOffset -= d * kRowHeight * 1.5f;
    ClampScroll();
    mHoverRow = HitTestRow(x, y);
    SetDirty(false);
  }

  void OnMouseDown(float x, float y, const IMouseMod& mod) override
  {
    if (IsEmptyDirectory())
    {
      if (mListBounds.Contains(x, y) && mOnBrowse)
        mOnBrowse();
      return;
    }

    if (!mListBounds.Contains(x, y))
    {
      IContainerBase::OnMouseDown(x, y, mod);
      return;
    }

    const int row = HitTestRow(x, y);
    if (row < 0 || row >= static_cast<int>(mVisible.size()))
      return;

    const std::string& path = mVisible[static_cast<size_t>(row)];
    const IRECT starBounds = RowBounds(row).GetFromRight(kStarColW);
    if (starBounds.Contains(x, y))
    {
      mFavorites.Toggle(path);
      if (mFavoritesOnly)
        RebuildVisibleList();
      UpdateHeaderLabels();
      SetDirty(true);
      return;
    }

    mSelectedPath = path;
    SetDirty(true);
    if (mOnLoad)
      mOnLoad(path);
  }

private:
  static constexpr float kRowHeight = 30.f;
  static constexpr float kStarColW = 34.f;

  class SearchFieldControl : public IEditableTextControl
  {
  public:
    using ChangeFunc = std::function<void(const std::string&)>;

    SearchFieldControl(const IRECT& bounds, const IText& text, ChangeFunc onChange)
    : IEditableTextControl(bounds, "Search...", text, IColor(255, 18, 18, 22))
    , mOnChange(std::move(onChange))
    , mPlaceholder(true)
    {
      SetTextEntryLength(128);
    }

    void OnMouseDown(float x, float y, const IMouseMod& mod) override
    {
      if (mPlaceholder)
        SetStr("");
      IEditableTextControl::OnMouseDown(x, y, mod);
    }

    void OnTextEntryCompletion(const char* str, int /*valIdx*/) override
    {
      std::string query = str ? str : "";
      mPlaceholder = query.empty();
      SetStr(mPlaceholder ? "Search..." : query.c_str());
      SetDirty(false);
      if (mOnChange)
        mOnChange(query);
    }

    void Draw(IGraphics& g) override
    {
      g.FillRoundRect(IColor(255, 18, 18, 22), mRECT, 3.f);
      g.DrawRoundRect(PluginColors::NAM_THEMECOLOR.WithOpacity(0.45f), mRECT, 3.f, nullptr, 1.f);
      ITextControl::Draw(g);
    }

  private:
    ChangeFunc mOnChange;
    bool mPlaceholder;
  };

  bool IsEmptyDirectory() const { return mDirectoryFiles.empty(); }

  void DrawEmptyState(IGraphics& g)
  {
    if (mEmptyHover)
      g.FillRoundRect(PluginColors::NAM_THEMECOLOR.WithOpacity(0.12f), mListBounds, 4.f);

    g.DrawSVG(mFolderSVG, mEmptyIconBounds);
    const IRECT hintBounds = mListBounds.GetFromBottom(mListBounds.H() * 0.45f).GetMidVPadded(18.f);
    g.DrawText(IText(15.f, PluginColors::OFF_WHITE, "Roboto-Regular", EAlign::Center, EVAlign::Middle),
               "Click to select a model folder", hintBounds);
  }

  void UpdateEmptyStateControls()
  {
    const bool empty = IsEmptyDirectory();
    if (mSearchControl)
      mSearchControl->Hide(empty);
    if (mFavToggle)
      mFavToggle->Hide(empty);
    if (mCountLabel)
      mCountLabel->Hide(empty);
  }

  static std::string FileNameFromPath(const std::string& path)
  {
    WDL_String w(path.c_str());
    return std::string(w.get_filepart());
  }

  static std::string ToLower(std::string s)
  {
    for (char& c : s)
      c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return s;
  }

  IRECT RowBounds(int index) const
  {
    const float y = mListBounds.T + static_cast<float>(index) * kRowHeight - mScrollOffset;
    return IRECT(mListBounds.L, y, mListBounds.R, y + kRowHeight);
  }

  int HitTestRow(float x, float y) const
  {
    if (!mListBounds.Contains(x, y))
      return -1;
    const int idx = static_cast<int>((y - mListBounds.T + mScrollOffset) / kRowHeight);
    if (idx < 0 || idx >= static_cast<int>(mVisible.size()))
      return -1;
    return idx;
  }

  void ClampScroll()
  {
    const float contentH = static_cast<float>(mVisible.size()) * kRowHeight;
    const float maxScroll = std::max(0.f, contentH - mListBounds.H());
    mScrollOffset = std::clamp(mScrollOffset, 0.f, maxScroll);
  }

  void RebuildVisibleList()
  {
    mVisible.clear();
    const std::string query = ToLower(mSearchQuery);

    auto matchesSearch = [&](const std::string& path) {
      if (query.empty())
        return true;
      return ToLower(FileNameFromPath(path)).find(query) != std::string::npos;
    };

    // Favorites filter only includes starred files from the current directory.
    for (const auto& path : mDirectoryFiles)
    {
      if (mFavoritesOnly && !mFavorites.IsFavorite(path))
        continue;
      if (matchesSearch(path))
        mVisible.push_back(path);
    }
    ClampScroll();
  }

  void UpdateHeaderLabels()
  {
    if (mCountLabel)
    {
      std::string label = std::to_string(mVisible.size()) + (mVisible.size() == 1 ? " model" : " models");
      if (mFavoritesOnly)
        label += " (favorites)";
      mCountLabel->SetStr(label.c_str());
    }
    if (mFavToggle)
      mFavToggle->SetLabelStr(mFavoritesOnly ? "All" : "Favorites");
  }

  IVStyle mStyle;
  ISVG mStarOutlineSVG;
  ISVG mStarFilledSVG;
  ISVG mFolderSVG;
  NAMFavoritesStore mFavorites;
  std::vector<std::string> mDirectoryFiles;
  std::vector<std::string> mVisible;
  std::string mSelectedPath;
  std::string mSearchQuery;
  bool mFavoritesOnly = false;
  float mScrollOffset = 0.f;
  int mHoverRow = -1;
  bool mEmptyHover = false;
  LoadFunc mOnLoad;
  BrowseFunc mOnBrowse;

  IRECT mHeaderBounds;
  IRECT mToolBounds;
  IRECT mListBounds;
  IRECT mEmptyIconBounds;
  SearchFieldControl* mSearchControl = nullptr;
  IVButtonControl* mFavToggle = nullptr;
  IVLabelControl* mTitleLabel = nullptr;
  IVLabelControl* mCountLabel = nullptr;
};
