#include "TCPSender.h"
#include <assert.h>
#include "Global.h"
// base -> 标识的是缓存的开始, base ...
// base -> 标识了
TCPSender::TCPSender(int N, int k): mod(1 << k), N(N), nextSeqNum(0), base(0), start_timer(false), counter(1 << k), packets(1 << k) {}


bool TCPSender::send(const Message &message) {
    if (this->waitingState == true) {
        return false;
    }
    // this->waiting true -> nextSeqNum < base + N;
    assert((nextSeqNum - base + mod) % mod < N);
    this->packets[nextSeqNum].acknum = -1;
    this->packets[nextSeqNum].seqnum = this->nextSeqNum;
    memcpy(this->packets[nextSeqNum].payload, message.data, sizeof(message.data));
    this->packets[nextSeqNum].checksum = pUtils->calculateCheckSum(this->packets[nextSeqNum]);

    pns->sendToNetworkLayer(RECEIVER, this->packets[nextSeqNum]);
    this->nextSeqNum = (this->nextSeqNum + 1) % mod;
    if ((nextSeqNum - base + mod) % mod == N) {
        this->waitingState = true; // 进入等待状态
    }
    if (!start_timer) {
        pns->startTimer(SENDER, Configuration::TIME_OUT,this->base);
        start_timer = true;
        this->timer_seq = this->base;
    }
    return true;
}

void TCPSender::receive(const Packet &ackPkt) {
    int checkSum = pUtils->calculateCheckSum(ackPkt);
    //如果校验和正确，并且确认序号=发送方已发送并等待确认的数据包序号
    if (checkSum == ackPkt.checksum) {
        // 在窗口内的包
        pUtils->printPacket("发送方接收到正确的应答报文.", ackPkt);
        if ((ackPkt.acknum - this->base + mod) % mod < N) {
            // base...N - 1
                if (last_ack != ackPkt.acknum) {
                    // 清除标记
                    counter[last_ack] = 0;
                    counter[ackPkt.acknum] = 1;
                    last_ack = ackPkt.acknum;
                } else {
                    counter[last_ack]++;
                }
                if (counter[ackPkt.acknum] == 3) {
                    // 重发下一个
                    assert(this->base == (ackPkt.acknum + 1) % mod);
                    assert((this->nextSeqNum - this->base + mod) % mod > 0);
                    if (start_timer) {
                        pns->stopTimer(SENDER, this->timer_seq);
                    }
                    pUtils->printPacket("快速重传缺失的包", this->packets[this->base]);
                    pns->sendToNetworkLayer(RECEIVER, this->packets[this->base]);
                    pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);
                    this->timer_seq = this->base;
                } else {
                    this->base = (ackPkt.acknum + 1) % mod;
                    pns->stopTimer(SENDER, this->timer_seq);
                    if (this->base != this->nextSeqNum) {
                        pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);
                        this->timer_seq = this->base;
                    } else {
                        this->start_timer = false;
                    }
                }
                if ((nextSeqNum - base + mod) % mod < N) {
                    this->waitingState = false;
                }
        }
    }
}

void TCPSender::timeoutHandler(int seqNum) {
    assert(seqNum == this->base);
    pns->stopTimer(SENDER, this->timer_seq);
    pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);
    this->timer_seq = this->base;
    pUtils->printPacket("发送方定时器时间到，重发上次发送的报文", this->packets[this->base]);
    pns->sendToNetworkLayer(RECEIVER, this->packets[this->base]);
}
