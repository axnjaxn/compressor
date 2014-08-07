#include <cstdio>
#include <csignal>
#include "compressor.h"

void segFault(int param) {
  printf("Segmentation fault.\n");
}

int main(int argc, char* argv[]) {
  signal(SIGSEGV, segFault);

  FILE* in = fopen(argv[1], "rb");
  FILE* out = fopen(argv[2], "wb");

  Compressor::encode(in, out);
  
  fclose(in);
  fclose(out);
  
  //TODO: Compress Info
  printf("Compression complete.\n");

  return 0;
}
