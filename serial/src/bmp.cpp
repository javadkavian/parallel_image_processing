#include "include/bmp.hpp"

using namespace std;



Bmp :: Bmp(char* file_name_)
          : file_name(file_name_)
          {}

bool Bmp :: find_and_allocate() {
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
    return true;
}


void Bmp :: get_pixels_from_BMP24() {
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
}



void Bmp :: write_out_BMP24() {
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
                        img_buffer[buffer_size - count] = char(pixels[i][j].red);
                        break;
                    case 1:
                        img_buffer[buffer_size - count] = char(pixels[i][j].green);
                        break;
                    case 2:
                        img_buffer[buffer_size - count] = char(pixels[i][j].blue);
                        break;
                }
            }
        }
    }
    write.write(img_buffer, buffer_size);
}




void Bmp :: run() {
    find_and_allocate();
    get_pixels_from_BMP24();
    write_out_BMP24();
}