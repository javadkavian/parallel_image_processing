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
    float red;
    float green;
    float blue;
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


#pragma pack(pop)


#endif