#ifndef AUCTION_HPP
#define AUCTION_HPP

#include <stdlib.h>
#include <string>

using namespace std;

class Auction {
    public:
    Auction(int uid, string name, int stvl, int dur);
    Auction(int uid, string aid, int state);   //for getUserAuctions
    Auction(string aid);
    string _name;
    int _startValue;
    int _duration;
    string _aid;
    int _state = 1;
    int _uid;
    string toString();
    string simpleToString();
};


#endif