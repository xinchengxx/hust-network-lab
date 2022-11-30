#include "SRRdtSender.h"
#include "Global.h"
#include "assert.h"

SRRdtSender::SRRdtSender(const int N, const int k): N(N), mod(1 << k), base(0), nextSeqNum(0), waitingState(false) {}

bool SRRdtSender::getWaitingState() {
    return this->waitingState;
}

bool SRRdtSender::send(const Message &message) {
    if (this->waitingState == true) {
        return false;
    }
    // nextSeqNum - base + mod

    assert((nextSeqNum - base + mod) % mod <= N);
    this->packetWaitingAck[nextSeqNum].acknum = -1;
    this->packetWaitingAck[nextSeqNum].seqnum = this->nextSeqNum;
    memcpy(this->packetWaitingAck[nextSeqNum].payload, message.data, sizeof(message.data));
    this->packetWaitingAck[nextSeqNum].checksum = pUtils->calculateCheckSum(this->packetWaitingAck[nextSeqNum]);

    pUtils->printPacket("发送方发送报文", this->packetWaitingAck[this->nextSeqNum]);
    pns->startTimer(SENDER, Configuration::TIME_OUT,this->nextSeqNum);
    pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[nextSeqNum]);
    this->nextSeqNum = (this->nextSeqNum + 1) % mod;
    if ((nextSeqNum - base + mod) % mod == N) {
        this->waitingState = true; // 进入等待状态
    }
    return true;
}

void SRRdtSender::receive(const Packet &ackPkt) {
    int checkSum = pUtils->calculateCheckSum(ackPkt);
    //如果校验和正确，并且确认序号=发送方已发送并等待确认的数据包序号
    if (checkSum == ackPkt.checksum) {
        // 处于窗口内
        if ((ackPkt.acknum - this->base + mod) % mod < N) {
            pUtils->printPacket("发送方正确收到确认", ackPkt);
            this->ackNums[ackPkt.acknum] = true;
            if (ackPkt.acknum == this->base) {
                int num = N, i = 0;
                while (i < num) {
                    if (this->ackNums[(this->base + i) % mod] == true) {
                        this->ackNums[(this->base + i) % mod] = false;
                        i++;
                    } else {
                        break;
                    }
                }
                this->waitingState = false;
                this->base = (this->base + i) % mod;
            }
            // 停止超时器
            pns->stopTimer(SENDER, ackPkt.acknum);
        }
    } else {
        pUtils->printPacket("收到ACK包校验和出错",ackPkt);
    }
}


void SRRdtSender::timeoutHandler(int seqNum) {
    pUtils->printPacket("发送方定时器时间到，重发上次发送的报文", this->packetWaitingAck[seqNum]);
    pns->stopTimer(SENDER, seqNum);
    pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[seqNum]);
    pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//重新启动发送方定时器

}