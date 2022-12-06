#include "GBNRdtSender.h"
#include "Global.h"
#include <assert.h>
#include <iostream>
GBNRdtSender::GBNRdtSender(const int N, const int k): base(0), N(N), waitingState(false), nextSeqNum(0), mod(1 << k) {}



bool GBNRdtSender::getWaitingState() {
    return this->waitingState;
}

bool GBNRdtSender::send(const Message &message) {
    if ((nextSeqNum - base + mod) % mod < N) {
        // this->waiting true -> nextSeqNum < base + N;
        this->waitingState = false;
        this->packetWaitingAck[nextSeqNum].acknum = -1;
        this->packetWaitingAck[nextSeqNum].seqnum = this->nextSeqNum;
        this->packetWaitingAck[nextSeqNum].checksum = 0;
        memcpy(this->packetWaitingAck[nextSeqNum].payload, message.data, sizeof(message.data)); // 拷贝数据
        this->packetWaitingAck[nextSeqNum].checksum = pUtils->calculateCheckSum(this->packetWaitingAck[nextSeqNum]);
        pUtils->printPacket("发送方发送报文", this->packetWaitingAck[this->nextSeqNum]);
        pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[nextSeqNum]);

        if (base == nextSeqNum) { //如果为窗口首元素则打开计时器
            pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);
        }
        this->nextSeqNum = (this->nextSeqNum + 1) % mod;
        // base...nextSeqNum == N
        int i = this->base;
        printf("\n\n");
        printf("新增加一个尚且未被确认的报文\n");
        printf("当前窗口内容为:-----\n");
        while (i != this->nextSeqNum) {
            pUtils->printPacket("", this->packetWaitingAck[i]);
            i = (i + 1) % mod;
        }
        printf("窗口打印完毕:------\n");
        printf("\n\n");
        if ((nextSeqNum - base + mod) % mod == N) {
            this->waitingState = true;
        }
        return true;
    } else {
        this->waitingState = true;
        return false;
    }
}

void GBNRdtSender::receive(const Packet &ackPkt) {
        int checkSum = pUtils->calculateCheckSum(ackPkt);
        //如果校验和正确，并且确认序号=发送方已发送并等待确认的数据包序号
        if (checkSum == ackPkt.checksum) {
            int last_base = this->base;
            // ack > this->base
            if ((ackPkt.acknum - this->base + mod) % mod < N) {
                this->base = (ackPkt.acknum + 1) % mod;
                pUtils->printPacket("发送方正确收到确认", ackPkt);
                if (this->base == this->nextSeqNum) {
                    pns->stopTimer(SENDER, last_base);
                } else {
                    pns->stopTimer(SENDER, last_base);
                    pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);
                }
                printf("\n\n");
                printf("接收到正确的ack\n");
                printf("当前的窗口为:--------\n");
                int i = this->base;
                while (i != this->nextSeqNum) {
                    pUtils->printPacket("", this->packetWaitingAck[i]);
                    i = (i + 1) % mod;
                }
                printf("当前的窗口为:--------\n");
                printf("\n\n");
                this->waitingState = false;
            }
        }
}

void GBNRdtSender::timeoutHandler(int seqNum) {
   // assert(seqNum == this->base);

    int i = 0;
    // 累积重新发送窗口
    while ((seqNum + i) % mod != nextSeqNum) {
        pUtils->printPacket("发送方定时器时间到，重发上次发送的报文", this->packetWaitingAck[(seqNum + i) % mod]);
        pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[(seqNum + i) % mod]);
        i++;
    }
    pns->stopTimer(SENDER, this->base);
    pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);
}



