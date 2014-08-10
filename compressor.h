#ifndef _BPJ_COMPRESSOR_H
#define _BPJ_COMPRESSOR_H

#include <cstdio>

class Compressor {  
public:
  static const unsigned short magic = 9001;

  static void encode(FILE* in, FILE* out);
  static int decode(FILE* in, FILE* out);

private:
  Compressor() { }
};

#endif
