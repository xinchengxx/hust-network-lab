#ifndef TCP_SENDER_H
#define TCP_SENDER_H
#include "RdtSender.h"
#include "DataStructure.h"
#include <vector>
class TCPSender: public RdtSender{
public:
    // no use
    bool getWaitingState() { return true; }
    bool send(const Message &message);
    TCPSender(int N, int k);
    ~TCPSender() = default;
    void timeoutHandler(int seqNum);
    void receive(const Packet &ackPkt);
private:
    bool start_timer;
    int timer_seq;
    int N;
    int mod;
    int base;
    bool waitingState;
    int nextSeqNum;
    Packet packets[8];
    int counter[8];
};

#endif