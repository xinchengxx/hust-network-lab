#ifndef SR_RDT_RECEIVER_H
#define SR_RDT_RECEIVER_H

#include "RdtReceiver.h"
#include <vector>
class SRRdtReceiver : public RdtReceiver
{
private:
    int base;	// recv_base
    std::vector<Packet> AckPkts;				//上次发送的确认报文
    std::vector<bool> buffer; // 标识收到的包
    const int mod; // GBN -> N
    int N;
public:
    SRRdtReceiver(const int N, const int k);
    virtual ~SRRdtReceiver() = default;

public:

    void receive(const Packet &packet);	//接收报文，将被NetworkService调用
};
#endif