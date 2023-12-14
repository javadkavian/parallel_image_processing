#include "include/bmp.hpp"
#include "include/globals.hpp"
#include <chrono>


using namespace std;




int main(int argc, char* argv[]) {
    auto start = chrono :: high_resolution_clock :: now();
    Bmp* bmp = new Bmp(argv[1]);
    bmp -> run();
    auto end = chrono :: high_resolution_clock :: now();
    chrono :: duration<double> duration = end - start;
    cout << "total duration : " << duration.count() * 1000 << " ms" << endl;
    return 0;
}