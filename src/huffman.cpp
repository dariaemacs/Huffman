#include "huffman.hpp"
#include "window.hpp"
#include "auxiliary.hpp"

#include <iostream>
#include <fstream>
#include <bitset>
#include <limits>

#include <FL/Fl.H>

Huffman::Huffman(const std::string name, Huffman_window* w) :
  hw(w), file_name(name), frequency(0x100, 0), codes(0x100, "")
{
  file_size = get_filesize(file_name);

  std::stringstream ss;
  ss << "File size: " << file_size << " Kb";
  hw->size_box1->copy_label(ss.str().c_str());
}

int Huffman::compress(){
  if(file_size < 0){
    std::cerr << "Can't get file size." << std::endl;
    return -1;
  }

  std::cout << "============ Compress ===========\n";

  int ret = encode_file();
  if(ret == -1){
    std::cerr << "Error in encode file." << std::endl;
    return -1;
  }

  ret = fill_queue();
  if(ret == -1){
    std::cerr << "Error in fill queue." << std::endl;
    return -1;
  }

  build_tree();
  root = queue.top();

  recursive_chs2codes(root, "");

  ret = write_encoding_file();
  if(ret == -1){
    std::cerr << "Error in write encoding file." << std::endl;
    return -1;
  }

  gui_compress_result();

  std::cout << "Normal file size:     " << file_size     << " b\n";
  std::cout << "Compressed file size: " << new_file_size << " b\n";
  
  float ratio = ((float)new_file_size * 100) / file_size;
  if(ratio > 100){
    std::cout << "Compression isn't effective. File is too small.\n";
  }else{
    std::cout << "Efficiency of compression: " << ratio << " %\n";
  }
  std::cout << "=================================\n";
  return 0;
}

int Huffman::decompress(){
  file_size = get_filesize(file_name);
  std::cout << "============ Decompress =========\n";

  check_file_name();
    
  decode_message = "";
  int ret = read_decoding_file();
  if(ret == -1){
    std::cerr << "Error in read decoding file." << std::endl;
    return -1;
  }
    
  ret = fill_queue();
  if(ret == -1){
    std::cerr << "Error in fill queue." << std::endl;
    return -1;
  }

    
  build_tree();
  root = queue.top();

  codes2chs();
    
  ret = write_decoding_file();
  if(ret == -1){
    std::cerr << "Error in writing decoding file." << std::endl;
    return -1;
  }

  gui_decompress_result();
  
  std::cout << "Compressed file size:   " << file_size     << " b\n";
  std::cout << "Decompressed file size: " << new_file_size << " b\n";

  std::cout << "=================================\n";
  return 0;
}


void Huffman::gui_set_progress(int value){
  std::stringstream ss;
  ss << value << "%";
  hw->progress->value(value);
  hw->progress->copy_label(ss.str().c_str());
}


int Huffman::encode_file() {
  std::ifstream is (file_name, std::ifstream::binary);
  if (!is) {
    std::cerr << "File opening error: " << file_name << std::endl; 
    return -1;
  }
  
  int i = 0;
  int file_step = file_size/200;
  if(file_step == 0){
    file_step = 10;
  }
  while(true){
    char ch;
    is.read (&ch, 1);
    ++i;

    if(is.eof()){
      break;
    }
    ++frequency[static_cast<unsigned char>(ch)];
    int value = (i + file_size/file_step) * 100.0 / file_size;
    gui_set_progress(value);
    if(i % file_step == 0){
      Fl::check();
    }
    std::cout << "\rReading file " << i << ": " << value << "%" << std::flush;
  }
  std::cout << "\rReading file "<< file_size << ": 100%" << std::flush;
  std::cout << "\n" << std::endl;

#ifdef DEBUG
  print(frequency);
#endif
  auto max_value = std::max_element(frequency.begin(), frequency.end());
  if(*max_value >= std::numeric_limits<int>::max()){
    std::cout << "This file is too big. Maximum frequency value must be less "
	      << std::numeric_limits<int>::max() << ", but it is " << *max_value << std::endl;
    return -1;
  }
  return 0;
}

int Huffman::fill_queue(){
  for(size_t i = 0; i < frequency.size(); ++i){
    if(frequency[i] != 0){
      pointer node = std::make_shared<Node>(static_cast<uchar>(i), frequency[i]);
      queue.push(node);
    }
  }

  if (queue.empty()) {
    std::cerr << "The queue is empty: " << __PRETTY_FUNCTION__ << std::endl; 
    return -1;
  }
  return 0;
}

void Huffman::build_tree(){

#ifdef DEBUG
  int i = queue.size();
#endif

  while(queue.size() > 1){
    Node::pointer x = queue.top();
    queue.pop();

    Node::pointer y = queue.top();
    queue.pop();

    std::string name = "";
    name += x->get_name();
    name += y->get_name();

    Node::pointer z = std::make_shared<Node>(name, *x + *y);
    z->left = x;
    z->right = y;

    x->parent = z;
    y->parent = z;

    queue.push(z);

#ifdef DEBUG
    std::cout << "\rBuild tree: " << (int)(((i-queue.size()+1)*100.0)/i) << "%" << std::flush;
#endif
  }
#ifdef DEBUG
  std::cout << std::endl;
#endif
}

void Huffman::message2code(std::ifstream& ifs){
  while(true){
    char ch;
    ifs.read (&ch, 1);

    if(ifs.eof()){
      break;
    }
    encode_message += codes[static_cast<uchar>(ch)];
  }
}

void Huffman::recursive_chs2codes(Node::pointer& node, std::string str){
  if(node->left == nullptr && node->right == nullptr){
    node->code(str);
    codes[node->get_byte()] = node->code();
    
#ifdef DEBUG
    std::cout << "[" << node->get_char() << "]=" << codes[node->get_byte()] << ";\n";
#endif
    return;
  }
  
  if(node->left != nullptr){
    recursive_chs2codes(node->left, str + "0");
  }

  if(node->right != nullptr){
    recursive_chs2codes(node->right, str + "1");
  }
}

void Huffman::iterative_chs2codes(){
  std::stack<pointer> tree_stack;
  tree_stack.push(root);
  
  pointer node = tree_stack.top();
    
  while(!(root->left == nullptr && root->right == nullptr)){
    if(node->right != nullptr){
      node->right->code(node->code() + "1");
      tree_stack.push(node->right);
    }
    
    if(node->left != nullptr){
      node->left->code(node->code() + "0");
      tree_stack.push(node->left);
    }
    
    if(node->left == nullptr && node->right == nullptr){
      node->code(node->parent->code());
      if(node == node->parent->left){
	node->code(node->code() + "0");
	node->parent->left = nullptr;
      }else if(node == node->parent->right){
	node->code(node->code() + "1");
	node->parent->right = nullptr;
      }

      if(node->is_leaf()){
	codes[node->get_byte()] = node->code();
      }
      
#ifdef DEBUG
      if(node->get_byte() != 0){
	std::cout << "leaf: " << *node << " code: " << node->code() << "\n";
      }
#endif
      tree_stack.pop();
      
    }
    node = tree_stack.top();    
  }
}

void Huffman::write_frequency(std::ofstream& output_file){
  for(size_t j = 0; j < frequency.size(); ++j){
    if(frequency[j]){
      output_file.write((char*) &j, sizeof(uchar));
      output_file.write((char*) &frequency[j], sizeof(int));
    }
  }
}
  
void Huffman::write_raw_message(std::ofstream& output_file){
  int byte_round = encode_message.size() / CHAR_BIT;
  uchar modulo = encode_message.size() % CHAR_BIT;

  output_file.write((char*) &byte_round, sizeof(byte_round));
  output_file.write((char*) &modulo, sizeof(uchar));

  // Записываем упакованное по 8 бит сообщение
  // Write the message dividing by 8 bits
  int i;
  for(i = 0; i < byte_round * CHAR_BIT; i += CHAR_BIT){
    std::bitset<CHAR_BIT> b(encode_message.substr(i, CHAR_BIT));
    uchar value = b.to_ulong();
    output_file.write((char*) &value, sizeof(uchar));
  }
  
  // Записываем лишние биты, которые не поместились в 8 бит
  // Write last bites 
  if(modulo > 0){
    std::bitset<CHAR_BIT> b(encode_message.substr(i, modulo));
    uchar value = b.to_ulong();
    output_file.write((char*) &value, sizeof(char));
  }
}

int Huffman::write_encoding_file(){
  out_file_name = file_name + TYPE;
  
  std::ifstream input_file (file_name, std::ifstream::binary);
  std::ofstream output_file(out_file_name, std::ofstream::binary);

  if(!input_file){
    std::cerr << "File opening error: " << file_name << std::endl; 
    return -1;
  }
  
  message2code(input_file);

  uchar count = count_if(frequency.begin(), frequency.end(), [](const auto& value) { return value != 0; } );

  // Первым делом записываем количество не нулевых частот символов
  // First of all need to write number of not null frequencies
  output_file.write((char*) &count, sizeof(count));

#ifdef DEBUG
  std::cout << "Write frequency..." << std::endl;
#endif

  write_frequency(output_file);

#ifdef DEBUG
  std::cout << "Write message..." << std::endl;
#endif

  write_raw_message(output_file);

  input_file.close();
  output_file.close();

  new_file_size = get_filesize(out_file_name);
  if(new_file_size < 0){
    std::cerr << "Can't get file size." << std::endl;
    return -1;
  }

#ifdef DEBUG
  float ratio = ((float)new_file_size * 100 / file_size);
  if(ratio < 100){
    std::cout << "Encoding's completed: " << out_file_name << ". Compressing is good: " << std::setprecision(3)
	      << ((float)new_file_size * 100 / file_size) << "%" << std::endl;
  }else{
    std::cout << "Encoding's completed: " << out_file_name << ". Compressing is bad: " << std::setprecision(3)
	      << ((float)new_file_size * 100 / file_size) << "%" << std::endl;
  }
#endif
  return 0;
}

void Huffman::read_frequency(std::ifstream& input_file, uchar& count){
  uchar i = 0;
  while(i < count){
    char index;
    input_file.read (&index, sizeof(index));

    int f;
    input_file.read (reinterpret_cast<char*>(&f), sizeof(int));

    frequency[static_cast<uchar>(index)] = f;
    ++i;

#ifdef DEBUG
    std::cout << "\rReading encoding frequency " << file_name << ": " << (int)(i*100.0/count) << "%" << std::flush;
#endif
  }

#ifdef DEBUG
  std::cout << std::endl;
#endif

}

void Huffman::read_raw_message(std::ifstream& input_file){
  int byte_round = 0;
  input_file.read (reinterpret_cast<char*>(&byte_round), sizeof(int));

  // Read counter (remainder after dividing by CHAR_BIT
  char modulo;
  input_file.read (&modulo, sizeof(modulo));

  std::stringstream ss;
  size_t entire_block_size = byte_round * CHAR_BIT;
  while(decode_message.size() < entire_block_size + modulo){
    // Read the message
    while(decode_message.size() < entire_block_size){
      char ch;
      input_file.read(&ch, sizeof (ch));
      
      std::bitset<CHAR_BIT> b(ch);
      
      decode_message += b.to_string();
    }
    
    // Read remainder
    char ch;
    input_file.read(&ch, sizeof(ch));
    std::bitset<CHAR_BIT> b(ch);
    decode_message += (b.to_string()).substr(CHAR_BIT - modulo, CHAR_BIT); 
  }
  std::cout << std::endl;
}

void Huffman::check_file_name(){
  out_file_name = file_name.substr(0, file_name.size() - TYPE.size());
  out_file_name += ".1";
  std::cout << "Output file: " << out_file_name << std::endl;
}

int Huffman::read_decoding_file(){
  std::ifstream input_file (file_name, std::ifstream::binary);
  if (!input_file) {
    std::cerr << "File opening error: " << file_name << std::endl;
    return -1;
  }

  uchar count = 0;
  input_file.read (reinterpret_cast<char*>(&count), sizeof(count));

  read_frequency(input_file, count);  
  read_raw_message(input_file);
  return 0;
}


void Huffman::codes2chs(){
  Node::pointer node = root;

  auto found_the_letter = [this](Node::pointer& n){
    message += n->get_byte();
    n = root;
  };
  
  for(size_t i = 0; i < decode_message.size(); ++i){
    char ch = decode_message[i];
    if(ch == '0'){
      if(node->left != nullptr){
	node = node->left;
	if(node->is_leaf()){
	  found_the_letter(node);
	}
      }
    }else if(ch == '1'){
      if(node->right != nullptr){
	node = node->right;
	if(node->is_leaf()){
	  found_the_letter(node);
	}
      }
    }
#ifdef DEBUG
    std::cout << "\rReading decoding " << file_name << ": " << (int)(((i+1) * 100.0)/decode_message.size()) << "%" << std::flush;
#endif
    
  }
#ifdef DEBUG
  std::cout << std::endl;
#endif
}

int Huffman::write_decoding_file(){
  std::ofstream output_file(out_file_name);
  if(!output_file){
    std::cerr << "Error in opening output file." << std::endl;
    return -1;
  }
  
  output_file << message;
  output_file.close();
  
  new_file_size = get_filesize(out_file_name);
  if(new_file_size < 0){
    std::cerr << "Can't get file size." << std::endl;
    return -1;
  }

#ifdef DEBUG
  std::cout << "Decoding complete: " << out_file_name << ". Uncompressing: " << std::setprecision(3)
	    << ((float)new_file_size * 100 / file_size) << "%" << std::endl;
#endif
  return 0;
}


void Huffman::gui_compress_result(){
  hw->outfile_box->show();
  hw->size_box2->show();
  hw->info->show();
    
  std::size_t found = out_file_name.find_last_of("/\\");
    
  std::stringstream ss;
  ss << "Output file name: " << out_file_name.substr(found + 1);
    
  hw->outfile_box->copy_label(ss.str().c_str());
  ss.str("");
  ss << "Output file size: " << new_file_size << " Kb";
  hw->size_box2->copy_label(ss.str().c_str());

  ss.str("");
  ratio = ((float)new_file_size * 100 / file_size);
  ss << "Efficiency: " << ratio << " %";
    
  hw->info->copy_label(ss.str().c_str());
}

void Huffman::gui_decompress_result(){
  hw->outfile_box->show();
  hw->size_box2->show();
  hw->info->show();
    
  size_t found = out_file_name.find_last_of("/\\");
    
  std::stringstream ss;
  ss << "Output file name: " << out_file_name.substr(found + 1);
    
  hw->outfile_box->copy_label(ss.str().c_str());
  ss.str("");
  ss << "Output file size: " << new_file_size << " Kb";
  hw->size_box2->copy_label(ss.str().c_str());

  ss.str("");
  ratio = ((float)file_size * 100 / new_file_size);
  ss << "Efficiency: " << ratio << " %";
    
  hw->info->copy_label(ss.str().c_str());
}

