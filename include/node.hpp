#ifndef NODE_H
#define NODE_H

#include <iostream>

using uchar = unsigned char;

class Node {
public:
  using pointer = std::shared_ptr<Node>;
  
  pointer parent  {nullptr}; // указатель на родителя, нужен только для расшифровки 
  pointer left  {nullptr};   // левый ребенок
  pointer right {nullptr};   // правый ребенок

  uchar byte_number {0};     // номер символа 
  int frequency {0};         // частота этого символа/символов  

  std::string code {""};     // кодовая строка из 0 и 1
  std::string bytes {""};    // 

  bool operator<(const Node& rth){
    return frequency < rth.frequency;
  }

  Node() {}
  Node(uchar b, int f) : byte_number(b), frequency(f) {}
  
private:
  friend std::ostream & operator<<(std::ostream & os, const Node& t);
};

using pointer = Node::pointer;

inline std::ostream & operator<<(std::ostream & os, const Node& t){
  if(t.byte_number == 0){
    return os << t.frequency << " [" << t.bytes << "]";
  }else{
    return os << t.frequency << " [" << t.byte_number << "]";
  }
}

class Greater{
public:
  bool operator()(const pointer& lth, const pointer& rth) const{
    return lth->frequency > rth->frequency;
  }
};

#endif //NODE_H
