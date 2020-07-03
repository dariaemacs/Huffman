#include "huffman.hpp"

const std::string compress = "compress";
const std::string decompress = "decompress";

int main(int argc, char **argv) {
  if (argc < 3 || argv[1][0] != '-') {
    std::cerr << "Usage: " << argv[0] << " -" << compress << "[-c] file.txt\n"
	      << "       " << argv[0] << " -" <<  decompress << "[-d] file.hff" << std::endl;
    return 1;
  }
  
  std::string arg1(argv[1]);
  arg1 = arg1.substr(1, arg1.size());

  std::string arg2(argv[2]);
  
  if(compress ==  arg1|| arg1 == "c"){
    std::cout << "Compress file: " << arg2 << "\n";
    
    Huffman huffman(arg2);
    int ret = huffman.compress();  
    if(ret == -1){
      std::cerr << "Compress file failed." << std::endl;
    }

  }else if(decompress == arg1 || arg1 == "d"){
    if(arg2.substr(arg2.size() - 4, arg2.size()) != Huffman::TYPE){
      std::cerr << "Wrong file format. It must end .hff: " << argv[2] << std::endl;
      return 1;
    }
    std::cout << "Decompress file: " << arg2 << "\n";

    Huffman huffman(arg2);
    int ret = huffman.decompress();
    if(ret == -1){
      std::cerr << "Decompress file failed." << std::endl;
    }
  }else{
    std::cerr << "Unknown parameters: " << argv[1] << " " << argv[2] << std::endl;
    return 1;
  }
}
