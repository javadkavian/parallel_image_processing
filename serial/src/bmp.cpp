#include "include/bmp.hpp"

using namespace std;



Bmp :: Bmp(char* file_name_)
          : file_name(file_name_)
          {}

bool Bmp :: find_and_allocate() {
    auto start = chrono :: high_resolution_clock :: now();
    ifstream file(file_name);
    if (!file) {
        std::cout << "File" << file_name << " doesn't exist!" << std::endl;
        return false;
    }

    file.seekg(0, ios::end);
    streampos length = file.tellg();
    file.seekg(0, ios::beg);

    img_buffer = new char[length];
    file.read(&img_buffer[0], length);

    PBITMAPFILEHEADER file_header;
    PBITMAPINFOHEADER info_header;

    file_header = (PBITMAPFILEHEADER)(&img_buffer[0]);
    info_header = (PBITMAPINFOHEADER)(&img_buffer[0] + sizeof(BITMAPFILEHEADER));
    rows = info_header->biHeight;
    cols = info_header->biWidth;
    buffer_size = file_header->bfSize;
    auto end = chrono :: high_resolution_clock :: now();
    chrono :: duration<double> duration = end - start;
    cout << "find and allocate duration : " << duration.count() * 1000 << " ms" << endl;
    return true;
}


void Bmp :: get_pixels_from_BMP24() {
    auto start = chrono :: high_resolution_clock :: now();
    int count = 1;
    int extra = cols % 4;
    pixels = vector<vector<pixel>>(rows, vector<pixel>(cols));
    for (int i = 0; i < rows; i++)
    {
        count += extra;
        for (int j = cols - 1; j >= 0; j--)
        {
            for (int k = 0; k < 3; k++)
            {
                unsigned char channel = img_buffer[buffer_size-count];
                count++;
                switch (k)
                {
                    case 0:
                        pixels[i][j].red = int(channel);
                        break;
                    case 1:
                        pixels[i][j].green = int(channel);
                        break;
                    case 2:
                        pixels[i][j].blue = int(channel);
                        break;
                }
            }
        }
    }
    auto end = chrono :: high_resolution_clock :: now();
    chrono :: duration<double> duration = end - start;
    cout << "get pixels duration : " << duration.count() * 1000 << " ms" << endl;
}



void Bmp :: write_out_BMP24() {
    auto start = chrono :: high_resolution_clock :: now();
    std::ofstream write(OUT_IMAGE_NAME);
    if (!write)
    {
        cout << "Failed to write " << OUT_IMAGE_NAME << endl;
        return;
    }
    int count = 0;
    int extra = cols % 4;
    for (int i = 0; i < rows; i++)
    {
        count += extra;
        for (int j = cols - 1; j >= 0; j--)
        {
            for (int k = 0; k < 3; k++)
            {
                count++;
                switch (k)
                {
                    case 0:
                        img_buffer[buffer_size - count] = char(pixels[i][j].green);
                        break;
                    case 1:
                        img_buffer[buffer_size - count] = char(pixels[i][j].blue);
                        break;
                    case 2:
                        img_buffer[buffer_size - count] = char(pixels[i][j].red);
                        break;
                }
            }
        }
    }
    write.write(img_buffer, buffer_size);
    auto end = chrono :: high_resolution_clock :: now();
    chrono :: duration<double> duration = end - start;
    cout << "write duration : " << duration.count() * 1000 << " ms" << endl;
}



void Bmp :: vertical_mirror() {
    auto start = chrono :: high_resolution_clock :: now();
    for(int i = 0 ; i < rows/2 ; i++){
        for(int j = 0 ; j < cols ; j++){
            pixel tmp = pixels[i][j];
            pixels[i][j] = pixels[rows - i - 1][j];
            pixels[rows - i - 1][j] = tmp;
        }
    }
    auto end = chrono :: high_resolution_clock :: now();
    chrono :: duration<double> duration = end - start;
    cout << "vertical mirror duration : " << duration.count() * 1000 << " ms" << endl;
}


void Bmp :: purple_haze() {
    auto start = chrono :: high_resolution_clock :: now();
    vector <vector<pixel>> tmp_pixels(rows, vector<pixel>(cols));
    for(int i = 0 ; i < rows ; i++){
        for(int j = 0 ; j < cols ; j++){
            tmp_pixels[i][j].red = 0.5*pixels[i][j].red + 0.3*pixels[i][j].green + 0.5*pixels[i][j].blue;
            if(tmp_pixels[i][j].red < 0){
                tmp_pixels[i][j].red = 0;
            }
            if(tmp_pixels[i][j].red > 255){
                tmp_pixels[i][j].red = 255;
            }
            tmp_pixels[i][j].green = 0.16*pixels[i][j].red + 0.5*pixels[i][j].green + 0.16*pixels[i][j].blue;
            if(tmp_pixels[i][j].green < 0){
                tmp_pixels[i][j].green = 0;
            }
            if(tmp_pixels[i][j].green > 255){
                tmp_pixels[i][j].green = 255;
            }
            tmp_pixels[i][j].blue = 0.6*pixels[i][j].red + 0.2*pixels[i][j].green + 0.8*pixels[i][j].blue;
            if(tmp_pixels[i][j].blue < 0){
                tmp_pixels[i][j].blue = 0;
            }
            if(tmp_pixels[i][j].blue > 255){
                tmp_pixels[i][j].blue = 255;
            }
        }
    }
    pixels = tmp_pixels;
    auto end = chrono :: high_resolution_clock :: now();
    chrono :: duration<double> duration = end - start;
    cout << "purple haze duration : " << duration.count() * 1000 << " ms" << endl;
}


pixel Bmp :: convolve(int row, int col, float coeff, vector<vector<int>>kernel){
    int red=0, green=0, blue=0;
    for(int i=-1; i<=1; i++){
        for(int j=-1; j<=1; j++){
            red += pixels[row+i][col+j].red * kernel[i+1][j+1];
            green += pixels[row+i][col+j].green * kernel[i+1][j+1];
            blue += pixels[row+i][col+j].blue * kernel[i+1][j+1];
        }
    }
    return {coeff*red, coeff*green, coeff*blue};
}


void Bmp :: blur_filter() {
    auto start = chrono :: high_resolution_clock :: now();
    vector <vector<pixel>> tmp(rows, vector<pixel>(cols));
    for(int i = 0 ; i < rows ; i++){
        tmp[i][0] = pixels[i][0];
        tmp[i][cols - 1] = pixels[i][cols - 1];
    }
    for(int j = 0 ; j < cols ; j++){
        tmp[0][j] = pixels[0][j];
        tmp[rows - 1][j] = pixels[rows-1][j];
    }
    for(int i = 1 ; i < rows - 1 ; i++){
        for(int j = 1 ; j < cols - 1 ; j++){
            tmp[i][j] = convolve(i, j, 0.0625, blur);
        }
    }
    pixels = tmp;
    auto end = chrono :: high_resolution_clock :: now();
    chrono :: duration<double> duration = end - start;
    cout << "blur kernel duration : " << duration.count() * 1000 << " ms" << endl;
}


void Bmp :: draw_line() {
    auto start = chrono :: high_resolution_clock :: now();
    for(int i = 0 ; i < rows ; i++){
        for(int j = 0 ; j < cols ; j++){
            if((i+j == rows/2) || (i+j == rows) || (i+j == 3*(rows/2))){
                pixels[i][j] = {255, 255, 255};
            }
        }
    }
    auto end = chrono :: high_resolution_clock :: now();
    chrono :: duration<double> duration = end - start;
    cout << "draw line duration : " << duration.count() * 1000 << " ms" << endl;
}


void Bmp :: run() {
    find_and_allocate();
    get_pixels_from_BMP24();
    vertical_mirror();
    blur_filter();
    purple_haze();
    draw_line();
    write_out_BMP24();
}