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

  void write(bool b);
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
public:
  static const unsigned short magic = 9001;
  
  typedef union {
    struct {
      unsigned int index:15;
      unsigned int bit:1;
    };
    struct {
      unsigned char a;
      unsigned char b;
    };
    unsigned short row;
  } TableRow;

  static void encode(FILE* in, FILE* out);
  static int decode(FILE* in, FILE* out);

private:
  Compressor() { }
};

#endif
