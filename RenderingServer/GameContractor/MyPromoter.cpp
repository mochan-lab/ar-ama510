#include "MyPromoter.h"

int MyPromoter::WaitingUser(const int serverid, MyHttp& mysock, MyUserClass& myuser)
{
		std::string url = "/rserver/" + std::to_string(serverid) + "/waitinguser";
		std::string sendBody = MyHttp::MakeHttpValue("GET", url, PromoterHost);
		mysock.SendStr(sendBody);
 
		std::pair<std::string, std::string> receives = mysock.ReceiveStrss();
		rapidjson::Document jsonBody;
		jsonBody.Parse(receives.second.c_str());
		if (jsonBody.HasParseError()) {
			//health = -1;
			return -1;
		}
		if (!jsonBody.HasMember("status") || !jsonBody["status"].IsString()) {
			//health = -1;
			return -1;
		}
		if (jsonBody["status"]  == "NONE")
		{
			return -1;
		}
		else if (jsonBody["status"] == "CHECKING_CLIENT_CONNECT") {
			//std::printf(receives.first.c_str());
			std::printf("Coming new client : %s\n", receives.second.c_str());
			myuser.SetUserName(jsonBody["client"].GetString());
			myuser.SetDPeerId(jsonBody["dpeerid"].GetString());
			myuser.SetMPeerId(jsonBody["mpeerid"].GetString());
			int res[2] = { jsonBody["clientwidth"].GetInt(), jsonBody["clientheight"].GetInt() };
			myuser.SetResolution(res);
			return 10;
		}
			
	return -1;
}

int MyPromoter::PleaseCall(const int serverid, MyHttp& mysock, MyUserClass& myuser)
{
	std::string url = "/rserver/" + std::to_string(serverid) + "/pleasecall";
	std::string body = R"({"status":"SOON_CALL","client":")" + myuser.UserName() +
		R"(","dpeerid":")" + myuser.DPeerId() +
		R"(","mpeerid":")" + myuser.MPeerId() + R"("})";
	std::string sendBody = MyHttp::MakeHttpValue("POST", url, PromoterHost, "application/json", body);
	mysock.SendStr(sendBody);

	std::pair<std::string, std::string> strs = mysock.ReceiveStrss();
	rapidjson::Document jsonBody;
	jsonBody.Parse(strs.second.c_str());
	if (jsonBody.HasParseError()) {
		return -1;
	}
	if (!jsonBody.HasMember("status") || !jsonBody["status"].IsString()) {
		return -1;
	}
	if (jsonBody["status"] == "SOON_CALL")
	{
		return 0;
	}
	return -1;
}

int MyPromoter::StatusChange(const int serverid, const std::string newstatus, MyHttp& mysock, MyUserClass& myuser)
{
	std::string url = "/rserver/" + std::to_string(serverid) + "/statusupdate";
	std::string body = R"({"status":")" + newstatus +
		R"(","client":")" + myuser.UserName() + R"("})";
	std::string sendBody = MyHttp::MakeHttpValue("POST", url, PromoterHost, "application/json", body);
	mysock.SendStr(sendBody);

	std::pair<std::string, std::string> strs = mysock.ReceiveStrss();
	rapidjson::Document jsonBody;
	jsonBody.Parse(strs.second.c_str());
	if (jsonBody.HasParseError()) {
		return -1;
	}
	if (!jsonBody.HasMember("status") || !jsonBody["status"].IsString()) {
		return -1;
	}
	if (jsonBody["status"] == newstatus.c_str())
	{
		return 0;
	}
	return -1;
}

int MyPromoter::RserverStatusUpdate(const int serverid, const std::string newstatus, const int usercount, MyHttp& mysock)
{
	std::string url = "/rserver/" + std::to_string(serverid) + "/rserverstatusupdate/" + std::to_string(usercount);
	std::string body = R"({"status":")" + newstatus +
		R"(","rserver":")" + std::to_string(serverid) + R"("})";
	std::string sendBody = MyHttp::MakeHttpValue("POST", url, PromoterHost, "application/json", body);
	mysock.SendStr(sendBody);

	std::pair<std::string, std::string> strs = mysock.ReceiveStrss();
	rapidjson::Document jsonBody;
	jsonBody.Parse(strs.second.c_str());
	if (jsonBody.HasParseError()) {
		return -1;
	}
	if (!jsonBody.HasMember("status") || !jsonBody["status"].IsString()) {
		return -1;
	}
	if (jsonBody["status"] == newstatus.c_str())
	{
		return 0;
	}
	return -1;
}

int MyPromoter::WaitingQuit(const int serverid, MyHttp& myapisock, MyHttp& myrtcsock, std::vector<MyUserClass>& myusers, std::vector<int>& opennums)
{

	std::vector<int> forDel;
	for (int i = myusers.size() - 1; i >= 0; i--) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		
		MyUserClass myuser = myusers[i];

		std::string url = "/rserver/" + std::to_string(serverid) + "/waitingquit/" + myuser.UserName();
		std::string sendBody = MyHttp::MakeHttpValue("GET", url, PromoterHost);
		myapisock.SendStr(sendBody);

		std::pair<std::string, std::string> receives = myapisock.ReceiveStrss();
		rapidjson::Document jsonBody;
		jsonBody.Parse(receives.second.c_str());
		if (jsonBody.HasParseError()) {
			continue;
		}
		if (!jsonBody.HasMember("status") || !jsonBody["status"].IsString()) {
			continue;
		}
		//if (jsonBody["status"] == "NOT_EXIST_THE_CLIENT" || jsonBody["status"] == "CLIENT_QUITING" || jsonBody["status"] == "CLIENT_QUITED")
		if (jsonBody["status"] == "CLIENT_QUITING" || jsonBody["status"] == "CLIENT_QUITED")
		{
			forDel.push_back(i);
			continue;
		}
	}

	for (int toDel : forDel) {
		myusers[toDel].CloseEncodePiper();
		myusers[toDel].CloseGameEngine();
		MySkyway::MCloseStream(myrtcsock, myusers[toDel]);
		MySkyway::DCloseConnect(myrtcsock, myusers[toDel]);
		std::string newstatus = "CLIENT_QUITED_INVALID";  //error please clean up
		MyPromoter::StatusChange(serverid, newstatus, myapisock, myusers[toDel]);
		opennums[myusers[toDel].PipeNum()] = 0;
		myusers.erase(myusers.begin() + toDel);
	}
	return 1;
}