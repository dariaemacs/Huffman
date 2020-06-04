#ifndef AUXILIARY_H
#define AUXILIARY_H

#include <iostream>
#include <sstream>
#include <fstream>

inline std::ifstream::pos_type get_filesize(std::string& filename){
  std::ifstream in(filename, std::ifstream::ate);
  return in.tellg();
}

template<class T>
void print(std::vector<T>& v ){
  for(int i = 0; i < v.size(); ++i){
    std::cout << "[" << i << "] = " << v[i] << ",\t ";
  }
  std::cout << std::endl;
}

template<typename T> void print_queue(T& queue) {
  while(!queue.empty()) {
    std::cout << *(queue.top()) << " ";
    queue.pop();
  }
  std::cout << std::endl;
}

#endif //AUXILIARY_H
