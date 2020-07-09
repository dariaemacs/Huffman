#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "node.hpp"

#include <queue>

using priority_queue_t = std::priority_queue<pointer, std::vector<pointer>, Greater>;

class Huffman_window;

class Huffman{
  int file_size     {0};
  int new_file_size {0};
  
  std::string file_name;
  std::string out_file_name;

  std::vector<int> frequency;
  std::vector<std::string> codes;

  priority_queue_t queue;
  pointer root {nullptr};
  
  std::string encode_message;
  std::string decode_message;
  std::string message;

public:
  static const std::string TYPE;

  Huffman(const std::string name): file_name(name), frequency(0x100, 0), codes(0x100, "") {}
   
  virtual ~Huffman() = default;
  
  int compress();
  int decompress();
  
private:
  int encode_file();
  int fill_queue();
  void build_tree();

  void message2code(std::ifstream& input_file);
  void recursive_chs2codes(Node::pointer& node, std::string str);
  void iterative_chs2codes();

  void write_frequency(std::ofstream& output_file);
  void write_raw_message(std::ofstream& output_file);
  int  write_encoding_file();
  
  void read_frequency(std::ifstream& input_file, uchar& count);
  void read_raw_message(std::ifstream& input_file);
  int  read_decoding_file();

  void check_file_name();
  void codes2chs();
  int  write_decoding_file();
  
};


#endif //HUFFMAN_H
