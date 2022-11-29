#include "TCPSender.h"
#include <assert.h>
#include "Global.h"
// base -> 标识的是缓存的开始, base ...
// base -> 标识了
TCPSender::TCPSender(int N, int k): mod(1 << k), N(N), nextSeqNum(0), base(0), start_timer(false) {}


bool TCPSender::send(const Message &message) {
    if (this->waitingState == true || ((this->nextSeqNum + 1))) {
        return false;
    }
    // this->waiting true -> nextSeqNum < base + N;
    assert((nextSeqNum - base + mod) % mod < N);
    this->packets[nextSeqNum].acknum = -1;
    this->packets[nextSeqNum].seqnum = this->nextSeqNum;
    this->nextSeqNum = (this->nextSeqNum + 1) % mod;
    memcpy(this->packets[nextSeqNum].payload, message.data, sizeof(message.data));
    pns->sendToNetworkLayer(RECEIVER, this->packets[nextSeqNum]);
    if ((nextSeqNum - base + mod) % mod == N) {
        this->waitingState = true; // 进入等待状态
    }
    if (!start_timer) {
        pns->startTimer(SENDER, Configuration::TIME_OUT,this->base);
        start_timer = true;
        this->timer_seq = this->base;
    }
    this->nextSeqNum = (this->nextSeqNum + 1) % mod;
    return true;
}

void TCPSender::receive(const Packet &ackPkt) {
    int checkSum = pUtils->calculateCheckSum(ackPkt);
    //如果校验和正确，并且确认序号=发送方已发送并等待确认的数据包序号
    if (checkSum == ackPkt.checksum) {
        if ((ackPkt.acknum - this->base + mod) % mod < N) {
            // base...N - 1
            if (!counter[ackPkt.acknum] == 0) {
                if (start_timer) {
                    pns->stopTimer(SENDER,  this->timer_seq);
                }
                this->base = (ackPkt.acknum + 1) % mod;
                if (this->base != this->nextSeqNum) {
                    pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);
                    this->timer_seq = this->base;
                } else {
                    start_timer = false; // 停止
                }
            }
        } else {
            counter[ackPkt.acknum]++;
            if (counter[ackPkt.acknum] == 3) {
                // 快速重传.
                pns->sendToNetworkLayer(RECEIVER, this->packets[nextSeqNum]);
                counter[ackPkt.acknum] = 0;
            }
        }
    }
}

void TCPSender::timeoutHandler(int seqNum) {
    pns->stopTimer(SENDER, this->timer_seq);
    pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);
    this->timer_seq = this->base;
    pUtils->printPacket("发送方定时器时间到，重发上次发送的报文", this->packets[this->base]);
    pns->sendToNetworkLayer(RECEIVER, this->packets[this->base]);
}
