#ifndef _BPJ_COMPRESSOR_H
#define _BPJ_COMPRESSOR_H

#include <cstdio>

class Compressor {  
private:
  static const unsigned short magic = 9001;
  
  typedef union {
    struct {
      unsigned int index:31;
      unsigned int bit:1;
    };
    unsigned int row:32;
  } TableRow;

public:
  static void encode(FILE* in, FILE* out);
  static void decode(FILE* in, FILE* out);
};

#endif

#include <cstdio>

int main(int argc, char** argv) {
  return 0;
}
