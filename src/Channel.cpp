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
                NetName += (char)('a' + i);
            }
            NetInfo netInfo;
            netInfo.StartPoint_ = indices[i];
            netInfo.EndPoint_ = indices[i + 1];
            NetsInfo_[NetName] = netInfo;
        }
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

void Channel::allocateNet(){
    /* allocate top */
    for(auto& Track: TopBoundaryLine_){
        std::string TrackName = Track.first;
        std::vector<std::array<size_t, 2>> TrackSec = Track.second;
        for(auto& Sec: TrackSec){
            size_t Start = Sec[0];
            size_t End = Sec[1];
            for(auto& Net: NetsInfo_){
                std::string NetName = Net.first;
                size_t NetStart = Net.second.StartPoint_.first;
                size_t NetEnd = Net.second.EndPoint_.first;
                if(NetStart == Start && Net.second.StartPoint_.second == 1){
                    Net.second.TrackName_ = TrackName;
                    break;
                }
                else if(NetEnd == Start && Net.second.EndPoint_.second == 1){
                    Net.second.TrackName_ = TrackName;
                    break;
                }
            }
        }
    }
    /* allocate bottom */
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
    
    input.close();
    
    return channel;
}