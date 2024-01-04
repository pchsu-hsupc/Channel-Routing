#include "Channel.h"

Channel::Channel(/* args */)
{
}

Channel::~Channel()
{
}

void Channel::createNetInfo(){
    /* parse TopNetIDs_ and BottomNetIDs to 1, 2a, 2b, ...*/
    for(size_t n = 1; n <= TopNetIDs_.size(); ++n){
        std::vector<std::pair<size_t, size_t>> indices = findAllIndices(TopNetIDs_, BottomNetIDs_, n);
        if (indices.size() == 0) break;
        for(size_t i = 0; i <= indices.size() - 2; i++){
            std::string NetName = std::to_string(n);
            if(indices.size() > 2){
                NetName += "_" + std::to_string(i + 1);
            }
            NetInfo netInfo;
            netInfo.StartPoint_ = indices[i];
            netInfo.EndPoint_ = indices[i + 1];
            NetsInfo_[NetName] = netInfo;
        }
        NumNets_++;
    }
}

void Channel::createVCG(){
    /* create VCG */
    for(auto& netInfo : NetsInfo_){
        std::string NetName = netInfo.first;
        size_t Start = netInfo.second.StartPoint_.first;
        size_t End = netInfo.second.EndPoint_.first;
        if(netInfo.second.StartPoint_.second == 1){
            for(auto& netInfo2 : NetsInfo_){
                if(netInfo2.second.StartPoint_.first == Start && netInfo2.second.StartPoint_.second == 0){
                    VCG_[NetName][netInfo2.first] = 1;
                    VCG_[netInfo2.first][NetName] = -1;
                }
                else if(netInfo2.second.EndPoint_.first == Start && netInfo2.second.EndPoint_.second == 0){
                    VCG_[NetName][netInfo2.first] = 1;
                    VCG_[netInfo2.first][NetName] = -1;
                }
            }
        }
        if(netInfo.second.EndPoint_.second == 1){
            for(auto& netInfo2 : NetsInfo_){
                if(netInfo2.second.StartPoint_.first == End && netInfo2.second.StartPoint_.second == 0){
                    VCG_[NetName][netInfo2.first] = 1;
                    VCG_[netInfo2.first][NetName] = -1;
                }
                else if(netInfo2.second.EndPoint_.first == End && netInfo2.second.EndPoint_.second == 0){
                    VCG_[NetName][netInfo2.first] = 1;
                    VCG_[netInfo2.first][NetName] = -1;
                }
            }
        }
    }
}

void Channel::constructTracks(){
    /* construct availalbe intervals */
    int Start = 0;
    int End = NumTopTracks_;
    for (int i = Start; i < End; i++)
    {
        std::string TrackName = "T" + std::to_string(i);
        std::string LastTrackName = "T" + std::to_string(i - 1);
        if(TracksInfo_.find(LastTrackName) != TracksInfo_.end()){
            TracksInfo_[TrackName] = TracksInfo_[LastTrackName];
        }
        else{
            std::vector<std::array<size_t, 2>> intervals;
            intervals.push_back({0, NumPins_ - 1});
            TracksInfo_[TrackName] = intervals;
        }
        for(const auto& TrackSec : TopBoundaryLine_[TrackName]){
            updateInterval(TracksInfo_[TrackName], TrackSec);
        }
    }

    Start = 0;
    End = NumButtomTracks_;
    for(int i = Start; i < End; i++){
        std::string TrackName = "B" + std::to_string(i);
        std::string LastTrackName = "B" + std::to_string(i - 1);
        if(TracksInfo_.find(LastTrackName) != TracksInfo_.end()){
            TracksInfo_[TrackName] = TracksInfo_[LastTrackName];
        }
        else{
            std::vector<std::array<size_t, 2>> intervals;
            intervals.push_back({0, NumPins_ - 1});
            TracksInfo_[TrackName] = intervals;
        }
        for(const auto& TrackSec : BottomBoundaryLine_[TrackName]){
            updateInterval(TracksInfo_[TrackName], TrackSec);
        }
    }
}

void Channel::allocateNet(){
    std::string TrackName;
    std::string prevNet;
    size_t      watermark1;
    size_t      watermark2;
    std::multimap<size_t, std::string> sortedNets;
    for(const auto& netInfo : NetsInfo_){
        sortedNets.insert({netInfo.second.StartPoint_.first, netInfo.first});
    }

    /* fill in top tracks */
    int Start = NumTopTracks_ - 1;
    int End = 0;
    for (int i = Start; i >= End; i--)
    {
        TrackName = "T" + std::to_string(i);
        if(TracksInfo_[TrackName].size() == 0) continue;
        watermark1 = 0;
        watermark2 = 0;
        for (size_t i = 0; i < TracksInfo_[TrackName].size(); ++i){
            auto& interval = TracksInfo_[TrackName][i];
            if(interval[0] >= watermark1){
                watermark1 = interval[0];
                watermark2 = interval[1];
            }

            /* every net try this interval */
            auto Net = sortedNets.begin();
            while(Net != sortedNets.end()){
                if( watermark1 <= NetsInfo_[Net->second].StartPoint_.first &&
                    NetsInfo_[Net->second].EndPoint_.first <= watermark2 && 
                    allValuesNotMinusOne(VCG_, Net->second) ){

                    deleteEdges(VCG_, Net->second);
                    NetsInfo_[Net->second].TrackName_ = TrackName;
                    std::array<size_t, 2> TrackSec = {NetsInfo_[Net->second].StartPoint_.first, NetsInfo_[Net->second].EndPoint_.first};
                    updateInterval(TracksInfo_[TrackName], TrackSec);
                    i--;
                    Net = sortedNets.erase(Net);
                    break;
                }
                else{
                    ++Net;
                }
            }
        }
    }
    
    /* fill in buttom tracks */
    Start = NumButtomTracks_ - 1;
    End = 0;
    for(int i = Start; i >= End; i--){
        TrackName = "B" + std::to_string(i);
        if(TracksInfo_[TrackName].size() == 0) continue;
        watermark1 = 0;
        watermark2 = 0;
        for (size_t i = 0; i < TracksInfo_[TrackName].size(); ++i){
            auto& interval = TracksInfo_[TrackName][i];

            if(interval[0] >= watermark1){
                watermark1 = interval[0];
                watermark2 = interval[1];
            }

            /* every net try this interval */
            auto Net = sortedNets.begin();
            while(Net != sortedNets.end()){
                if( watermark1 <= NetsInfo_[Net->second].StartPoint_.first &&
                    NetsInfo_[Net->second].EndPoint_.first <= watermark2 &&
                    allValuesNotOne(VCG_, Net->second) ){

                    deleteEdges(VCG_, Net->second);
                    NetsInfo_[Net->second].TrackName_ = TrackName;
                    std::array<size_t, 2> TrackSec = {NetsInfo_[Net->second].StartPoint_.first, NetsInfo_[Net->second].EndPoint_.first};
                    updateInterval(TracksInfo_[TrackName], TrackSec);
                    i--;
                    Net = sortedNets.erase(Net);
                    break;
                }
                else{
                    ++Net;
                }
            }
        }
    }

    /* fill in added tracks */
    size_t Count = 0;
    while (sortedNets.size() != 0)
    {
        Count++;
        TrackName = "C" + std::to_string(Count);
        std::vector<std::array<size_t, 2>> intervals;
        intervals.push_back({0, TopNetIDs_.size() - 1});
        TracksInfo_[TrackName] = intervals;
        watermark1 = 0;
        watermark2 = 0;
        for (size_t i = 0; i < TracksInfo_[TrackName].size(); ++i){
            auto& interval = TracksInfo_[TrackName][i];

            if(interval[0] >= watermark1){
                watermark1 = interval[0];
                watermark2 = interval[1];
            }

            /* every net try this interval */
            auto Net = sortedNets.begin();
            while(Net != sortedNets.end()){
                if( watermark1 <= NetsInfo_[Net->second].StartPoint_.first &&
                    NetsInfo_[Net->second].EndPoint_.first <= watermark2 &&
                    allValuesNotMinusOne(VCG_, Net->second) 
                    ){

                    deleteEdges(VCG_, Net->second);
                    NetsInfo_[Net->second].TrackName_ = TrackName;
                    std::array<size_t, 2> TrackSec = {NetsInfo_[Net->second].StartPoint_.first, NetsInfo_[Net->second].EndPoint_.first};
                    updateInterval(TracksInfo_[TrackName], TrackSec);
                    i--;
                    prevNet = Net->second;
                    Net = sortedNets.erase(Net);
                    break;
                }
                else if( watermark1 - 1 <= NetsInfo_[Net->second].StartPoint_.first &&
                         NetsInfo_[Net->second].EndPoint_.first <= watermark2 &&
                         allValuesNotMinusOne(VCG_, Net->second) && 
                         checkSameNetSeries(prevNet, Net->second)
                        ){
                    deleteEdges(VCG_, Net->second);
                    NetsInfo_[Net->second].TrackName_ = TrackName;
                    std::array<size_t, 2> TrackSec = {NetsInfo_[Net->second].StartPoint_.first, NetsInfo_[Net->second].EndPoint_.first};
                    updateInterval(TracksInfo_[TrackName], TrackSec);
                    i--;
                    prevNet = Net->second;
                    Net = sortedNets.erase(Net);
                    break;
                }
                else{
                    ++Net;
                }
            }
        }
    }
    NumAddedTracks_ = Count;
}

bool checkSameNetSeries(const std::string& NetName1, const std::string& NetName2) {
    size_t pos1 = NetName1.find("_");
    size_t pos2 = NetName2.find("_");

    if (pos1 == std::string::npos || pos2 == std::string::npos) {
        return false;
    }

    return NetName1.substr(0, pos1) == NetName2.substr(0, pos2);
}

void deleteEdges(std::unordered_map<std::string, std::unordered_map<std::string, int>>& VCG, const std::string& NetName){
    auto it = VCG.find(NetName);
    if (it != VCG.end()) {
        for (const auto& innerPair : it->second) {
            VCG[innerPair.first].erase(NetName);
        }
        VCG.erase(NetName);
    }
}

bool allValuesNotMinusOne(const std::unordered_map<std::string, std::unordered_map<std::string, int>>& VCG, const std::string& NetName){
    auto it = VCG.find(NetName);
    if (it != VCG.end()) {
        for (const auto& innerPair : it->second) {
            if (innerPair.second == -1) {
                return false;
            }
        }
        return true;
    }
    return false;
}

/* for rVCG_ to filling in buttom tracks */
bool allValuesNotOne(const std::unordered_map<std::string, std::unordered_map<std::string, int>>& VCG, const std::string& NetName){
    auto it = VCG.find(NetName);
    if (it != VCG.end()) {
        for (const auto& innerPair : it->second) {
            if (innerPair.second == 1) {
                return false;
            }
        }
        return true;
    }
    return false;
}

void updateInterval(std::vector<std::array<size_t, 2>>& intervals, const std::array<size_t, 2>& TrackSec){
    std::vector<std::array<size_t, 2>> newIntervals;
    for(const auto& interval : intervals){
        if(TrackSec[1] < interval[0] || interval[1] < TrackSec[0]){
            newIntervals.push_back(interval);
            continue;
        }
        else{
            if(interval[0] < TrackSec[0] && TrackSec[1] < interval[1]){
                newIntervals.push_back({interval[0], TrackSec[0] - 1});
                newIntervals.push_back({TrackSec[1] + 1, interval[1]});
            }
            else if(interval[0] < TrackSec[0] && interval[1] <= TrackSec[1]){
                newIntervals.push_back({interval[0], TrackSec[0] - 1});
            }
            else if(TrackSec[0] <= interval[0] && TrackSec[1] < interval[1]){
                newIntervals.push_back({TrackSec[1] + 1, interval[1]});
            }
        }
    }
    std::sort(newIntervals.begin(), newIntervals.end());
    intervals = newIntervals;
}

std::vector<std::pair<size_t, size_t>> findAllIndices(const std::vector<size_t>& vec1, const std::vector<size_t>& vec2, int value) {
    std::vector<std::pair<size_t, size_t>> indices;
    for (size_t i = 0; i < vec1.size(); ++i) {
        if (vec1[i] == value) {
            indices.push_back(std::make_pair(i, 1));
        }
        else if (vec2[i] == value) {
            indices.push_back(std::make_pair(i, 0));
        }
    }
    return indices;
}

Channel* parseChannelInstance(std::ifstream& input){
    
    Channel* channel = new Channel();
    std::string line;
    std::string bdline;
    size_t s, e, temp;

    /* pass first & last line */
    while (std::getline(input, line)) {
        std::stringstream ss(line);
        if (!line.empty() && line[0] == 'T') {
            ss >> bdline >> s >> e;
            channel->TopBoundaryLine_[bdline].push_back({s, e});
        }
        else if(!line.empty() && line[0] == 'B'){
            ss >> bdline >> s >> e;
            channel->BottomBoundaryLine_[bdline].push_back({s, e});
        }
        else{
            while (ss >> temp)
            {
                channel->TopNetIDs_.push_back(temp);
            }
            break;
        }
    }
    std::getline(input, line);
    std::stringstream ss2(line);
    while (ss2 >> temp)
    {
        channel->BottomNetIDs_.push_back(temp);
    }
    
    channel->NumTopTracks_ = channel->TopBoundaryLine_.size();
    channel->NumButtomTracks_ = channel->BottomBoundaryLine_.size();
    channel->NumPins_ = channel->TopNetIDs_.size();
    input.close();
    return channel;
}

std::vector<std::string> extractSameSeriesNames(const std::unordered_map<std::string, NetInfo>& NetsInfo, const std::string& series) {
    std::vector<std::string> result;

    for (const auto& pair : NetsInfo) {
        size_t pos = pair.first.find('_');
        if (pos != std::string::npos) {
            std::string currentSeries = pair.first.substr(0, pos);
            if (currentSeries == series) {
                result.push_back(pair.first);
            }
        }
        else{
            if(pair.first == series){
                result.push_back(pair.first);
                break;
            }
        }
    }

    return result;
}

void outputRoutingResult(std::ofstream& output, Channel* channel){

    std::vector<std::string> sameSeriesNets;
    std::vector<size_t> DogLegs;
    std::vector<NetInfo> NetSecInfos;
    NetInfo NetSecInfo;

    output << "Channel density: " << channel->NumAddedTracks_ << std::endl;
    for(size_t i = 1; i <= channel->NumNets_; ++i){
        std::string NetSeries = std::to_string(i);
        output << "Net " << NetSeries << std::endl;
        sameSeriesNets = extractSameSeriesNames(channel->NetsInfo_, NetSeries);
        std::sort(sameSeriesNets.begin(), sameSeriesNets.end(),
        [](const std::string& a, const std::string& b) {
            size_t posA = a.find('_');
            size_t posB = b.find('_');
            int numA = std::stoi(a.substr(posA + 1));
            int numB = std::stoi(b.substr(posB + 1));
            return numA < numB;
        });

        NetSecInfos.clear();
        NetSecInfo.TrackName_ = "";
        DogLegs.clear();
        for(const auto& Net : sameSeriesNets){
            if(NetSecInfo.TrackName_ != channel->NetsInfo_.at(Net).TrackName_){
                if(NetSecInfo.TrackName_ != ""){
                    if(NetSecInfo.TrackName_[0] == 'C'){
                        NetSecInfo.TrackName_ = "C" + std::to_string(channel->NumAddedTracks_ - std::stoi(NetSecInfo.TrackName_.substr(1)) + 1);
                    }
                    NetSecInfos.push_back(NetSecInfo);
                    DogLegs.push_back(NetSecInfo.EndPoint_.first);
                }
                NetSecInfo.TrackName_ = channel->NetsInfo_.at(Net).TrackName_;
                NetSecInfo.StartPoint_ = channel->NetsInfo_.at(Net).StartPoint_;
                NetSecInfo.EndPoint_ = channel->NetsInfo_.at(Net).EndPoint_;
            }
            else{
                NetSecInfo.EndPoint_ = channel->NetsInfo_.at(Net).EndPoint_;
            }
        }
        if(NetSecInfo.TrackName_[0] == 'C'){
            NetSecInfo.TrackName_ = "C" + std::to_string(channel->NumAddedTracks_ - std::stoi(NetSecInfo.TrackName_.substr(1)) + 1);
        }
        NetSecInfos.push_back(NetSecInfo);

        for(const auto& Sec: NetSecInfos){
            output << Sec.TrackName_ << " " << Sec.StartPoint_.first << " " << Sec.EndPoint_.first << std::endl;
        }

        for(const auto& DogLeg : DogLegs){
            output << "Dogleg " << DogLeg << std::endl;
        }
    }
}