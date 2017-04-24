///

#ifndef BLAZE_HPP
#define BLAZE_HPP
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdint>
#include <functional>

class BzStringA {
public:
  BzStringA(const wchar_t *wstr, int cp = CP_UTF8);
  BzStringA(const wchar_t *wstr, size_t length, int cp = CP_UTF8);
  BzStringA(const BzStringA &other) = delete;
  BzStringA(BzStringA &&other) {
    Release();
    data_ = other.data_;
    size_ = other.size_;
    other.data_ = nullptr;
    other.size_ = 0;
  }
  ~BzStringA();

  const char *data() const { return data_; }
  size_t size() const { return size_; }

private:
  void Release();
  char *data_;
  size_t size_;
};

class BzString {
public:
  BzString(const char *str, int cp = CP_UTF8);
  BzString(const char *str, size_t length, int cp = CP_UTF8);
  BzString(const BzString &other) = delete;
  BzString(BzString &&other) {
    Release();
    data_ = other.data_;
    size_ = other.size_;
    other.data_ = nullptr;
    other.size_ = 0;
  }
  ~BzString();
  const wchar_t *data() const { return data_; }
  size_t size() const { return size_; }

private:
  void Release();
  wchar_t *data_;
  size_t size_;
};

struct ProgressInvoke {
  bool (*impl)(void *data, std::uint64_t current, std::uint64_t total);
  void *userdata;
};

class BlazeManager {
private:
  void Finalize() {}
};

struct CharCaseCompareW {
  bool operator()(const wchar_t *left, const wchar_t *right) {
    return _wcsicmp(left, right) == 0;
  }
};

struct CharCompareW {
  bool operator()(const wchar_t *left, const wchar_t *right) const {
    return wcscmp(left, right) == 0;
  }
};

inline size_t HashInternal(const unsigned char *data,
                               size_t length) noexcept {
#if defined(_WIN64)
  static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
  constexpr size_t kFNVOffsetBasis = 14695981039346656037ULL;
  constexpr size_t kFNVPrime = 1099511628211ULL;
#else
  static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
  constexpr size_t kFNVOffsetBasis = 2166136261U;
  constexpr size_t kFNVPrime = 16777619U;
#endif

  size_t val = kFNVOffsetBasis;
  for (size_t next = 0; next < length; ++next) {
    // fold in another byte
    val ^= static_cast<size_t>(data[next]);
    val *= kFNVPrime;
  }

  return val;
}

struct CharHashW {
  size_t operator()(const wchar_t *wstr) const {
    auto src = reinterpret_cast<const unsigned char *>(wstr);
    auto length = wcslen(wstr) * 2;
    return HashInternal(src, length);
  }
};

#include "console.hpp"

#ifdef _DEBUG
#define DebugPrint BaseMessagePrint
#else
#ifdef _MSC_VER
#define DebugPrint __noop
#else
namespace detail {
inline void consume() {}
template <typename T0, typename... T>
void consume(const T0 &t0, const T &... t) {
  (void)t0;
  consume(t...);
}
}

#define noop(...) (void)([&]() { detail::consume(__VA_ARGS__); })
#define DebugPrint noop
#endif

#endif

class ErrorMessage {
public:
	ErrorMessage(DWORD errid) :lastError(errid), release_(false) {
		if (FormatMessageW(
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_ALLOCATE_BUFFER, nullptr, errid,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
			(LPWSTR)&buf, 0, nullptr) == 0) {
			buf = L"Unknown error";
		}
		else {
			release_ = true;
		}
	}
	~ErrorMessage() {
		if (release_) {
			LocalFree(buf);
		}
	}
	const wchar_t *message()const {
		return buf;
	}
	DWORD LastError()const { return lastError; }
private:
	DWORD lastError;
	LPWSTR buf;
	bool release_;
	char reserved[sizeof(intptr_t) - sizeof(bool)];
};


#endif
