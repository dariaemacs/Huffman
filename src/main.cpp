#include "window.hpp"
#include "huffman.hpp"

int main(int argc, char **argv) {
  auto window = std::make_unique<Huffman_window>(400, 310, "Huffman algorithm");

  window->end();
  window->show(argc, argv);

  return Fl::run();
}
