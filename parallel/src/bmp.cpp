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
    pixels = vector<vector<pixel>>(rows, vector<pixel>(cols));
    tmp_vector = vector <vector<pixel>>(rows, vector<pixel>(cols));

    buffer_size = file_header->bfSize;
    create_thread_args();
    auto end = chrono :: high_resolution_clock :: now();
    chrono :: duration<double> duration = end - start;
    cout << "find and allocate duration : " << duration.count() * 1000 << " ms" << endl;
    return true;
}


void Bmp :: create_thread_args(){
    int extra = cols%4;
    int offset = extra + 3*cols;
    args arg1(img_buffer, &rows, &cols, &buffer_size, 0, rows/4,0, cols/4, &pixels, &tmp_vector, 1);
    args arg2(img_buffer, &rows, &cols, &buffer_size, rows/4, rows/2, cols/4, cols/2, &pixels, &tmp_vector, 1 + (rows/4)*(offset));
    args arg3(img_buffer, &rows, &cols, &buffer_size, rows/2, 3*(rows/4), cols/2, 3*(cols/4), &pixels, &tmp_vector,1 + (rows/2)*(offset));
    args arg4(img_buffer, &rows, &cols, &buffer_size, 3*(rows/4), rows, 3*(cols/4), cols, &pixels, &tmp_vector,1 + 3*(rows/4)*(offset));
    thread_parameters = {arg1, arg2, arg3, arg4};
}



void* bounded_get_pixels(void* arg){
    args* arg_;
    arg_ = (args*)arg;  
    int count = arg_->count;
    int extra = *arg_->cols % 4;
    for (int i = arg_->begin_r; i < arg_->end_r; i++)
    {
        count += extra;
        for (int j = *arg_->cols - 1; j >= 0; j--)
        {
            for (int k = 0; k < 3; k++)
            {
                unsigned char channel = arg_->img_buffer[*arg_->buffer_size-count];
                count++;
                switch (k)
                {
                    case 0:
                        (*arg_->pixels)[i][j].green = int(channel);
                        break;
                    case 1:
                        (*arg_->pixels)[i][j].blue = int(channel);
                        break;
                    case 2:
                        (*arg_->pixels)[i][j].red = int(channel);
                        break;
                }
            }
        }
    }
}


void Bmp :: get_pixels_from_BMP24() {
    auto start = chrono :: high_resolution_clock :: now();
    pthread_t threads[4];
    for(int i = 0 ; i < 4 ; i++){
            pthread_create(&threads[i], nullptr, bounded_get_pixels, &thread_parameters[i]);
    }
    for(int i = 0 ; i < 4 ; i++){
        pthread_join(threads[i], nullptr);
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
    auto end = chrono :: high_resolution_clock :: now();
    chrono :: duration<double> duration = end - start;
    cout << "write duration : " << duration.count() * 1000 << " ms" << endl;
}


void* column_bounded_mirror(void* arg){
    args* arg_;
    arg_ = (args*)arg;
    for(int i = 0 ; i < *arg_->rows/2 ; i++){
        for(int j = arg_->begin_c ; j < arg_->end_c ; j++){
            pixel tmp = (*arg_->pixels)[i][j];
            (*arg_->pixels)[i][j] = (*arg_->pixels)[*arg_->rows - i - 1][j];
            (*arg_->pixels)[*arg_->rows - i - 1][j] = tmp;
        }
    }


}


void Bmp :: vertical_mirror() {
    auto start = chrono :: high_resolution_clock :: now();
    pthread_t threads[4];
    for(int i = 0 ; i < 4 ; i++){
            pthread_create(&threads[i], nullptr, column_bounded_mirror, &thread_parameters[i]);
    }
    for(int i = 0 ; i < 4 ; i++){
        pthread_join(threads[i], nullptr);
    }
    auto end = chrono :: high_resolution_clock :: now();
    chrono :: duration<double> duration = end - start;
    cout << "vertical mirror duration : " << duration.count() * 1000 << " ms" << endl;
}



void* bounded_purple_haze(void* arg) {
    args* arg_;
    arg_ = (args*)arg;
    for(int i = arg_->begin_r ; i < arg_->end_r ; i++){
        for(int j = 0 ; j < *arg_->cols ; j++){
            pixel tmp = (*arg_->pixels)[i][j];
            tmp.red = 0.5*(*arg_->pixels)[i][j].red + 0.3*(*arg_->pixels)[i][j].green + 0.5*(*arg_->pixels)[i][j].blue;
            if(tmp.red < 0){
                tmp.red = 0;
            }
            if(tmp.red > 255){
                tmp.red = 255;
            }
            tmp.green = 0.16*(*arg_->pixels)[i][j].red + 0.5*(*arg_->pixels)[i][j].green + 0.16*(*arg_->pixels)[i][j].blue;
            if(tmp.green < 0){
                tmp.green = 0;
            }
            if(tmp.green > 255){
                tmp.green = 255;
            }
            tmp.blue = 0.6*(*arg_->pixels)[i][j].red + 0.2*(*arg_->pixels)[i][j].green + 0.8*(*arg_->pixels)[i][j].blue;
            if(tmp.blue < 0){
                tmp.blue = 0;
            }
            if(tmp.blue > 255){
                tmp.blue = 255;
            }
            (*arg_->pixels)[i][j] = tmp;
        
        }
    }
}


void Bmp :: purple_haze() {
    auto start = chrono :: high_resolution_clock :: now();
    pthread_t threads[4];
    for(int i = 0 ; i < 4 ; i++){
            pthread_create(&threads[i], nullptr, bounded_purple_haze, &thread_parameters[i]);
    }
    for(int i = 0 ; i < 4 ; i++){
        pthread_join(threads[i], nullptr);
    }
    auto end = chrono :: high_resolution_clock :: now();
    chrono :: duration<double> duration = end - start;
    cout << "purple haze duration : " << duration.count() * 1000 << " ms" << endl;
}






pixel my_convolve(int row, int col, float coeff, void* arg){
    args* arg_;
    arg_ = (args*)arg;
    int red=0, green=0, blue=0;
    for(int i=-1; i<=1; i++){
        for(int j=-1; j<=1; j++){
            red += (*arg_->pixels)[row+i][col+j].red * (arg_->blur)[i+1][j+1];
            green += (*arg_->pixels)[row+i][col+j].green * (arg_->blur)[i+1][j+1];
            blue += (*arg_->pixels)[row+i][col+j].blue * (arg_->blur)[i+1][j+1];
        }
    }
    return {coeff*red, coeff*green, coeff*blue};
}

void* bounded_blur_filter(void* arg){
    args* arg_;
    arg_ = (args*)arg;
    for(int i = arg_->begin_r ; i < arg_->end_r ; i++){
        (*arg_->tmp)[i][0] = (*arg_->pixels)[i][0];
        (*arg_->tmp)[i][*arg_->cols - 1] = (*arg_->pixels)[i][*arg_->cols - 1];
    }
    for(int j = 0 ; j < *arg_->cols ; j++){
        (*arg_->tmp)[arg_->begin_r][j] = (*arg_->pixels)[arg_->begin_r][j];
        (*arg_->tmp)[arg_->end_r - 1][j] = (*arg_->pixels)[arg_->end_r-1][j];
    }
    for(int i = arg_->begin_r + 1 ; i < arg_->end_r - 1 ; i++){
        for(int j = 1 ; j < *arg_->cols - 1 ; j++){
            (*arg_->tmp)[i][j] = my_convolve(i, j, 0.0625, arg);
        }
    }
}


void Bmp :: blur_filter() {
    auto start = chrono :: high_resolution_clock :: now();
    pthread_t threads[4];
    for(int i = 0 ; i < 4 ; i++){
        pthread_create(&threads[i], nullptr, bounded_blur_filter, &thread_parameters[i]);
    }
    for(int i = 0 ; i < 4 ; i++){
        pthread_join(threads[i], nullptr);
    }
    pixels = tmp_vector;
    auto end = chrono :: high_resolution_clock :: now();
    chrono :: duration<double> duration = end - start;
    cout << "blur kernel duration : " << duration.count() * 1000 << " ms" << endl;
}


void* bounded_draw_line(void* arg){
    args* arg_;
    arg_ = (args*)arg;
    for(int i = arg_->begin_r ; i < arg_ -> end_r ; i++){
        for(int j = 0 ; j < *arg_->cols ; j++){
            if((i+j == *arg_->rows/2) || (i+j == *arg_->rows) || (i+j == 3*(*arg_->rows/2))){
                (*arg_->pixels)[i][j] = {255, 255, 255};
            }
        }
    }
}

void Bmp :: draw_line() {
    auto start = chrono :: high_resolution_clock :: now();
    pthread_t threads[4];
    for(int i = 0 ; i < 4 ; i++){
        pthread_create(&threads[i], nullptr, bounded_draw_line, &thread_parameters[i]);
    }
    for(int i = 0 ; i < 4 ; i++){
        pthread_join(threads[i], nullptr);
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