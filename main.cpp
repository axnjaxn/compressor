#include "compressor.h"
#include "huffman.h"
#include <cstdio>
#include <csignal>
#include <cstring>
#include <string>

int main(int argc, char* argv[]) {
  if (argc < 3 || argc > 4) return 1;

  if (!strcmp(argv[1], "-x")) {
    if (argc != 4) return 1;

    FILE* in = fopen(argv[2], "rb");
    FILE* out = fopen(argv[3], "wb");
    
    Compressor::decode(in, out);
    //HuffmanEncoder::decode(in, out);
    
    fclose(in);
    fclose(out);
    
    //TODO: Compress Info
    printf("Decompression complete.\n");
  }
  else {
    if (argc != 3) return 1;

    FILE* in = fopen(argv[1], "rb");
    FILE* out = fopen(argv[2], "wb");
    
    Compressor::encode(in, out);
    //HuffmanEncoder::encode(in, out);
    
    fclose(in);
    fclose(out);
    
    //TODO: Compress Info
    printf("Compression complete.\n");
  }

  return 0;
}
