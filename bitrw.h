#ifndef _BPJ_BIT_RW_H
#define _BPJ_BIT_RW_H

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

#endif
