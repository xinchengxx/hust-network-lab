#ifndef GBN_RDT_RECEIVER_H
#define GBN_RDT_RECEIVER_H
#include "RdtReceiver.h"
class GBNRdtReceiver :public RdtReceiver
{
private:
    int expectSequenceNumberRcvd;	// 期待收到的下一个报文序号
    Packet lastAckPkt;				//上次发送的确认报文
    const int mod; // GBN -> N
    const int N;
public:
    GBNRdtReceiver(const int N, const int K);
    virtual ~GBNRdtReceiver() = default;

public:

    void receive(const Packet &packet);	//接收报文，将被NetworkService调用
};
#endif