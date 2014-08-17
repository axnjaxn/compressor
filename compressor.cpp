#include "compressor.h"
#include <vector>
#include <stack>
#include <cstring>

class Trie {
public:
  Trie* child[256];
  unsigned int index;
  
  Trie(unsigned int index = 0) {memset(child, 0, sizeof(Trie*) * 256); this->index = index;}
  ~Trie() {for (int i = 0; i < 256; i++) delete child[i];}
};

void writeEntry(unsigned short index, unsigned char byte, FILE* out) {
  fwrite(&index, 2, 1, out);
  fwrite(&byte, 1, 1, out);
}

void writeShortEnding(FILE* out) {
  unsigned short marker = 0xFFFE;
  fwrite(&marker, 2, 1, out);
}

void writeLongEnding(unsigned short index, FILE* out) {
  unsigned short marker = 0xFFFF;
  fwrite(&marker, 2, 1, out);
  fwrite(&index, 2, 1, out);
}

void Compressor::encode(FILE* in, FILE* out) {
  Trie root, *scan;

  for (int i = 0; i < 256; i++)
    root.child[i] = new Trie(i);

  unsigned short magic = Compressor::magic;
  fwrite(&magic, 2, 1, out);

  unsigned short next = 256;
  unsigned char buf;
  for (;;) {
    buf = fgetc(in);
    
    if (feof(in)) {
      writeShortEnding(out);
      return;
    }

    scan = root.child[buf];

    for (;;) {
      buf = fgetc(in);

      if (feof(in)) {
	writeLongEnding(scan->index, out);
	return;
      }

      if (scan->child[buf]) scan = scan->child[buf];
      else {
	writeEntry(scan->index, buf, out);
	scan->child[buf] = new Trie(next++);
	break;
      }
    }
  }
}

template <typename tn>
inline void swap(tn& a, tn& b) {tn t = a; a = b; b = t;}

typedef struct {
  union {
    unsigned short index;
    struct {
      unsigned char a;
      unsigned char b;
    } bytes;
  };
  unsigned char byte;
} TableEntry;

void writePattern(const std::vector<TableEntry>& entries, unsigned short index, FILE* out) {
  static std::stack<unsigned char> stk;

  //Retrieve pattern
  while (index >= 256) {
    stk.push(entries[index - 256].byte);
    index = entries[index - 256].index;
  }
  stk.push((unsigned char)index);
  
  //Write pattern to file
  unsigned char byte;
  while (!stk.empty()) {
    byte = stk.top();
    stk.pop();
    fwrite(&byte, 1, 1, out);
  }
}

int Compressor::decode(FILE* in, FILE* out) {
  std::vector<TableEntry> rows;

  bool swap_flag = 0;
 
  TableEntry entry;
  fread(&entry.index, 2, 1, in);
  if (entry.index != Compressor::magic) {
    swap_flag = 1;
    swap(entry.bytes.a, entry.bytes.b);
    if (entry.index != Compressor::magic) return 1;//Invalid!
  }

  unsigned short buf;
  for (;;) {
    fread(&entry.index, 2, 1, in);
    if (swap_flag) swap(entry.bytes.a, entry.bytes.b); 
    
    if (entry.index == 0xFFFE) return 0;

    fread(&entry.byte, 1, 1, in);
    if (entry.index == 0xFFFF) {
      fwrite(&entry.byte, 1, 1, out);
      return 0;
    }
    else {
      rows.push_back(entry);
      writePattern(rows, rows.size() + 255, out);
    }
  }
}
