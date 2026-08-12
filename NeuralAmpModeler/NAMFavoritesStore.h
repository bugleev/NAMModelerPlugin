#pragma once

#include <string>
#include <vector>

// Persists favorite NAM model absolute paths under the user's LocalAppData.
class NAMFavoritesStore
{
public:
  void Load();
  void Save() const;

  bool IsFavorite(const std::string& path) const;
  // Returns true if the path is a favorite after the toggle.
  bool Toggle(const std::string& path);

  const std::vector<std::string>& GetAll() const { return mFavorites; }

  static std::string NormalizePath(const std::string& path);

private:
  static std::string GetStorePath();
  static bool PathsEqual(const std::string& a, const std::string& b);

  std::vector<std::string> mFavorites;
};
