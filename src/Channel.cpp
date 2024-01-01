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
    /* allocate top */
    size_t Start = NumTopTracks_ - 1;
    size_t End = 0;
    for (size_t i = Start; i < End; i++)
    {
        std::string TrackName = "T" + std::to_string(i);
        if(TopBoundaryLine_.find(TrackName) != TopBoundaryLine_.end()){
            for( const auto& TrackSec : TopBoundaryLine_[TrackName] ){
            }
        }
    }
    
    /* allocate bottom */
}

void updateInterval(std::vector<std::array<size_t, 2>>& intervals, const std::array<size_t, 2>& TrackSec){
    std::vector<std::array<size_t, 2>> newIntervals;
    for(const auto& interval : intervals){
        if(TrackSec[1] <= interval[0] || interval[1] <= TrackSec[0]){
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
            else if(TrackSec[0] < interval[0] && interval[1] < TrackSec[1]){
                continue;
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