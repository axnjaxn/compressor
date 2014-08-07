#ifndef _BPJ_COMPRESSOR_H
#define _BPJ_COMPRESSOR_H

#include <cstdio>

class BitWriter {
private:
  FILE* fp;
  unsigned char buf;
  int bitno;

public:
  BitWriter(FILE* fp);

  void next(bool b);
  void flush();
};

class BitReader {
private:
  FILE* fp;
  unsigned char buf;
  int bitno;

public:
  BitReader(FILE* fp);

  bool eof() const;
  bool next();
};

class Compressor {  
private:
  static const unsigned short magic = 9001;
  
  typedef union {
    struct {
      unsigned int index:15;
      unsigned int bit:1;
    };
    unsigned short row;
  } TableRow;

public:
  static void encode(FILE* in, FILE* out);
  static void decode(FILE* in, FILE* out);
};

#endif
