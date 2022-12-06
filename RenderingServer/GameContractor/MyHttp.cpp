#include "MyHttp.h"
MyHttp::MyHttp(std::string DestIpAddr, unsigned short DestPort)
{
	memset(&m_destAddr, 0, sizeof(m_destAddr));
	m_destAddr.sin_port = htons(DestPort);
	m_destAddr.sin_family = AF_INET;
	inet_pton(AF_INET, DestIpAddr.c_str(), &m_destAddr.sin_addr.s_addr);

	m_destSocket = socket(AF_INET, SOCK_STREAM, 0);

	connect(m_destSocket, (struct sockaddr*)&m_destAddr, sizeof(m_destAddr));
}

MyHttp::~MyHttp()
{
	closesocket(m_destSocket);
}

void MyHttp::SendStr(std::string& sendStr)
{
	//connect(m_destSocket, (struct sockaddr*)&m_destAddr, sizeof(m_destAddr));
	int i = send(m_destSocket, sendStr.c_str(), sendStr.size(), 0);
}

std::pair<std::string, std::string> MyHttp::ReceiveStrs()
{
	int rVal;
	char buf[2];
	int size = 1;
	int rfindpos;
	std::string recvFull;
	std::pair<std::string, std::string> retStrs = std::make_pair("header", "");
	std::string recvHead;
	int switchstep = 0;
	int contentLength;

	while (1) {
		rVal = recv(m_destSocket, buf, size, 0);
		if (rVal == 0 || rVal == -1)
		{
			return std::make_pair("error", "");
		}
		buf[sizeof(buf) - 1] = '\0';
		recvFull += buf;

		switch (switchstep)
		{
		case 0:
			rfindpos = recvFull.rfind("\r\n\r\n");
			if (rfindpos != std::string::npos)
			{
				int lengthstart = recvFull.rfind("Content-Length:") + 16;
				int lengthlast = recvFull.find("\r\n", lengthstart);
				std::string numstr = recvFull.substr(lengthstart, lengthlast-lengthstart);
				contentLength = atoi(numstr.c_str());
				if (contentLength == 0)
				{
					retStrs.first = recvFull;
					return retStrs;
				}
				switchstep = 1;
				retStrs.first = recvFull;
			}
			break;
		case 1:
			retStrs.second += buf;
			if (retStrs.second.size() == contentLength)
			{
				return retStrs;
			}
			break;
		}
	}
}

std::pair<std::string, std::string> MyHttp::ReceiveStrss()
{
	int rVal;
	char buf[2];
	int size = 1;
	int rfindpos;
	std::string recvFull;
	std::pair<std::string, std::string> retStrs = std::make_pair("header", "");
	std::string recvHead;
	int switchstep = 0;
	int contentLength;

	while (1) {
		rVal = recv(m_destSocket, buf, size, 0);
		if (rVal == 0 || rVal == -1)
		{
			return std::make_pair("error", "");
		}
		buf[sizeof(buf) - 1] = '\0';
		recvFull += buf;

		switch (switchstep)
		{
		case 0:
			rfindpos = recvFull.rfind("\r\n\r\n");
			if (rfindpos != std::string::npos)
			{
				int lengthstart = recvFull.rfind("content-length:") + 16;
				int lengthlast = recvFull.find("\r\n", lengthstart);
				std::string numstr = recvFull.substr(lengthstart, lengthlast - lengthstart);
				contentLength = atoi(numstr.c_str());
				if (contentLength == 0)
				{
					retStrs.first = recvFull;
					return retStrs;
				}
				switchstep = 1;
				retStrs.first = recvFull;
			}
			break;
		case 1:
			retStrs.second += buf;
			if (retStrs.second.size() == contentLength)
			{
				return retStrs;
			}
			break;
		}
	}
}

std::string MyHttp::MakeHttpValue(const std::string& Method, const std::string& Url, const std::string& Host, const std::string& ContentType, std::string& Content)
{
	std::string httpValue;
	httpValue = Method + " " + Url + " HTTP/1.1" +
		"\r\nHost: " + Host +
		"\r\nContent-Type: " + ContentType +
		"\r\nContent-Length: ";

	//LF -> CRLF
	int lffind = Content.find("\n");
	if (lffind != std::string::npos)
	{
		Content.insert(lffind, "\r");
		lffind = Content.find("\n");
		int crfind = Content.find("\r");
		while (1)
		{
			if (crfind == std::string::npos)
			{
				Content.insert(lffind, "\r");
			}
			crfind = Content.find("\r", lffind);
			lffind = Content.find("\n", lffind + 1);
			if (lffind == std::string::npos)
			{
				break;
			}
		}
	}

	//set Content-Length and Content
	httpValue += std::to_string(Content.size()) + "\r\n\r\n" + Content;

	return httpValue;
}

std::string MyHttp::MakeHttpValue(const std::string& Method, const std::string& Url, const std::string& Host)
{
	std::string httpValue;
	httpValue = Method + " " + Url + " HTTP/1.1" +
		"\r\nHost: " + Host + "\r\n\r\n";

	return httpValue;
}

void MyHttp::StartWSA()
{
	WSADATA m_wsa_data;
	if (WSAStartup(MAKEWORD(2, 0), &m_wsa_data) != 0) {
		std::printf("CANNOT intialized Winsock (WSAStartup)");
	}
}

void MyHttp::CleanWSA()
{
	WSACleanup();
}