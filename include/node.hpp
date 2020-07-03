#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <memory>

using uchar = unsigned char;

class Node {
public:
  using pointer = std::shared_ptr<Node>;
  
  pointer parent {nullptr}; // указатель на родителя, нужен только для расшифровки 
  pointer left   {nullptr}; // левый ребенок
  pointer right  {nullptr}; // правый ребенок

  Node() = default;
  Node(uchar uch, int f) : ch(uch), frequency(f) {}
  Node(const std::string& n, int f) : name(n), frequency(f) {}

  int operator + (const Node& rth) const {
    return frequency + rth.frequency;
  }

  friend std::ostream & operator<<(std::ostream & os, const Node& t);  
  
  int get_frequency() const {
    return frequency;
  }

  bool is_single() const {
    return ch != 0;
  }

  uchar get_byte() const {
    return ch;
  }

  char get_char() const {
    return static_cast<char>(ch);
  }

  std::string get_name() const {
    if(is_single()){
      return std::string(1, get_char());
    }else{
      return name;
    }
  }

  std::string code() const {
    return code_string;
  }

  void code(const std::string& c){
    code_string = c;
  }
  
private:
  std::string name{""};

  uchar ch                {0};    // number of byte 
  int   frequency         {0};    // byte frequency  
  std::string code_string {""};   // string of Huffman codes
};

using pointer = Node::pointer;

inline std::ostream & operator<<(std::ostream & os, const Node& node){
  if(node.is_single()){
    if(node.ch == 10){
      return os << "[\\n] = " << node.frequency;
    }else{
      return os << "[" << node.ch << "] = " << node.frequency;
    }
  }else{
    return os << "[" << node.name << "] = " << node.frequency;
  }
}

class Greater{
public:
  bool operator()(const pointer& lth, const pointer& rth) const{
    return lth->get_frequency() > rth->get_frequency();
  }
};

#endif //NODE_H
