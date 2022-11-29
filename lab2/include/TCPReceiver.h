#ifndef TCP_RECEIVER_H
#define TCP_RECEIVER_H
#include "DataStructure.h"
#include "RdtReceiver.h"
#include <vector>
class TCPReceiver: public RdtReceiver{
public:
    TCPReceiver(const int N, const int k);
    virtual ~TCPReceiver() = default;
    void receive(const Packet &packet);
private:

    int base;	// recv_base
    Packet AckPkts[8];				//上次发送的确认报文
    bool buffer[8]; // 标识收到的包
    const int mod;
    int N;
};

#endif