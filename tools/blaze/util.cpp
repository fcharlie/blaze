#include "blaze.hpp"
#include <Wincrypt.h>
#include <array>

#define BUFFER_SIZE 4096
#define SHA256LENGTH 32

bool FileSecureHash(const std::wstring &file, std::wstring &hash) {
  BOOL bResult = true;
  HCRYPTPROV hProv = 0;
  HCRYPTHASH hHash = 0;
  HANDLE hFile = nullptr;
  BYTE rgbFile[BUFFER_SIZE];
  DWORD cbRead = 0;
  std::array<unsigned char, 32> sha256;
  DWORD cbHash = 0;
  CHAR rgbDigits[] = "0123456789abcdef";
  hFile = CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                      OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
  if (INVALID_HANDLE_VALUE == hFile) {
    BaseErrorMessagePrint(L"Error opening file %s\nError: %d\n", file.c_str(),
                          GetLastError());
    return false;
  }
  // must use PROV_RSA_AES
  if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES,
                           CRYPT_VERIFYCONTEXT)) {
    CloseHandle(hFile);
    return false;
  }

  if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
    CloseHandle(hFile);
    CryptReleaseContext(hProv, 0);
    return false;
  }

  while (bResult = ReadFile(hFile, rgbFile, BUFFER_SIZE, &cbRead, NULL)) {
    if (0 == cbRead) {
      break;
    }
    if (!CryptHashData(hHash, rgbFile, cbRead, 0)) {
      CryptReleaseContext(hProv, 0);
      CryptDestroyHash(hHash);
      CloseHandle(hFile);
      return false;
    }
  }

  if (!bResult) {
    CryptReleaseContext(hProv, 0);
    CryptDestroyHash(hHash);
    CloseHandle(hFile);
    return false;
  }

  cbHash = sha256.size();
  if (CryptGetHashParam(hHash, HP_HASHVAL, sha256.data(), &cbHash, 0)) {
    for (DWORD i = 0; i < cbHash; i++) {
      hash.push_back(rgbDigits[sha256[i] >> 4]);
      hash.push_back(rgbDigits[sha256[i] & 0xf]);
    }
  } else {
    bResult = FALSE;
  }

  CryptDestroyHash(hHash);
  CryptReleaseContext(hProv, 0);
  CloseHandle(hFile);

  return (bResult == TRUE);
}
