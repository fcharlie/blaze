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


//-----------------------------------------------------------------------------
// MurmurHash2, 64-bit versions, by Austin Appleby

// The same caveats as 32-bit MurmurHash2 apply here - beware of alignment 
// and endian-ness issues if used across multiple platforms.

// unsigned long int uint64_t;

// 64-bit hash for 64-bit platforms
uint64_t MurmurHash64A(const void * key, int len, unsigned int seed)
{
	const uint64_t m = 0xc6a4a7935bd1e995;
	const int r = 47;

	uint64_t h = seed ^ (len * m);

	const uint64_t * data = (const uint64_t *)key;
	const uint64_t * end = data + (len / 8);

	while (data != end)
	{
		uint64_t k = *data++;

		k *= m;
		k ^= k >> r;
		k *= m;

		h ^= k;
		h *= m;
	}

	const unsigned char * data2 = (const unsigned char*)data;

	switch (len & 7)
	{
	case 7: h ^= uint64_t(data2[6]) << 48;
	case 6: h ^= uint64_t(data2[5]) << 40;
	case 5: h ^= uint64_t(data2[4]) << 32;
	case 4: h ^= uint64_t(data2[3]) << 24;
	case 3: h ^= uint64_t(data2[2]) << 16;
	case 2: h ^= uint64_t(data2[1]) << 8;
	case 1: h ^= uint64_t(data2[0]);
		h *= m;
	};

	h ^= h >> r;
	h *= m;
	h ^= h >> r;

	return h;
}


// 64-bit hash for 32-bit platforms
uint64_t MurmurHash64B(const void * key, int len, unsigned int seed)
{
	const unsigned int m = 0x5bd1e995;
	const int r = 24;

	unsigned int h1 = seed ^ len;
	unsigned int h2 = 0;

	const unsigned int * data = (const unsigned int *)key;

	while (len >= 8)
	{
		unsigned int k1 = *data++;
		k1 *= m; k1 ^= k1 >> r; k1 *= m;
		h1 *= m; h1 ^= k1;
		len -= 4;

		unsigned int k2 = *data++;
		k2 *= m; k2 ^= k2 >> r; k2 *= m;
		h2 *= m; h2 ^= k2;
		len -= 4;
	}

	if (len >= 4)
	{
		unsigned int k1 = *data++;
		k1 *= m; k1 ^= k1 >> r; k1 *= m;
		h1 *= m; h1 ^= k1;
		len -= 4;
	}

	switch (len)
	{
	case 3: h2 ^= ((unsigned char*)data)[2] << 16;
	case 2: h2 ^= ((unsigned char*)data)[1] << 8;
	case 1: h2 ^= ((unsigned char*)data)[0];
		h2 *= m;
	};

	h1 ^= h2 >> 18; h1 *= m;
	h2 ^= h1 >> 22; h2 *= m;
	h1 ^= h2 >> 17; h1 *= m;
	h2 ^= h1 >> 19; h2 *= m;

	uint64_t h = h1;

	h = (h << 32) | h2;

	return h;
}