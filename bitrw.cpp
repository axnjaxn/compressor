#include "bitrw.h"

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
