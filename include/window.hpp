#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <memory>
#include <string>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Progress.H>

void load_file(Fl_Widget* w, void* data);
void compress_file(Fl_Widget* w, void* data);
void decompress_file(Fl_Widget* w, void* data);

class Huffman_window: public Fl_Window{
  
  const int space         {10};
  const int button_height {30};
  const int width         {400};
  const int height        {310};

public:
  using Box_ptr           = std::unique_ptr<Fl_Box>;
  using Button_ptr        = std::unique_ptr<Fl_Button>;
  using Return_Button_ptr = std::unique_ptr<Fl_Return_Button>;
  using Progress_ptr      = std::unique_ptr<Fl_Progress>;
  using FileChooser_ptr   = std::unique_ptr<Fl_File_Chooser>;

  Box_ptr size_box1;
  Box_ptr size_box2;
  Box_ptr outfile_box;

  Box_ptr info;

  Progress_ptr progress;
  
  Button_ptr        load_button;
  Button_ptr        unzip_button;
  Return_Button_ptr zip_button;

  std::string file_name;
  
  Huffman_window(int w, int h, const char *label = 0L)
    : Fl_Window(w, h, label)
  {
    Fl::scheme("gtk+");
    initialization();
  }
  
  virtual ~Huffman_window() = default;
  
  void initialization();
  void set_name(std::string name){
    file_name = name;
  }
};

#endif //WINDOW_H
