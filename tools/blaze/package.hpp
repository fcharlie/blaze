#ifndef BLAZE_PACKAGE_HPP
#define BLAZE_PACKAGE_HPP
#include <string>
#include <unordered_map>

inline bool IsAlphaNum(int c) {
  return ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') ||
          (c >= 'a' && c <= 'z'));
}

class Semver {
public:
  enum {
    kVersionMajor,
    kVersionMinor,
    kVersionPatch,
    kVersionPreRelease,
    kVersionPreReleaseId,
    kVersionBuild ///
  };
  enum ReleasePhase {
    kAlphaPhase, ///
    kBetaPhase,
    kRcPhase,
    kReleasePhase
  };
  bool Parse(const std::string &ver) {
    //
    int status = kVersionMajor;
    for (auto iter = ver.begin(); iter != ver.end(); iter++) {
      char ch = *iter;
      switch (status) {
      case kVersionMajor:
        if (ch == '.') {
          status = kVersionMinor;
          continue;
        }
        if (ch < '0' || ch > '9') {
          return false;
        }
        major_ += major_ * 10 + ch; /// convert to dec
        break;
      case kVersionMinor:
        if (ch == '.') {
          status = kVersionPatch;
          continue;
        }
        if (ch < '0' || ch > '9') {
          return false;
        }
        minor_ += minor_ * 10 + ch; /// convert to dec
        break;
      case kVersionPatch:
        if (ch == '-') {
          status = kVersionPreRelease;
          continue;
        }
        if (ch == '+') {
          status = kVersionPatch;
          continue;
        }
        if (ch < '0' || ch > '9') {
          return false;
        }
        patch_ += patch_ * 10 + ch;
        break;
      case kVersionPreRelease:
        if (ch == '.') {
          status = kVersionPreReleaseId;
          continue;
        }
        if (ch == '+') {
          status = kVersionBuild;
          continue;
        }
        if (IsAlphaNum(ch) || ch == '-' || ch == '.') {
          prerelease_.push_back(ch);
        } else {
          return false;
        }
        break;
      case kVersionPreReleaseId:
        if (ch == '+') {
          status = kVersionBuild;
          continue;
        }
        if (IsAlphaNum(ch) || ch == '-') {
          prerelease_.push_back(ch);
          prereleaseid_ += ch;
        } else {
          return false;
        }
        break;
      case kVersionBuild:
        if (IsAlphaNum(ch) || ch == '-') {
          build_.push_back(ch);
        } else {
          return false;
        }
        break;
      default:
        break;
      }
    }
    if (prerelease_.find("alpha") != prerelease_.npos) {
      phase_ = kAlphaPhase;
    } else if (prerelease_.find("beta") != prerelease_.npos) {
      phase_ = kBetaPhase;
    } else if (prerelease_.find("rc") != prerelease_.npos) {
      phase_ = kRcPhase;
    }
    ////
    return true;
  }
  int compare(const Semver &other) {
    ///
    if (major_ > other.major_)
      return 1;
    if (major_ < other.major_)
      return -1;
    if (minor_ > other.minor_)
      return 1;
    if (minor_ < other.minor_)
      return -1;
    if (patch_ > other.patch_)
      return 1;
    if (patch_ < other.patch_)
      return -1;
    if (phase_ > other.phase_)
      return 1;
    if (phase_ < other.phase_)
      return -1;
    if (prereleaseid_ > other.prereleaseid_)
      return 1;
    if (prereleaseid_ < other.prereleaseid_)
      return -1;
    return 0;
  }

private:
  int major_{0};
  int minor_{0};
  int patch_{0};
  int phase_{kReleasePhase};
  std::string prerelease_;
  int prereleaseid_;
  std::string build_;
  ///
};

struct LocalPackage{
	std::wstring version;
	std::wstring installtime;
    bool enable;
};

struct RemotePackage{
    std::wstring name;
    std::wstring version;
    std::wstring url;
};

class Repositories {
public:
	Repositories(const Repositories &) = delete;
	Repositories &operator=(const Repositories&) = delete;
	bool Initialize(const wchar_t *rfile);
private:

};

class RepositoriesInstalled {
public:
	RepositoriesInstalled()
	{
		////
	}
	RepositoriesInstalled(const RepositoriesInstalled&) = delete;
	RepositoriesInstalled& operator=(const RepositoriesInstalled&) = delete;
	bool Discover(const wchar_t *rdfile);
private:
	std::unordered_map<std::wstring, LocalPackage> packages;
};

#endif

