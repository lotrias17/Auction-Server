#include "../asInc/tcpBuffer.hpp"

vector<tcpBuffer> tcpList;

tcpBuffer::tcpBuffer() {
    _fd = -1;
}

tcpBuffer::tcpBuffer(int fd) {
    _fd = fd;
    canWrite = false;
    toCreate = true;
}

void tcpBuffer::update(char* buf) {
    _buffer += buf;
}

void removeTcp(int fd) {
    for (int i = 0; i < (int) tcpList.size(); i++) {
        if (tcpList[i]._fd == fd) tcpList.erase(tcpList.begin()+i);
    }
    return;
}

tcpBuffer *getTcpBuffer(int fd) {
    for (int i = 0; i < (int) tcpList.size(); i++) {
        if (tcpList[i]._fd == fd) return &tcpList[i];
    }
    tcpBuffer t = tcpBuffer(fd);
    tcpList.push_back(t);
    return &tcpList[tcpList.size()-1];
}