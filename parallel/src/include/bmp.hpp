#ifndef __BMP__
#define __BMP__

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <chrono>
#include <pthread.h>
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
        std :: vector < std :: vector <pixel>> tmp_vector;
        std :: vector<std :: vector<int>> blur = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
        std :: vector<args> thread_parameters;
    public:
        Bmp(char* file_name_);
        void create_thread_args();
        bool find_and_allocate();
        void get_pixels_from_BMP24();
        void write_out_BMP24();
        void vertical_mirror();
        void blur_filter();
        void purple_haze();
        void draw_line();
        void run();

};






#endif