#include <iostream>
#include <fstream>
#include <limits>
#include <time.h>
#include <cstdlib>
#include <chrono>
#include "src/Channel.h"
using namespace std;

int main(int argc, char* argv[]){

    srand(time(NULL));
    ifstream    input(argv[1]);
    ofstream    output(argv[2]);

    if(!input.is_open()){
        cerr << "Error opening file" << endl;
        exit(1);
    }

    std::stringstream buffer;
    buffer << input.rdbuf();
    input.close();

    Channel* channel = parseChannelInstance(buffer);
    channel->createNetInfo();
    channel->createVCG();
    channel->constructTracks();
    channel->allocateNet();
    outputRoutingResult(output, channel);

    return 0;   
}