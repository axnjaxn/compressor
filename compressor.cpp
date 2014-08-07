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
  BTree* tree = new BTree();
  BTree* scan = tree;
  unsigned int next = 1;
  bool bit;

  unsigned short magic = Compressor::magic;
  fwrite(&magic, 2, 1, out);

  for (;;) {
    //Finished reading?
    if (reader.eof()) {
      //Write a special row
      entry.row = ~0;
      printf("Write row: ID = %d bit = %d\n", entry.index, entry.bit);
      fwrite(&entry.row, 2, 1, out);
      
      //Write final pattern matched
      entry.index = scan->index;
      entry.bit = 0;
      printf("Write row: ID = %d bit = %d\n", entry.index, entry.bit);
      fwrite(&entry.row, 2, 1, out);

      //Done compressing
      break;
    }

    //Read one bit from input
    bit = reader.next();

    //If the pattern's been seen before, continue
    if (!bit) {
      if (scan->zero) {scan = scan->zero; continue;}
    }
    else {
      if (scan->one) {scan = scan->one; continue;}
    }

    //Otherwise, prepare the next row and record the pattern
    entry.index = next++;
    entry.bit = bit;
    printf("Write row: ID = %d bit = %d\n", entry.index, entry.bit);
    fwrite(&entry.row, 2, 1, out);

    if (!bit) scan->zero = new BTree(entry.index);
    else scan->one = new BTree(entry.index);
  }

  delete tree;
}

void Compressor::decode(FILE* in, FILE* out) { }
