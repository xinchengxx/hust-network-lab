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
    std::vector<Packet> cachedPkts;				//上次发送的确认报文
    std::vector<bool> buffer; // 标识收到的包
    const int mod;
    int N;
};

#endif