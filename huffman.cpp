#include "huffman.h"
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
    count = zero->count + one->count;
  }
  ~HuffmanTree() {
    delete zero;
    delete one;
  }

  bool isLeaf() const {return !zero && !one;}

  static void writeTree(HuffmanTree* tree, BitWriter& writer) {
    if (tree->isLeaf()) {
      writer.write(1);
      unsigned char symbol = tree->symbol;
      for (int i = 0; i < 8; i++) {
	writer.write((symbol & 0x80) == 0x80);
	symbol <<= 1;
      }
    }
    else {
      writer.write(0);
      writeTree(tree->zero, writer);
      writeTree(tree->one, writer);
    }    
  }

  static HuffmanTree* readTree(BitReader& reader) {
    if (reader.next()) {
      unsigned char symbol = 0;
      for (int i = 0; i < 8; i++) {
	if (reader.next()) symbol |= 1;
	symbol <<= 1;
      }
      return new HuffmanTree(symbol, 0);
    }
    else {
      HuffmanTree* tree = new HuffmanTree(0);
      tree->zero = readTree(reader);
      tree->one = readTree(reader);
    }
  }

  void getStrings(std::string strings[256], const std::string& builder) const {
    if (isLeaf()) strings[symbol] = builder;
    else {
      zero->getStrings(strings, builder + "0");
      one->getStrings(strings, builder + "1");
    }
  }
};

template <typename tn>
inline void swap(tn& a, tn& b) {tn t = a; a = b; b = t;}

void HuffmanEncoder::encode(FILE* in, FILE* out) {
  //Write magic
  unsigned short magic = HuffmanEncoder::magic;
  fwrite(&magic, 2, 1, out);

  //Set up symbol table
  std::vector<HuffmanTree*> symbols;
  for (int i = 0; i < 256; i++)
    symbols.push_back(new HuffmanTree((unsigned char)i, 0));

  //Determine probabilities
  size_t pos = ftell(in);
  for (unsigned char byte = fgetc(in); !feof(in); byte = fgetc(in))
    symbols[byte]->count++;
  fseek(in, pos, SEEK_SET);

  //Until convergence:
  while (symbols.size() > 1) {
    //Determine two minimum counts
    int least = 0, second = 1;
    if (symbols[second]->count < symbols[least]->count)
      swap(second, least);
    for (int i = 2; i < symbols.size(); i++)
      if (symbols[i]->count <= symbols[second]->count) {
	if (symbols[i]->count <= symbols[least]->count) {
	  second = least;
	  least = i;
	}
	else 
	  second = i;
      }
    
    //Combine them
    HuffmanTree* combined = new HuffmanTree(symbols[second], symbols[least]);
    if (second > least) swap(second, least);
    symbols.erase(symbols.begin() + least);
    symbols.erase(symbols.begin() + second);
    symbols.push_back(combined);
  }

  //Now that they're converged, write the symbol table
  BitWriter writer(out);
  HuffmanTree::writeTree(symbols[0], writer);

  //Finally, encode the symbols
  std::string strings[256];
  symbols[0]->getStrings(strings, "");
  for (unsigned char byte = fgetc(in); !feof(in); byte = fgetc(in))
    for (int i = 0; i < strings[byte].size(); i++)
      if (strings[byte][i] == '0') writer.write(0);
      else writer.write(1);

  //TODO: Ending sequence
  writer.flush();

  //Clean up
  delete symbols[0];
}

int HuffmanEncoder::decode(FILE* in, FILE* out) {
  //Read magic (and deduce endianness)
  bool swap_flag = 0;
  unsigned short magic;
  fread(&magic, 2, 1, in);
  if (magic != HuffmanEncoder::magic) {
    swap_flag = 1;
    magic = ((magic & 0xFF) << 8) | ((magic & 0xFF00) >> 8);
    if (magic != HuffmanEncoder::magic) return 1;//Invalid!
  }

  //Read symbol table
  BitReader reader(in);
  HuffmanTree* tree = HuffmanTree::readTree(reader);
  HuffmanTree* scan = tree;

  //Finally, decode the symbols
  bool bit;
  while (!reader.eof()) {
    bit = reader.next();
    if (bit) scan = scan->one;
    else scan = scan->zero;

    if (scan->isLeaf()) {
      fwrite(&scan->symbol, 1, 1, out);
      scan = tree;
    }
    
    //TODO: Ending sequence    
  }

  delete tree;
  return 0;
}
