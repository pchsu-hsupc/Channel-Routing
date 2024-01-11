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

    bool isTopDown = true;
    size_t rTopDown = 0;
    size_t rBottomUp = 0;

    Channel* channelTopDown = parseChannelInstance(buffer);
    channelTopDown->createNetInfo();
    channelTopDown->createVCG();
    channelTopDown->constructTracks();

    Channel* channelBottomUp = new Channel(*channelTopDown);

    rTopDown = channelTopDown->allocateNet(isTopDown);
    rBottomUp = channelBottomUp->allocateNet(!isTopDown);

    if(rTopDown < rBottomUp)
        outputRoutingResult(output, channelTopDown, isTopDown);
    else
        outputRoutingResult(output, channelBottomUp, !isTopDown);

    return 0;   
}