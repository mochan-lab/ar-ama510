#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <thread>

#include "rapidjson/document.h"
#include "MyUserClass.h"
#include "MyHttp.h"

class MySkyway
{
public:
	static std::string apikey;
	static std::string skywayHost;

	static int MMakePeer(MyHttp& mysock, MyUserClass& myuser);
	static int DMakePeer(MyHttp& mysock, MyUserClass& myuser);

	static int MWaitBindPeer(MyHttp& mysock, MyUserClass& myuser);
	static int DWaitBindPeer(MyHttp& mysock, MyUserClass& myuser);

	static int MOpenSendPort(MyHttp& mysock, MyUserClass& myuser);
	static int DOpenSendPort(MyHttp& mysock, MyUserClass& myuser);

	static int MWaitCall(MyHttp& mysock, MyUserClass& myuser);
	static int DWaitConnect(MyHttp& mysock, MyUserClass& myuser);

	static int MAnswerCall(MyHttp& mysock, MyUserClass& myuser);
	static int DWaitOpen(MyHttp& mysock, MyUserClass& myuser);

	static int MStartStream(MyHttp& mysock, MyUserClass& myuser);
	static int DPutParams(MyHttp& mysock, MyUserClass& myuser);

	static int MCloseStream(MyHttp& mysock, MyUserClass& myuser);
	static int DCloseConnect(MyHttp& mysock, MyUserClass& myuser);
};