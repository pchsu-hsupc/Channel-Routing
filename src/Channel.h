#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <array>
#include <utility>
#include <fstream>
#include <sstream>
#include <algorithm>

typedef struct NetInfo_{
    /* data */
    std::pair<size_t, size_t>   StartPoint_; // first: x-coordinates, second: top 1 or bottom 0
    std::pair<size_t, size_t>   EndPoint_;
    std::string                 TrackName_ = "";
} NetInfo;


class Channel
{
public:
    /* data */
    std::unordered_map<std::string, std::vector<std::array<size_t, 2>>>     TopBoundaryLine_;
    std::unordered_map<std::string, std::vector<std::array<size_t, 2>>>     BottomBoundaryLine_;
    std::vector<size_t>                                                     TopNetIDs_;
    std::vector<size_t>                                                     BottomNetIDs_;
    std::unordered_map<std::string, NetInfo>                                NetsInfo_;
    std::unordered_map<std::string, std::unordered_map<std::string, int>>   VCG_; // VCG_[n1][n2] = 1, means n1 -> n2, -1 means n1 <- n2
    std::unordered_map<std::string, std::vector<std::array<size_t, 2>>>     TracksInfo_; // available intervals
    size_t                                                                  NumPins_ = 0;
    size_t                                                                  NumTopTracks_ = 0;
    size_t                                                                  NumButtomTracks_ = 0;
    size_t                                                                  NumAddedTracks_ = 0;
    /* member functions */
    
    /* constructor & distructor */
    Channel();
    ~Channel();

    /* member functions */
    void createNetInfo();
    void createVCG();
    void constructTracks();
    void allocateNet();
};

Channel* parseChannelInstance(std::ifstream& input);
void deleteEdges(std::unordered_map<std::string, std::unordered_map<std::string, int>>& VCG, const std::string& NetName);
bool allValuesNotMinusOne(const std::unordered_map<std::string, std::unordered_map<std::string, int>>& VCG, const std::string& NetName);
bool allValuesNotOne(const std::unordered_map<std::string, std::unordered_map<std::string, int>>& VCG, const std::string& NetName);
std::vector<std::pair<size_t, size_t>> findAllIndices(const std::vector<size_t>& vec1, const std::vector<size_t>& vec2, int value);
void updateInterval(std::vector<std::array<size_t, 2>>& intervals, const std::array<size_t, 2>& TrackSec);