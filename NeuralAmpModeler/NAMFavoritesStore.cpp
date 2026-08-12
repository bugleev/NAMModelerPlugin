#include "NAMFavoritesStore.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <filesystem>

#include "IPlugPaths.h"
#include "json.hpp"

namespace
{
std::string ToLowerAscii(std::string s)
{
  for (char& c : s)
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  return s;
}
} // namespace

std::string NAMFavoritesStore::NormalizePath(const std::string& path)
{
  std::string normalized = path;
  for (char& c : normalized)
  {
    if (c == '/')
      c = '\\';
  }
  // Windows paths are case-insensitive for identity.
  return ToLowerAscii(normalized);
}

bool NAMFavoritesStore::PathsEqual(const std::string& a, const std::string& b)
{
  return NormalizePath(a) == NormalizePath(b);
}

std::string NAMFavoritesStore::GetStorePath()
{
  WDL_String dir;
  iplug::INIPath(dir, "NeuralAmpModeler");
  std::filesystem::path folder(dir.Get());
  std::error_code ec;
  std::filesystem::create_directories(folder, ec);
  folder /= "favorites.json";
  return folder.string();
}

void NAMFavoritesStore::Load()
{
  mFavorites.clear();
  const std::string path = GetStorePath();
  std::ifstream in(path);
  if (!in)
    return;

  try
  {
    nlohmann::json j;
    in >> j;
    if (!j.contains("favorites") || !j["favorites"].is_array())
      return;

    for (const auto& item : j["favorites"])
    {
      if (!item.is_string())
        continue;
      const std::string fav = item.get<std::string>();
      if (fav.empty())
        continue;
      // Deduplicate while preserving first-seen order.
      bool exists = false;
      for (const auto& existing : mFavorites)
      {
        if (PathsEqual(existing, fav))
        {
          exists = true;
          break;
        }
      }
      if (!exists)
        mFavorites.push_back(fav);
    }
  }
  catch (...)
  {
    mFavorites.clear();
  }
}

void NAMFavoritesStore::Save() const
{
  nlohmann::json j;
  j["favorites"] = mFavorites;
  const std::string path = GetStorePath();
  std::ofstream out(path);
  if (!out)
    return;
  out << j.dump(2);
}

bool NAMFavoritesStore::IsFavorite(const std::string& path) const
{
  for (const auto& fav : mFavorites)
  {
    if (PathsEqual(fav, path))
      return true;
  }
  return false;
}

bool NAMFavoritesStore::Toggle(const std::string& path)
{
  if (path.empty())
    return false;

  for (auto it = mFavorites.begin(); it != mFavorites.end(); ++it)
  {
    if (PathsEqual(*it, path))
    {
      mFavorites.erase(it);
      Save();
      return false;
    }
  }

  mFavorites.push_back(path);
  Save();
  return true;
}
