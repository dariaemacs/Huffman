#include "window.hpp"
#include "huffman.hpp"

void Huffman_window::initialization(){
  int y = space;
  progress = std::make_unique<Fl_Progress>(space, space, width - 2*space, button_height, "");
  progress->selection_color(0x4444ff00);
  progress->minimum(0);
  progress->maximum(100);
  progress->value(0);

  y += button_height + space/2;

  size_box1 = std::make_unique<Fl_Box>(space, y, width - 2*space, button_height, "File size: 0 Kb");
  size_box1->box(FL_NO_BOX);
  size_box1->labelfont(FL_COURIER);
  size_box1->labelsize(18);

  y += button_height + space;
    
  load_button = std::make_unique<Fl_Button>(width - 2.5*button_height - space, y, 2.5*button_height, button_height, "Load file");

  load_button->callback(load_file, this);

  y += button_height + space;

  outfile_box = std::make_unique<Fl_Box>(space, y, width - 2*space, button_height, "Output file name:");
  outfile_box->box(FL_NO_BOX);
  outfile_box->labelfont(FL_COURIER);
  outfile_box->labelsize(18);
  outfile_box->hide();
  
  y += button_height + space;

  size_box2 = std::make_unique<Fl_Box>(space, y, width - 2*space, button_height, "Output file size: 0 Kb");
  size_box2->box(FL_NO_BOX);
  size_box2->labelfont(FL_COURIER);
  size_box2->labelsize(18);
  size_box2->hide();
    
  y += button_height + space;

  info = std::make_unique<Fl_Box>(space, y, width - 2*space, button_height, "Efficiency: %");
  info->box(FL_NO_BOX);
  info->labelfont(FL_COURIER);
  info->labelsize(18);
  info->hide();

  y += button_height + space;
    
  int x = width - 7*button_height - 2*space;
  unzip_button = std::make_unique<Fl_Button>(x, height - button_height - space, 3.5*button_height, button_height, "Decompress");
  unzip_button->callback(decompress_file, this);
       
  x += 3.5*button_height + space;
  zip_button = std::make_unique<Fl_Return_Button>(x, height - button_height - space, 3.5*button_height, button_height, "Compress");
  zip_button->callback(compress_file, this);
    
}

void Huffman_window::set_name(std::string name){
  file_name = name;
}

void load_file(Fl_Widget* w, void* data){
  Huffman_window* hw = (Huffman_window*)data;

  std::cout << "Load file...\n";
  auto chooser = std::make_unique<Fl_File_Chooser>(".", "*", Fl_File_Chooser::SINGLE, "Load file");
  chooser->show();
  
  while(chooser->shown()) {
    Fl::wait();
  }
  hw->set_name(chooser->value());
  
  hw->progress->copy_label(hw->file_name.c_str());
  std::cout << "Name: " << hw->file_name << std::endl;
}

void compress_file(Fl_Widget* w, void* data){
  
  Huffman_window* hw = (Huffman_window*)data;
  Huffman huffman(hw->file_name, hw);
  
  huffman.compress();  
}

void decompress_file(Fl_Widget* w, void* data){
  
  Huffman_window* hw = (Huffman_window*)data;
  Huffman huffman(hw->file_name, hw);
  
  huffman.decompress();
}
