#include<iostream>
#include<fstream>
#include <limits>
#include <time.h>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[]){

    srand(time(NULL));
    ifstream    input(argv[1]);
    ofstream    output(argv[2]);

    if(!input.is_open()){
        cerr << "Error opening file" << endl;
        exit(1);
    }

    std::cout << "Hello World!" << std::endl;  

    return 0;   
}