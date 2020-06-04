#ifndef TREE_H
#define TREE_H

#include <iostream>

using uchar = unsigned char;

class Tree {
public:
  using pointer = std::shared_ptr<Tree>;
  
  pointer parent  {nullptr}; // указатель на родителя, нужен только для расшифровки 
  pointer left  {nullptr};   // левый ребенок
  pointer right {nullptr};   // правый ребенок

  uchar byte_number {0};     // номер символа 
  int frequency {0};         // частота этого символа/символов  

  std::string code {""};     // кодовая строка из 0 и 1
  std::string bytes {""};    // 

  bool operator<(const Tree& rth){
    return frequency < rth.frequency;
  }

  Tree() {}
  Tree(uchar b, int f) : byte_number(b), frequency(f) {}
  
private:
  friend std::ostream & operator<<(std::ostream & os, const Tree& t);
};

using pointer = Tree::pointer;

inline std::ostream & operator<<(std::ostream & os, const Tree& t){
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

#endif //TREE_H
