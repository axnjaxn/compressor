#include "compressor.h"

BitWriter::BitWriter(FILE* fp) {
  this->fp = fp;
  bitno = 0;
}

void BitWriter::next(bool b) {
  if (b) buf |= (1 << (7 - bitno));
  bitno++;

  if (bitno > 7)
    flush();
}  

void BitWriter::flush() {
  bitno = 0;
  fputc(buf, fp);
  buf = 0;
}

BitReader::BitReader(FILE* fp) {
  this->fp = fp;
  buf = fgetc(fp);
  bitno = 0;
}

bool BitReader::eof() const {
  return feof(fp);
}

bool BitReader::next() {
  bool b = (buf >> (7 - bitno++));

  if (bitno > 7) {
    bitno = 0;
    buf = fgetc(fp);
  }  

  return b;
}  

class BTree {
public:
  BTree *zero, *one;
  unsigned int index;
  
  BTree(unsigned int index = 0) {zero = one = NULL; this->index = index;}
  ~BTree() {if (zero) delete zero; if (one) delete one;}
};

void Compressor::encode(FILE* in, FILE* out) {
  BitReader reader(in);
  
  TableRow entry;
  BTree* tree = new BTree(), *scan;
  tree->zero = new BTree(0);
  tree->one = new BTree(1);
  unsigned int next = 2;
  bool bit;

  unsigned short magic = Compressor::magic;
  fwrite(&magic, 2, 1, out);
  while (!reader.eof()) {
    //Initialize pattern search
    scan = tree;

    for (;;) {
      //If the pattern has no more bits,
      if (reader.eof()) {
	//Write a special row
	entry.row = ~0;
	fwrite(&entry.row, 2, 1, out);

	//Then break and write the currently matched pattern
	entry.index = scan->index;
	entry.bit = 0;
	break;
      }
      
      //If there are more bits
      bit = reader.next();
      if (!bit) {
	//If the pattern's been seen before, continue
	if (scan->zero) scan = scan->zero;
	//Otherwise, prepare the next row and record the pattern
	else {
	  entry.index = next;
	  entry.bit = bit;
	  scan->zero = new BTree(next++);
	  break;
	}
      }
      else {
	//If the pattern's been seen before, continue	if (scan->zero) scan = scan->zero;
	if (scan->one) scan = scan->one;
	//Otherwise, prepare the next row and record the pattern
	else {
	  entry.index = next;
	  entry.bit = bit;
	  scan->one = new BTree(next++);
	  break;
	}
      }  
    }
    
    //Write this row
    fwrite(&entry.row, 2, 1, out);
  }

  delete tree;
}

void Compressor::decode(FILE* in, FILE* out) { }
