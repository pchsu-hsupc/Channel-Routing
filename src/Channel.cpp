#include "Channel.h"

Channel::Channel(/* args */)
{
}

Channel::~Channel()
{
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
            channel->TopBoundaryLine_[bdline] = {s, e};
        }
        else if(!line.empty() && line[0] == 'B'){
            ss >> bdline >> s >> e;
            channel->BottomBoundaryLine_[bdline] = {s, e};
        }
        else{
            while (ss >> temp)
            {
                channel->TopNetIDs_.push_back(temp);
                if(temp != 0) channel->NetsInfo_[temp];
            }
            break;
        }
    }
    std::getline(input, line);
    std::stringstream ss2(line);
    while (ss2 >> temp)
    {
        channel->BottomNetIDs_.push_back(temp);
        if(temp != 0) channel->NetsInfo_[temp];
    }
    
    input.close();
    
    return channel;
}