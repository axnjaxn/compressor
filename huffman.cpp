#ifndef _BPJ_HUFFMAN_H
#define _BPJ_HUFFMAN_H

#include <cstdio>

class HuffmanEncoder {
public:
  static const unsigned short magic = 9002;
  
  static void encode(FILE* in, FILE* out);
  static int decode(FILE* in, FILE* out);

private:
  HuffmanEncoder() { }
};

#endif

#include "bitrw.h"
#include <string>
#include <vector>

class HuffmanTree {
public:
  HuffmanTree *zero, *one;
  unsigned char symbol;
  unsigned int count;

  HuffmanTree(unsigned char symbol, unsigned int count) {
    this->symbol = symbol; this->count = count;
    this->zero = this->one = NULL;
  }
  HuffmanTree(unsigned int count) {
    symbol = 0; this->count = count;
    this->zero = this->one = NULL;
  }
  HuffmanTree(HuffmanTree* zero, HuffmanTree* one) {
    this->zero = zero;
    this->one = one;
    symbol = 0;
    count = zero->count + zero->one;
  }
  ~HuffmanTree() {
    delete zero;
    delete one;
  }

  bool isLeaf() const {return zero || one;}
}

template <typename tn>
inline void swap(tn& a, tn& b) {tn t = a; a = b; b = t;}

void HuffmanEncoder:encode(FILE* in, FILE* out) {
  //Set up symbol table
  std::vector<HuffmanTree*> symbols;
  for (int i = 0; i < 256; i++)
    symbols.push_back(new HuffmanTree((unsigned char)i, 0));

  //Determine probabilities
  size_t pos = ftell(in);
  for (unsigned char byte = fgetc(fp); !feof(fp); byte = fgetc(fp))
    symbols[byte].count++;
  fseek(fp, pos, SEEK_SET);

  //Until convergence:
  while (symbols.size() > 1) {
    //Determine two minimum counts
    int least = 0, second = 1;
    for (int i = 2; i < symbols.size(); i++)
      if (symbols[i].count < symbols[second].count) {
	if (symbols[i].count < symbols[least.count]) {
	  second = least;
	  least = i;
	}
	else 
	  second = i;
      }
    
    //Combine them
    HuffmanTree* combined = new HuffmanTree(symbols[second], symbols[least]);
    if (second < least) swap(second, least);
    symbols.erase(symbols.begin() + least);
    symbols.erase(symbols.begin() + second);
    symbols.push_back(combined);
  }

  //Now that they're converged, write the symbol table
  //TODO

  //Finally, encode the symbols
}

int HuffmanEncoder::decode(FILE* in, FILE* out) {
  
}
