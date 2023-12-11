#include "../asInc/Auction.hpp"

Auction::Auction(string name, int stvl, int dur, int uid) {
    _name = name;
    _startValue = stvl;
    _duration = dur;
    _uid = uid;
}

Auction::Auction(int uid, string aid, int state) {
    _uid = uid;
    _aid = aid;
    _state = state;
}

Auction::Auction(string aid) {
    _aid = aid;
}

string Auction::toString() {
    return "AID " + _aid + "\nNome: " + _name + "\nValue: " +
    to_string(_startValue) + "\nDuration: " + to_string(_duration) + 
    "\nState: " + to_string(_state) + '\n';
}

string Auction::simpleToString() {
    return _aid + " " + to_string(_state);
}
