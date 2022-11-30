// StopWait.cpp : 定义控制台应用程序的入口点。
//


#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "StopWaitRdtSender.h"
#include "StopWaitRdtReceiver.h"
#include "GBNRdtReceiver.h"
#include "GBNRdtSender.h"
#include "SRRdtReceiver.h"
#include "SRRdtSender.h"
#include "TCPReceiver.h"
#include "TCPSender.h"


int main(int argc, char* argv[])
{
//	RdtSender *ps = new StopWaitRdtSender();
//	RdtReceiver * pr = new StopWaitRdtReceiver();
//    RdtSender *ps = new GBNRdtSender(Configuration::N, Configuration::k);
//    RdtReceiver *pr = new GBNRdtReceiver(Configuration::N, Configuration::k);
//    RdtSender *ps = new SRRdtSender(Configuration::N, Configuration::k);
//    RdtReceiver *pr = new SRRdtReceiver(Configuration::N, Configuration::k);
    RdtSender *ps = new TCPSender(Configuration::N, Configuration::k);
    RdtReceiver *pr = new TCPReceiver(Configuration::N, Configuration::k);
	pns->setRunMode(0);  //VERBOS模式
//	pns->setRunMode(1);  //安静模式
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile("./input.txt");
	pns->setOutputFile("./output.txt");

	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//指向唯一的工具类实例，只在main函数结束前delete
	delete pns;										//指向唯一的模拟网络环境类实例，只在main函数结束前delete
	
	return 0;
}

