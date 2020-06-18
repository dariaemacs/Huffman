#include "huffman.hpp"
#include "window.hpp"
#include "auxiliary.hpp"

#include <iostream>
#include <fstream>
#include <bitset>
#include <thread>

#include <FL/Fl.H>

Huffman::Huffman(const std::string name, Huffman_window* w) :
  hw(w), file_name(name), frequency(0x100, 0), codes(0x100, "")
{
  file_size = get_filesize(file_name);
  
  std::stringstream ss;
  ss << "File size: " << file_size << " Kb";
  hw->size_box1->copy_label(ss.str().c_str());
}

void Huffman::compress(){    
  std::cout << "============ Compress ===========\n";

  encode_file();

  fill_queue();

  build_tree();
  root = queue.top();

  recursive_chs2codes(root, "");

  write_encoding_file();

  gui_compress_result();
    
  std::cout << "=================================\n";
}

void Huffman::decompress(){
  std::cout << "============ Decompress =========\n";

  check_file_name();
    
  decode_message = "";
  read_decoding_file();
    
  fill_queue();
    
  build_tree();
  root = queue.top();

  codes2chs();
    
  write_decoding_file();

  gui_decompress_result();

  std::cout << "=================================\n";
}


void Huffman::gui_set_progress(int value){
  std::stringstream ss;
  ss << value << "%";
  hw->progress->value(value);
  hw->progress->copy_label(ss.str().c_str());
}

void Huffman::encode_file() {
  std::ifstream is (file_name, std::ifstream::binary);
  if (!is) {
    std::cerr << "File opening error: " << file_name; 
    return;
  }
  
  int i = 0;
  int file_step = 1000;
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

  std::cout << std::endl;

#ifdef DEBUG
  print(frequency);
#endif
}

void Huffman::fill_queue(){
  for(size_t i = 0; i < frequency.size(); ++i){
    if(frequency[i] != 0){
      pointer node = std::make_shared<Node>(static_cast<uchar>(i), frequency[i]);
      queue.push(node);
    }
  }

  if (queue.empty()) {
    std::cerr << "The queue is empty: " << __PRETTY_FUNCTION__ << std::endl; 
    return;
  }
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

      if(node->is_single()){
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
  int byte_round = encode_message.size() / BYTE_SIZE;
  uchar modulo = encode_message.size() - byte_round * BYTE_SIZE;

  output_file.write((char*) &byte_round, sizeof(byte_round));
  output_file.write((char*) &modulo, sizeof(uchar));

  // Записываем упакованное по 8 бит сообщение
  // Write the message dividing by 8 bits
  int i;
  for(i = 0; i < byte_round * BYTE_SIZE; i += BYTE_SIZE){
    std::bitset<BYTE_SIZE> b(encode_message.substr(i, BYTE_SIZE));
    uchar value = b.to_ulong();
    output_file.write((char*) &value, sizeof(uchar));
  }
  
  // Записываем лишние биты, которые не поместились в 8 бит
  // Write last bites 
  if(modulo > 0){
    std::bitset<BYTE_SIZE> b(encode_message.substr(i, modulo));
    uchar value = b.to_ulong();
    output_file.write((char*) &value, sizeof(char));
  }
}

void Huffman::write_encoding_file(){
  out_file_name = file_name + TYPE;
  
  std::ifstream input_file (file_name, std::ifstream::binary);
  std::ofstream output_file(out_file_name, std::ofstream::binary);

  if(!input_file){
    std::cerr << "File opening error: " << file_name; 
    return;
  }
  
  message2code(input_file);

  int count = 0;
  for(int i = 0; i < 0x100; ++i){
    if(frequency[i] != 0){
      ++count;
    }
  }

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
#ifdef DEBUG
  ratio = ((float)new_file_size * 100 / file_size);
  if(ratio < 100){
    std::cout << "Encoding's completed: " << out_file_name << ". Compressing is good: " << std::setprecision(3)
	      << ((float)new_file_size * 100 / file_size) << "%" << std::endl;
  }else{
    std::cout << "Encoding's completed: " << out_file_name << ". Compressing is bad: " << std::setprecision(3)
	      << ((float)new_file_size * 100 / file_size) << "%" << std::endl;
  }
#endif
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

void Huffman::read_frequency(std::ifstream& input_file, int& count){
  int i = 0;
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

  // Read counter (remainder after dividing by BYTE_SIZE
  char modulo;
  input_file.read (&modulo, sizeof(modulo));

  std::stringstream ss;
  size_t entire_block_size = byte_round * BYTE_SIZE;
  while(decode_message.size() < entire_block_size + modulo){
    // Read the message
    while(decode_message.size() < entire_block_size){
      char ch;
      input_file.read(&ch, sizeof (ch));
      
      std::bitset<BYTE_SIZE> b(ch);
      
      decode_message += b.to_string();
    }
    
    // Read remainder
    char ch;
    input_file.read(&ch, sizeof(ch));
    std::bitset<BYTE_SIZE> b(ch);
    decode_message += (b.to_string()).substr(BYTE_SIZE - modulo, BYTE_SIZE); 
  }
  std::cout << std::endl;
}

void Huffman::check_file_name(){
  const std::regex hff_regex("^[A-Za-z0-9-_.]+\\" + TYPE);
  std::size_t found = file_name.find_last_of("/\\");
  
  std::stringstream ss;
  ss << "File name: " << file_name.substr(found + 1);
  std::string tmp_file_name = file_name.substr(found + 1);
    
  std::cout << "Does file name include .hff? :"
	    << std::regex_match(tmp_file_name, hff_regex) << " : " << tmp_file_name << std::endl;

  if(std::regex_match(tmp_file_name, hff_regex)){
    out_file_name = file_name.substr(0, found + 1) + tmp_file_name.substr(0, tmp_file_name.size() - 4);
    out_file_name += ".1";
  }else{
    out_file_name = file_name.substr(0, found) + tmp_file_name + ".1";
    file_name = file_name.substr(0, found) + tmp_file_name + ".hff";
  }
}

void Huffman::read_decoding_file(){
  std::ifstream input_file (file_name, std::ifstream::binary);
  if (!input_file) {
    std::cerr << "File opening error: " << file_name; 
    return;
  }

  int count = 0;
  input_file.read (reinterpret_cast<char*>(&count), sizeof(int));

  read_frequency(input_file, count);  
  read_raw_message(input_file);
}


void Huffman::codes2chs(){
  std::stack<pointer> tree_stack;
  tree_stack.push(root);
    
  pointer node = tree_stack.top();

  encode_message = "";

  auto found_the_letter = [this, &node](pointer&){
    encode_message = "";
    message += node->get_char();
    node = root;
  };
  
  for(size_t i = 0; i < decode_message.size(); ++i){
    char ch = decode_message[i];
    if(ch == '0'){
      if(node->left != nullptr){
	node = node->left;
	encode_message += "0";
      }else{	
	if(node->left == nullptr && node->right == nullptr){
	  found_the_letter(node);
	  --i;
	}
      }
    }else{
      if(node->right != nullptr){
	node = node->right;
	encode_message += "1";
      }else{	
	if(node->left == nullptr && node->right == nullptr){
	  found_the_letter(node);
	  --i;
	}
      }
    }

    if(i == decode_message.size() - 1){
      if(node->left == nullptr && node->right == nullptr){
	found_the_letter(node);
	--i;
      } 
    }

    int value = (i + 1) * 100.0/decode_message.size();
    gui_set_progress(value);
    if(i % 20000 == 0){
      Fl::check();
    }
#ifdef DEBUG
    std::cout << "\rReading decoding " << file_name << ": " << (int)(((i+1) * 100.0)/decode_message.size()) << "%" << std::flush;
#endif

  }
#ifdef DEBUG
  std::cout << std::endl;
#endif
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

void Huffman::write_decoding_file(){
  std::ofstream output_file(out_file_name);
  output_file << message;
  output_file.close();

  new_file_size = get_filesize(out_file_name);
  
#ifdef DEBUG
  std::cout << "Decoding complete: " << out_file_name << ". Uncompressing: " << std::setprecision(3)
	    << ((float)new_file_size * 100 / file_size) << "%" << std::endl;
#endif
}
