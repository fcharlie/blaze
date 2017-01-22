#include <stdio.h>
#include <stdlib.h>
#include "keccak.hpp"

bool FileKeccakHash(const char *filename, int width) {
  static char to_hex[] = "0123456789abcdef";
  FILE *fp = fopen(filename, "rb");
  if (fp == nullptr)
    return false;
  //// must unsigned char
  unsigned char data[128] = {0};
  std::string hexdata;
  char buffer[4096];
  switch (width) {
  case 244: {
    keccak::sha3_244_hasher hash;
    while (!feof(fp)) {
      auto i = fread(buffer, 1, sizeof(buffer), fp);
      hash.update(buffer, i);
    }
    hash.finish(data, 36);
  } break;
  case 256: {
    keccak::sha3_256_hasher hash;
    while (!feof(fp)) {
      auto i = fread(buffer, 1, sizeof(buffer), fp);
      hash.update(buffer, i);
    }
    hash.finish(data, 64);
  } break;
  case 384: {
    keccak::sha3_384_hasher hash;
    while (!feof(fp)) {
      auto i = fread(buffer, 1, sizeof(buffer), fp);
      hash.update(buffer, i);
    }
    hash.finish(data, 96);
  } break;
  case 512: {
    keccak::sha3_512_hasher hash;
    while (!feof(fp)) {
      auto i = fread(buffer, 1, sizeof(buffer), fp);
      hash.update(buffer, i);
    }
    hash.finish(data, 128);
  } break;
  default:
    fclose(fp);
    return false;
  }
  auto blen = width / 8;
  for (int i = 0; i < blen; i++) {
    unsigned int val = data[i];
    hexdata.push_back(to_hex[val >> 4]);
    hexdata.push_back(to_hex[val & 0xf]);
  }
  fprintf(stderr, "%s  %s\n", hexdata.c_str(), filename);
  fclose(fp);
  return true;
}

int main(int argc, char **argv) {
  //
  int width = 256;
  if (argc < 2) {
    fprintf(stderr, "usage: \n\t%s filename\n\t%s -w=256 filename\n", argv[0],
            argv[0]);
    return 1;
  }
  const char *filename = nullptr;
  if (argc == 2) {
    filename = argv[1];
  } else if (argc > 2) {
    const char *w = argv[1];
    if (strncmp("-w=", argv[1], 3) == 0) {
      char *c;
      width = strtol(w + 3, &c, 10);
      filename = argv[2];
    } else {
      fprintf(stderr, "usage: \n\t%s filename\n\t%s -w=256 filename\n", argv[0],
              argv[0]);
      return 1;
    }
  }
  if (!FileKeccakHash(filename, width)) {
    fprintf(stderr, "File %s SHA-3 %d error\n", filename, width);
  }
  return 0;
}
