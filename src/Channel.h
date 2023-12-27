#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <array>
#include <fstream>
#include <sstream>

typedef struct NetInfo_{
    /* data */
    std::string  TrackName_ = "";
    size_t       Start_ = 0;
    size_t       End_ = 0;
} NetInfo;


class Channel
{
public:
    /* data */
    std::unordered_map<std::string, std::array<size_t, 2>> TopBoundaryLine_;
    std::unordered_map<std::string, std::array<size_t, 2>> BottomBoundaryLine_;
    std::vector<size_t>                                    TopNetIDs_;
    std::vector<size_t>                                    BottomNetIDs_;
    std::unordered_map<size_t, NetInfo>                    NetsInfo_;
    size_t                                                 NumTracks_ = 0;

    /* member functions */
    
    /* constructor & distructor */
    Channel();
    ~Channel();


};

Channel* parseChannelInstance(std::ifstream& input);
