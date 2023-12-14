#include "include/bmp.hpp"
#include "include/globals.hpp"


using namespace std;




int main(int argc, char* argv[]) {
    Bmp* bmp = new Bmp(argv[1]);
    bmp -> run();
    return 0;
}