#include "blaze.hpp"
#include <zip.h>
#include <unzip.h>
#include <cassert>
#include <vector>
#include <Shlwapi.h>

#define ZIP_GPBF_LANGUAGE_ENCODING_FLAG 0x800
#define BUFFER_SIZE 4096

bool ZipDecompressCurrentFile(unzFile uf, const std::wstring &dir) {
  char szFilePath[MAX_PATH];
  unz_file_info64 fileinfo;
  if (unzGetCurrentFileInfo64(uf, &fileinfo, szFilePath, sizeof(szFilePath),
                              NULL, 0, NULL, 0) != UNZ_OK) {
    return false;
  }
  if (unzOpenCurrentFile(uf) != UNZ_OK) {
    return false;
  }
  std::wstring fileName = dir;
  std::wstring dirName;
  if (fileName.back() != '/' || fileName.back() != '\\') {
    fileName.push_back('\\');
  }
  if ((fileinfo.flag & ZIP_GPBF_LANGUAGE_ENCODING_FLAG)) {
    BzString bs(szFilePath);
    // wprintf(L"File: %s\n", bs.data());
    assert(bs.data());
    dirName.assign(bs.data(), bs.size());
  } else {
    BzString bs(szFilePath, CP_ACP);
    assert(bs.data());
    dirName.assign(bs.data(), bs.size());
  }
  std::wstring buffer(dirName);
  LPWSTR lpszCurrentFile = &buffer[0];
  LPWSTR lpszFileName = lpszCurrentFile;
  size_t length = buffer.length();
  for (size_t i = 0; i <= length; ++i) {
    if (lpszFileName[i] == 0) {
      fileName += lpszCurrentFile;
      break;
    }
    if (lpszFileName[i] == '\\' || lpszFileName[i] == '/') {
      lpszFileName[i] = '\0';
      fileName += lpszCurrentFile;
      fileName += L"\\";
      if (CreateDirectoryW(fileName.c_str(), NULL) != TRUE &&
          GetLastError() != ERROR_ALREADY_EXISTS) {
        return false;
      }
      lpszCurrentFile = lpszFileName + i + 1;
    }
  }
  if (lpszCurrentFile[0] == 0) {
    return TRUE;
  }
  HANDLE hFile = CreateFileW(fileName.c_str(), GENERIC_WRITE, 0, NULL,
                             CREATE_ALWAYS, 0, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    return false;
  }
  BYTE rawbuf[BUFFER_SIZE];

  while (true) {
    int nSize = unzReadCurrentFile(uf, rawbuf, BUFFER_SIZE);

    if (nSize < 0) {
      CloseHandle(hFile);
      return false;
    } else if (nSize == 0) {
      break;
    } else {
      DWORD dwWritten = 0;
      if (!WriteFile(hFile, rawbuf, (DWORD)nSize, &dwWritten, NULL) ||
          dwWritten != (DWORD)nSize) {
        CloseHandle(hFile);
        return false;
      }
    }
  }
  FILETIME ftLocal, ftUTC;
  DosDateTimeToFileTime((WORD)(fileinfo.dosDate >> 16), (WORD)fileinfo.dosDate,
                        &ftLocal);
  LocalFileTimeToFileTime(&ftLocal, &ftUTC);
  SetFileTime(hFile, &ftUTC, &ftUTC, &ftUTC);
  return true;
}

bool ZipDecompressArchive(const std::wstring &file, const std::wstring &dir) {
  BaseMessagePrint(L"Decompress file: %s to %s\n", file.c_str(), dir.c_str());
  BzStringA as(file.c_str(), file.size(), CP_ACP);
  /// when build minizip as unicode, unzOpen64 support unicode
  unzFile uf = unzOpen64(as.data());
  if (uf == nullptr) {
    BaseErrorMessagePrint(L"unzOpen64() failed\n");
    return false;
  }
  unz_global_info64 gi;
  if (unzGetGlobalInfo64(uf, &gi) != UNZ_OK) {
    return false;
  }
  CreateDirectoryW(dir.c_str(), nullptr);
  for (int i = 0; i < gi.number_entry; ++i) {
    if (!ZipDecompressCurrentFile(uf, dir)) {
      unzClose(uf);
      return false;
    }
    if (i < gi.number_entry - 1) {
      if (unzGoToNextFile(uf) != UNZ_OK) {
        unzClose(uf);
        return false;
      }
    }
  }
  unzClose(uf);
  return true;
}

bool ZipCompressArchive(const std::wstring &path, const std::wstring &file) {
  ///
  return true;
}