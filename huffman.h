#ifndef _BPJ_HUFFMAN_H
#define _BPJ_HUFFMAN_H

#include <cstdio>

class HuffmanEncoder {
public:
  static const unsigned short magic = 9002;
  
  static void encode(FILE* in, FILE* out);
  static int decode(FILE* in, FILE* out);

private:
  HuffmanEncoder() { }
};

#endif
