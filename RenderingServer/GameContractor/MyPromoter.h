#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>
#include <vector>

#include "rapidjson/document.h"
#include "MyUserClass.h"
#include "MyHttp.h"
#include "MySkyway.h"

class MyPromoter
{
public:
	static std::string PromoterIP;
	static unsigned short PromoterPort;
	static std::string PromoterHost;


	static int WaitingUser(const int serverid, MyHttp& mysock, MyUserClass& myuser);
	static int PleaseCall(const int serverid, MyHttp& mysock, MyUserClass& myuser);
	static int StatusChange(const int serverid, const std::string newstatus, MyHttp& mysock, MyUserClass& myuser);
	static int RserverStatusUpdate(const int serverid, const std::string newstatus, const int usercount, MyHttp& mysock);

	static int WaitingQuit(const int serverid, MyHttp& myapisock, MyHttp& myrtcsock, std::vector<MyUserClass>& myusers, std::vector<int>& opennums);
};  

