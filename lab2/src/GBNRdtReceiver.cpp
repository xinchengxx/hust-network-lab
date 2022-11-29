#include "GBNRdtReceiver.h"
#include "Global.h"

GBNRdtReceiver::GBNRdtReceiver(const int N, const int K): mod(1 << K), expectSequenceNumberRcvd(0), N(N) {
    lastAckPkt.acknum = -1; //初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接受的数据包出错时该确认报文的确认号为-1
    lastAckPkt.checksum = 0;
    lastAckPkt.seqnum = -1;
    for(int i = 0; i < Configuration::PAYLOAD_SIZE;i++){
        lastAckPkt.payload[i] = '.';
    }
    lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}

void GBNRdtReceiver::receive(const Packet &packet) {
    // check sum
    pUtils->printPacket("接收方收到发送方的报文", packet);
    int checkSum = pUtils->calculateCheckSum(packet);
    if (checkSum == packet.checksum) {
        if (this->expectSequenceNumberRcvd == packet.seqnum) {
            pUtils->printPacket("接收方正确收到发送方的报文", packet);
            //取出Message，向上递交给应用层
            Message msg;
            memcpy(msg.data, packet.payload, sizeof(packet.payload));
            pns->delivertoAppLayer(RECEIVER, msg);

            lastAckPkt.acknum = packet.seqnum; //确认序号等于收到的报文序号
            lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
            pUtils->printPacket("接收方发送确认报文", lastAckPkt);
            pns->sendToNetworkLayer(SENDER, lastAckPkt);    //调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对
            this->expectSequenceNumberRcvd = (this->expectSequenceNumberRcvd + 1) % mod;
        } else if ((this->expectSequenceNumberRcvd - packet.seqnum + mod) % mod < N) {
            lastAckPkt.acknum = packet.seqnum; //确认序号等于收到的报文序号
            lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
            pUtils->printPacket("接收方发送确认报文", lastAckPkt);
            pns->sendToNetworkLayer(SENDER, lastAckPkt);
        }
    }
}