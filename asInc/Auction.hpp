#ifndef AUCTION_HPP
#define AUCTION_HPP

#include <stdlib.h>
#include <string>

using namespace std;

class Auction {
    public:
    Auction(int uid, string name, string Fname, int highVal, int stVal, string date, int timeActive, string aid, int state); // for getAuction
    Auction(int uid, string name, int stvl, int dur);
    Auction(int uid, string aid, int state);   //for getUserAuctions
    Auction(string aid);
    string _name, _aid, _fname, _date;
    int _highValue, _startValue, _duration, _uid;
    int _state = 1;    
    string toString();
    string simpleToString();
};

int getAuctionTimeActive(int startSec, int duration, int currSec);

#endif