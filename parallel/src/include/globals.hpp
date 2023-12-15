#ifndef __GLOBAL__
#define __GLOBAL__



typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

#pragma pack(push, 1)
typedef struct tagBITMAPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct pixel{
    int red;
    int green;
    int blue;
    pixel(int r, int g, int b){
        red = std :: max(0, std :: min(255,r));
        green = std :: max(0, std :: min(255,g));
        blue = std :: max(0, std :: min(255,b));
    }
    pixel(){
        red = 0;
        green = 0;
        blue = 0;
    }
} Pixel;

typedef struct args{
    char* img_buffer;
    int* rows;
    int* cols;
    int* buffer_size;
    int begin_r;
    int end_r;
    int begin_c;
    int end_c;
    std :: vector < std :: vector <pixel>>* pixels;
    std :: vector < std :: vector <pixel>>* tmp;
    std :: vector<std :: vector<int>> blur = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
    args(char* img_buf, int* rows_, int* cols_, int* buf_size,
        int begin_r_, int end_r_, int begin_c_, int end_c_,
        std :: vector < std :: vector <pixel>>* pixels_,
        std :: vector < std :: vector <pixel>>* tmp_)
    {
        img_buffer = img_buf;
        rows = rows_;
        cols = cols_;
        buffer_size = buf_size;
        begin_r = begin_r_;
        end_r = end_r_;
        begin_c = begin_c_;
        end_c = end_c_;
        pixels = pixels_;
        tmp = tmp_;
    }
}args;

#pragma pack(pop)


#endif