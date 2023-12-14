#ifndef __BMP__
#define __BMP__

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "globals.hpp"



#define OUT_IMAGE_NAME "filtered_img.bmp"


class Bmp{
    private:
        char* img_buffer;
        char* file_name;
        int rows;
        int cols;
        int buffer_size;
        std :: vector < std :: vector <pixel>> pixels;
    public:
        Bmp(char* file_name_);
        bool find_and_allocate();
        void get_pixels_from_BMP24();
        void write_out_BMP24();
        void run();

};






#endif