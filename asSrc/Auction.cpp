#include "../asInc/Auction.hpp"

Auction::Auction(string name, int stvl, int dur, int uid) {
    _name = name;
    _startValue = stvl;
    _duration = dur;
    _uid = uid;
    // _id = aid++;
}

string Auction::toString() {
    return "AID " + to_string(_id) + "\nNome: " + _name + "\nValue: " +
    to_string(_startValue) + "\nDuration: " + to_string(_duration) + 
    "\nState: " + to_string(_state) + '\n';
}