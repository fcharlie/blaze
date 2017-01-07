#include "blaze.hpp"

BzStringA::BzStringA(const wchar_t *wstr, int cp) {
	if (wstr == nullptr)
		return;
	auto unicodeLen = WideCharToMultiByte(cp, 0, wstr, -1, NULL, 0, NULL, NULL);
	if (unicodeLen == 0)
		return;
	data_ = new char[unicodeLen + 1];
	if (data_ == nullptr)
		return;
	data_[unicodeLen] = 0;
	::WideCharToMultiByte(cp, 0, wstr, -1, data_, unicodeLen, NULL, NULL);
	size_ = unicodeLen;
}

BzStringA::BzStringA(const wchar_t* wstr, size_t length, int cp) {
	if (wstr == nullptr)
		return;
	auto unicodeLen = WideCharToMultiByte(cp, 0, wstr, length, NULL, 0, NULL, NULL);
	if (unicodeLen == 0)
		return;
	data_ = new char[unicodeLen + 1];
	if (data_ == nullptr)
		return;
	data_[unicodeLen] = 0;
	::WideCharToMultiByte(cp, 0, wstr, length, data_, unicodeLen, NULL, NULL);
	size_ = unicodeLen;
}

BzStringA::~BzStringA() {
	Release();
}

void BzStringA::Release() {
	if (data_) {
		delete[] data_;
	}
}
//////////////////////////////////

BzString::BzString(const char *str, int cp ) {
	if (str == nullptr)
		return;
	auto len = strlen(str);
	int unicodeLen = ::MultiByteToWideChar(cp, 0, str, len, NULL, 0);
	if (unicodeLen == 0)
		return;
	data_ = new wchar_t[unicodeLen + 1];
	if (data_ == nullptr)
		return;
	data_[unicodeLen] = 0;
	::MultiByteToWideChar(cp, 0, str, -1, (LPWSTR)data_, unicodeLen);
	size_ = unicodeLen;
}

BzString::BzString(const char *str, size_t length, int cp) {
	if (str == nullptr)
		return;
	int unicodeLen = ::MultiByteToWideChar(cp, 0, str, length, NULL, 0);
	if (unicodeLen == 0)
		return;
	data_ = new wchar_t[unicodeLen + 1];
	if (data_ == nullptr)
		return;
	data_[unicodeLen] = 0;
	::MultiByteToWideChar(cp, 0, str, -1, (LPWSTR)data_, unicodeLen);
	size_ = unicodeLen;
}

BzString::~BzString() {
	Release();
}

void BzString::Release() {
	if (data_) {
		delete[] data_;
	}
}