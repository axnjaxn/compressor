#ifndef _BPJ_COMPRESSOR_H
#define _BPJ_COMPRESSOR_H

#include <cstdio>

class BitWriter {

};

class BitReader {
private:
  FILE* fp;

public:
  BitReader(FILE* fp) {
    this->fp = fp;
  }

  
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

  class BTree {
  public:
    BTree *zero, *one;
    unsigned int index;

    BTree(unsigned int index = 0) {zero = one = NULL; this->index = index;}
    ~BTree() {if (zero) delete zero; if (one) delete one;}
  };

public:
  static void encode(FILE* in, FILE* out);
  static void decode(FILE* in, FILE* out);
};

#endif

void Compressor::encode(FILE* in, FILE* out) {
  BitReader reader(in);
  
  TableRow entry;
  BTree* tree = new BTree(), *scan;
  tree->zero = new BTree(0);
  tree->one = new BTree(1);
  unsigned int next = 2;
  bool bit;

  fwrite(&Compressor::magic, 2, 1, fp);
  while (!reader.eof()) {
    scan = tree;
    for (;;) {
      if (reader.eof()) doSomething();
      
      bit = reader.next();
      if (!bit) {
	if (scan->zero) scan = scan->zero;
	else {
	  entry.index = next;
	  entry.bit = bit;
	  scan->zero = new BTree(next++);
	}
      }
      else {
	if (scan->zero) scan = scan->zero;
	else {
	  entry.index = next++;
	  entry.bit = bit;
	  scan->one = new BTree(next++);
	}
      }  
    }
    
    fwrite(&entry.row, 2, 1, fp);
  }
}
