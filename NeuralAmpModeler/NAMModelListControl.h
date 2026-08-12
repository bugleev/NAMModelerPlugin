#pragma once

#include <algorithm>
#include <cctype>
#include <cmath>
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

  void SetDirectoryFiles(const std::vector<std::string>& directoryFiles, const std::string& selectedPath,
                         const std::string& directoryPath = "")
  {
    mDirectoryFiles = directoryFiles;
    mSelectedPath = selectedPath;
    mDirectoryPath = directoryPath.empty() ? DeriveDirectoryPath(directoryFiles, selectedPath) : directoryPath;
    mFavorites.Load();
    RebuildVisibleList();
    UpdateToolbar();
    SetDirty(true);
  }

  void SetSelectedPath(const std::string& selectedPath)
  {
    mSelectedPath = selectedPath;
    SetDirty(false);
  }

  void OnAttached() override
  {
    LayoutBounds();

    const auto favBounds = mToolBounds.GetFromRight(kFavBtnW);
    mFavToggle = new IVButtonControl(
      favBounds, DefaultClickActionFunc, "Favorites",
      mStyle.WithDrawFrame(true).WithValueText(IText(12.f, PluginColors::OFF_WHITE, "Roboto-Regular")));
    mFavToggle->SetAnimationEndActionFunction([this](IControl* /*pCaller*/) {
      mFavoritesOnly = !mFavoritesOnly;
      mScrollOffset = 0.f;
      RebuildVisibleList();
      UpdateToolbar();
      SetDirty(true);
    });
    AddChildControl(mFavToggle);

    mCountLabel =
      new IVLabelControl(mCountBounds, "0 models",
                         mStyle.WithDrawFrame(false).WithValueText(
                           IText(12.f, PluginColors::NAM_THEMECOLOR, "Roboto-Regular", EAlign::Far, EVAlign::Middle)));
    AddChildControl(mCountLabel);

    mFolderNameLabel =
      new IVLabelControl(mFolderNameBounds, "No folder selected",
                         mStyle.WithDrawFrame(false).WithValueText(
                           IText(13.f, PluginColors::OFF_WHITE, "Roboto-Regular", EAlign::Near, EVAlign::Middle)));
    AddChildControl(mFolderNameLabel);

    mFavorites.Load();
    RebuildVisibleList();
    UpdateToolbar();
  }

  void Draw(IGraphics& g) override
  {
    g.FillRoundRect(IColor(255, 22, 22, 26), mRECT, 6.f);
    g.DrawRoundRect(PluginColors::NAM_THEMECOLOR.WithOpacity(0.45f), mRECT, 6.f, nullptr, 1.f);

    // Compact toolbar: folder icon | folder name | count | favorites
    if (mFolderHover)
      g.FillRoundRect(PluginColors::NAM_THEMECOLOR.WithOpacity(0.18f), mFolderIconBounds.GetPadded(2.f), 3.f);
    g.DrawSVG(mFolderSVG, mFolderIconBounds);

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
    const bool overFolder = mFolderIconBounds.Contains(x, y);
    if (overFolder != mFolderHover)
    {
      mFolderHover = overFolder;
      SetDirty(false);
    }

    if (overFolder)
    {
      GetUI()->SetMouseCursor(ECursor::HAND);
      IContainerBase::OnMouseOver(x, y, mod);
      return;
    }

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
    GetUI()->SetMouseCursor(ECursor::ARROW);
    IContainerBase::OnMouseOver(x, y, mod);
  }

  void OnMouseOut() override
  {
    if (mHoverRow != -1 || mEmptyHover || mFolderHover)
    {
      mHoverRow = -1;
      mEmptyHover = false;
      mFolderHover = false;
      SetDirty(false);
    }
    if (GetUI())
      GetUI()->SetMouseCursor(ECursor::ARROW);
    IContainerBase::OnMouseOut();
  }

  void OnMouseWheel(float x, float y, const IMouseMod& /*mod*/, float d) override
  {
    if (IsEmptyDirectory() || !mListBounds.Contains(x, y))
      return;
    mScrollOffset -= d * kRowHeight * 1.5f;
    ClampScroll();
    mHoverRow = HitTestRow(x, y);
    SetDirty(false);
  }

  void OnMouseDown(float x, float y, const IMouseMod& mod) override
  {
    if (mFolderIconBounds.Contains(x, y))
    {
      if (mOnBrowse)
        mOnBrowse();
      return;
    }

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
      UpdateToolbar();
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
  static constexpr float kToolH = 22.f;
  static constexpr float kFavBtnW = 84.f;
  static constexpr float kCountW = 88.f;
  static constexpr float kFolderIconSize = 18.f;

  bool IsEmptyDirectory() const { return mDirectoryFiles.empty(); }

  void LayoutBounds()
  {
    const float pad = 6.f;
    IRECT inner = mRECT.GetPadded(-pad);
    mToolBounds = inner.GetFromTop(kToolH);
    mListBounds = inner.GetReducedFromTop(kToolH + 6.f);
    mEmptyIconBounds = mListBounds.GetCentredInside(56.f, 56.f).GetVShifted(-12.f);

    mFolderIconBounds = mToolBounds.GetFromLeft(kFolderIconSize + 4.f).GetCentredInside(kFolderIconSize, kFolderIconSize);
    const auto favBounds = mToolBounds.GetFromRight(kFavBtnW);
    mCountBounds = IRECT(favBounds.L - kCountW - 4.f, mToolBounds.T, favBounds.L - 4.f, mToolBounds.B);
    mFolderNameBounds = IRECT(mFolderIconBounds.R + 8.f, mToolBounds.T, mCountBounds.L - 6.f, mToolBounds.B);
  }

  void DrawEmptyState(IGraphics& g)
  {
    if (mEmptyHover)
      g.FillRoundRect(PluginColors::NAM_THEMECOLOR.WithOpacity(0.12f), mListBounds, 4.f);

    g.DrawSVG(mFolderSVG, mEmptyIconBounds);
    const IRECT hintBounds = mListBounds.GetFromBottom(mListBounds.H() * 0.45f).GetMidVPadded(18.f);
    g.DrawText(IText(15.f, PluginColors::OFF_WHITE, "Roboto-Regular", EAlign::Center, EVAlign::Middle),
               "Click to select a model folder", hintBounds);
  }

  void UpdateToolbar()
  {
    if (mFolderNameLabel)
      mFolderNameLabel->SetStr(FolderDisplayName().c_str());
    if (mCountLabel)
    {
      std::string label = std::to_string(mVisible.size()) + (mVisible.size() == 1 ? " model" : " models");
      if (mFavoritesOnly)
        label += " (fav)";
      mCountLabel->SetStr(label.c_str());
      mCountLabel->Hide(IsEmptyDirectory());
    }
    if (mFavToggle)
    {
      mFavToggle->Hide(IsEmptyDirectory());
      mFavToggle->SetLabelStr(mFavoritesOnly ? "All" : "Favorites");
    }
  }

  std::string FolderDisplayName() const
  {
    if (mDirectoryPath.empty())
      return "No folder selected";
    WDL_String w(mDirectoryPath.c_str());
    // Strip trailing separators so get_filepart returns the folder name.
    while (w.GetLength() > 1)
    {
      const char last = w.Get()[w.GetLength() - 1];
      if (last == '/' || last == '\\')
        w.SetLen(w.GetLength() - 1);
      else
        break;
    }
    const char* part = w.get_filepart();
    if (part && *part)
      return std::string(part);
    return mDirectoryPath;
  }

  static std::string DeriveDirectoryPath(const std::vector<std::string>& files, const std::string& selectedPath)
  {
    const std::string& src = !selectedPath.empty() ? selectedPath : (files.empty() ? std::string() : files.front());
    if (src.empty())
      return {};
    WDL_String w(src.c_str());
    w.remove_filepart(true);
    return std::string(w.Get());
  }

  static std::string FileNameFromPath(const std::string& path)
  {
    WDL_String w(path.c_str());
    return std::string(w.get_filepart());
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
    for (const auto& path : mDirectoryFiles)
    {
      if (mFavoritesOnly && !mFavorites.IsFavorite(path))
        continue;
      mVisible.push_back(path);
    }
    ClampScroll();
  }

  IVStyle mStyle;
  ISVG mStarOutlineSVG;
  ISVG mStarFilledSVG;
  ISVG mFolderSVG;
  NAMFavoritesStore mFavorites;
  std::vector<std::string> mDirectoryFiles;
  std::vector<std::string> mVisible;
  std::string mSelectedPath;
  std::string mDirectoryPath;
  bool mFavoritesOnly = false;
  float mScrollOffset = 0.f;
  int mHoverRow = -1;
  bool mEmptyHover = false;
  bool mFolderHover = false;
  LoadFunc mOnLoad;
  BrowseFunc mOnBrowse;

  IRECT mToolBounds;
  IRECT mListBounds;
  IRECT mEmptyIconBounds;
  IRECT mFolderIconBounds;
  IRECT mFolderNameBounds;
  IRECT mCountBounds;
  IVButtonControl* mFavToggle = nullptr;
  IVLabelControl* mFolderNameLabel = nullptr;
  IVLabelControl* mCountLabel = nullptr;
};
