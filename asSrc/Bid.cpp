#include "../asInc/Bid.hpp"

Bid::Bid(int value, string uid, string aid) {
    _value = value;
    _uid = uid;
    _aid = aid;
}

string Bid::simpleToString() {
    return _uid + " " + to_string(_value);
}

