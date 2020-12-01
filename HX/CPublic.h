#pragma once
#include <string>
#include "CSerialPort/SerialPort.h"
#include "CSerialPort/SerialPortInfo.h"

using namespace itas109;


class CPublic
{
public:
	CPublic();
	virtual ~CPublic();

public:
	static bool test1;
	//判断是否到达
	static bool ArriveFlag;
	//判断识别是否完成
	static bool IdentifyDone;
	//开始执行识别
	static bool ExecuteIdentify;

	static std::string Port;
	static int BaudRate;
	static int Parity;
	static int DataBits;
	static int Stop;


	//内存泄漏
	//static CSerialPort m_SerialPort;//About CSerialPort
public:
	////创建CRC16的全局函数
	//static unsigned short CRC16(unsigned char* puchMsg, unsigned short usDataLen);
	//
	////发送函数
	//static void SendData(int CommTypeIn, WORD DownAdd, DWORD DownData);

};



