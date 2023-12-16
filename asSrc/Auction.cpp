#include "../asInc/Auction.hpp"

Auction::Auction(int uid, string name, int stvl, int dur) {
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

// Auction(stoi(uid), aucName, Fname, stoi(value), startDate1+" "+startDate2 , timeActive, aid, state);
Auction::Auction(int uid, string name, string Fname, int highVal, int stVal, string date, int timeActive, string aid, int state) {
    _uid = uid;
    _name = name;
    _fname = Fname;
    _highValue = highVal;
    _startValue = stVal;
    _date = date;
    _duration = timeActive;
    _aid = aid;
    _state = state;
}

Auction::Auction(string aid) {
    _aid = aid;
}

string Auction::toString() {
    return to_string(_uid)+" "+_name+" "+_fname+" "+to_string(_startValue)
    +" "+_date+" ";
}

string Auction::simpleToString() {
    return _aid + " " + to_string(_state);
}

int getAuctionTimeActive(int startSec, int duration, int currSec) {
    if (currSec < startSec + duration) return currSec - startSec;

    return duration;
}