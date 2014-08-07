#include "compressor.h"
#include <vector>
#include <stack>

BitWriter::BitWriter(FILE* fp) {
  this->fp = fp;
  buf = 0;
  bitno = 0;
}

void BitWriter::write(bool b) {
  if (b) buf |= (1 << (7 - bitno));
  bitno++;

  if (bitno >= 8)
    flush();
}  

void BitWriter::flush() {
  if (!bitno) return;

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
  bool b = ((buf >> (7 - bitno++)) & 1);

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

void writeRow(const Compressor::TableRow& entry, FILE* out) {
  fwrite(&entry.row, 2, 1, out);
}

void Compressor::encode(FILE* in, FILE* out) {
  BitReader reader(in);
  
  TableRow entry;
  BTree* tree = new BTree();
  BTree* scan = tree;
  unsigned int next = 1, prev = 0;
  bool bit;

  unsigned short magic = Compressor::magic;
  fwrite(&magic, 2, 1, out);

  for (;;) {
    //Finished reading?
    if (reader.eof()) {
      //Write a special row
      entry.row = ~0;
      writeRow(entry, out);
      
      //Write final pattern matched
      entry.index = scan->index;
      entry.bit = 0;
      writeRow(entry, out);

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
    entry.index = scan->index;
    entry.bit = bit;
    writeRow(entry, out);

    if (!bit) scan->zero = new BTree(next++);
    else scan->one = new BTree(next++);

    scan = tree;
  }

  delete tree;
}

template <typename tn>
inline void swap(tn& a, tn& b) {tn t = a; a = b; b = t;}

void readRow(Compressor::TableRow& entry, FILE* in, bool swap_flag) {
  fread(&entry, 2, 1, in);
  if (swap_flag)
    swap(entry.a, entry.b);
}

void writePattern(const std::vector<Compressor::TableRow>& rows, unsigned short index, BitWriter& writer) {
  static std::stack<Compressor::TableRow> stk;
  Compressor::TableRow entry;
  
  //Retrieve pattern
  while (index != 0) {
    entry = rows[index];
    stk.push(entry);
    index = entry.index;
  }
  
  //Write pattern to file
  while (!stk.empty()) {
    writer.write(stk.top().bit);
    stk.pop();
  }
}

int Compressor::decode(FILE* in, FILE* out) {
  BitWriter writer(out);

  std::vector<TableRow> rows;
  rows.push_back(TableRow());

  TableRow entry;
  bool swap_flag = 0;

  fread(&entry, 2, 1, in);
  if (entry.row != Compressor::magic) {
    swap_flag = 1;
    swap(entry.a, entry.b);
    if (entry.row != Compressor::magic) return 1;//Invalid!
  }

  for (;;) {
    //Read row
    readRow(entry, in, swap_flag);
        
    //Is this the end?
    if (entry.row == (unsigned short)(~0)) {
      readRow(entry, in, swap_flag);
      writePattern(rows, entry.index, writer);
      writer.flush();      
      break;
    }

    //Record pattern and write to output
    rows.push_back(entry);
    writePattern(rows, rows.size() - 1, writer);
  }

  return 0;
}
