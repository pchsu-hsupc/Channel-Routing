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

struct NetInfo{
    /* data */
    std::pair<size_t, size_t>   StartPoint_; // first: x-coordinates, second: top 1 or bottom 0
    std::pair<size_t, size_t>   EndPoint_;
    std::string                 TrackName_ = "";

    NetInfo() = default;
    NetInfo( const std::pair<size_t, size_t>& StartPoint, const std::pair<size_t, size_t>& EndPoint, std::string TrackName = ""):
        StartPoint_(StartPoint), EndPoint_(EndPoint), TrackName_(TrackName) {}
};

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
    size_t                                                                  NumNets_ = 0;
    size_t                                                                  NumTopTracks_ = 0;
    size_t                                                                  NumButtomTracks_ = 0;
    size_t                                                                  NumAddedTracks_ = 0;
    
    /* constructor & distructor */
    Channel();
    ~Channel();

    /* copy assignment operator */
    Channel& operator=(const Channel& rhs){
        if(this == &rhs)
            return *this;
        TopBoundaryLine_ = rhs.TopBoundaryLine_;
        BottomBoundaryLine_ = rhs.BottomBoundaryLine_;
        TopNetIDs_ = rhs.TopNetIDs_;
        BottomNetIDs_ = rhs.BottomNetIDs_;
        NetsInfo_ = rhs.NetsInfo_;
        VCG_ = rhs.VCG_;
        TracksInfo_ = rhs.TracksInfo_;
        NumPins_ = rhs.NumPins_;
        NumNets_ = rhs.NumNets_;
        NumTopTracks_ = rhs.NumTopTracks_;
        NumButtomTracks_ = rhs.NumButtomTracks_;
        return *this;
    };

    /* member functions */
    void createNetInfo();
    void createVCG();
    void constructTracks();
    size_t allocateNet(bool isTopDown);
};

Channel* parseChannelInstance(std::stringstream& input);
void outputRoutingResult(std::ofstream& outputfile, Channel* channel, bool isTopDown);
void deleteEdges(std::unordered_map<std::string, std::unordered_map<std::string, int>>& VCG, const std::string& NetName);
bool allValuesNotMinusOne(const std::unordered_map<std::string, std::unordered_map<std::string, int>>& VCG, const std::string& NetName);
bool allValuesNotOne(const std::unordered_map<std::string, std::unordered_map<std::string, int>>& VCG, const std::string& NetName);
std::vector<std::pair<size_t, size_t>> findAllIndices(const std::vector<size_t>& vec1, const std::vector<size_t>& vec2, int value);
bool updateInterval(std::vector<std::array<size_t, 2>>& intervals, const std::array<size_t, 2>& TrackSec);
// void updateInterval(std::vector<std::array<size_t, 2>>& intervals, const std::array<size_t, 2>& TrackSec, size_t& index);
std::vector<std::string> extractSameSeriesNames(const std::unordered_map<std::string, NetInfo>& NetsInfo, const std::string& series);
bool checkSameNetSeries(const std::string& NetName1, const std::string& NetName2);