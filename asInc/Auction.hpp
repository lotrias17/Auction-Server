#ifndef AUCTION_HPP
#define AUCTION_HPP

#include <stdlib.h>
#include <string>

using namespace std;

class Auction {
    public:
    Auction(string name, int startValue, int duration, int uid);
    string _name;
    int _startValue;
    int _duration;
    int _id = 0;
    int _state = 1;
    int _uid;
    string toString();
};


#endif