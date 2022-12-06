#pragma once

#include <WinSock2.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <ws2tcpip.h>
#include <windows.h>
#include <thread>
#pragma comment(lib,"ws2_32.lib")

class MyHttp
{
public:
	MyHttp(std::string DestIpAddr, unsigned short DestPort);
	~MyHttp();

	void SendStr(std::string& sendStr);
	std::pair<std::string, std::string> ReceiveStrs(); //<header, body>
	std::pair<std::string, std::string> ReceiveStrss();//content-length is all small caps ver 

private:
	sockaddr_in m_destAddr;
	int m_destSocket;

public: //static functions
	static std::string MakeHttpValue(const std::string& Method, const std::string& Url, const std::string& Host, const std::string& ContentType, std::string& Content);
	static std::string MakeHttpValue(const std::string& Method, const std::string& Url, const std::string& Host);
	
	static void StartWSA();
	static void CleanWSA();
};

