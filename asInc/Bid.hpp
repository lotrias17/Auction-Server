#ifndef BID_HPP
#define BID_HPP

#include <stdlib.h>
#include <string>

using namespace std;

class Bid {
    public:
    Bid(int value, string uid, string aid);
    int _value;
    string _uid;
    string _aid;
    string simpleToString();
};


#endif