#include "GBNRdtSender.h"
#include "Global.h"
#include <assert.h>
#include <iostream>
GBNRdtSender::GBNRdtSender(const int N, const int k): base(0), N(N), waitingState(false), nextSeqNum(0), mod(1 << k) {}


//useless
bool GBNRdtSender::getWaitingState() {
    return this->waitingState;
}

bool GBNRdtSender::send(const Message &message) {
    if ((nextSeqNum - base + mod) % mod == N) {
        return false;
    }
    // this->waiting true -> nextSeqNum < base + N;
    assert((nextSeqNum - base + mod) % mod <= N);

    this->packetWaitingAck[nextSeqNum].acknum = -1;
    this->packetWaitingAck[nextSeqNum].seqnum = this->nextSeqNum;
    this->packetWaitingAck[nextSeqNum].checksum = 0;
    memcpy(this->packetWaitingAck[nextSeqNum].payload, message.data, sizeof(message.data));
    this->packetWaitingAck[nextSeqNum].checksum = pUtils->calculateCheckSum(this->packetWaitingAck[nextSeqNum]);
    pUtils->printPacket("发送方发送报文", this->packetWaitingAck[this->nextSeqNum]);
    pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[nextSeqNum]);
    if (base == nextSeqNum) {
        pns->startTimer(SENDER, Configuration::TIME_OUT,this->base);
    }
    this->nextSeqNum = (this->nextSeqNum + 1) % mod;
    if ((nextSeqNum - base + mod) % mod == N) {
        this->waitingState = true;
    }
    return true;
}

void GBNRdtSender::receive(const Packet &ackPkt) {
        int checkSum = pUtils->calculateCheckSum(ackPkt);
        //如果校验和正确，并且确认序号=发送方已发送并等待确认的数据包序号
        if (checkSum == ackPkt.checksum) {
            int last_base = this->base;
            if ((ackPkt.acknum - this->base + mod) % mod < N) {
                this->base = (ackPkt.acknum + 1) % mod;
                pUtils->printPacket("发送方正确收到确认", ackPkt);
                if (this->base == this->nextSeqNum) {
                    pns->stopTimer(SENDER, last_base);
                } else {
                    pns->stopTimer(SENDER, last_base);
                    pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);
                }
                this->waitingState = false;
            }
        } else {
            //assert(this->packetWaitingAck[this->base].seqnum == this->base);
            pUtils->printPacket("发送方没有正确收到确认，重发上次发送的报文", this->packetWaitingAck[this->base]);
            pns->stopTimer(SENDER, this->base);									//首先关闭定时器
            pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);			//重新启动发送方定时器
            pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[this->base]);			//重新发送数据包
        }
}

void GBNRdtSender::timeoutHandler(int seqNum) {
    assert(seqNum == this->base);

    int i = 0;
    while ((seqNum + i) % mod != nextSeqNum) {
        pUtils->printPacket("发送方定时器时间到，重发上次发送的报文", this->packetWaitingAck[(seqNum + i) % mod]);
        pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[(seqNum + i) % mod]);
        i++;
    }
    pns->stopTimer(SENDER, this->base);
    pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);
}



