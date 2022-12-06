#include "MySkyway.h"

int MySkyway::MMakePeer(MyHttp& mysock, MyUserClass& myuser)
{
	std::string body;
	body = R"({"key":")" + apikey + R"(","domain":"MyGlobalIP","peer_id":")" + myuser.MPeerId() + R"(","turn":true})";

	std::string sendBody = MyHttp::MakeHttpValue("POST", "/peers", skywayHost, "application/json", body);
	mysock.SendStr(sendBody);

	std::pair<std::string, std::string> strs = mysock.ReceiveStrs();
	rapidjson::Document jsonBody;
	jsonBody.Parse(strs.second.c_str());
	if (jsonBody.HasParseError()) {
		return -1;
	}
	rapidjson::Value& jsonParams = jsonBody["params"];
	if (!jsonParams.HasMember("token") || !jsonParams["token"].IsString()) {
		return -1;
	}
	myuser.SetMSkywayToken(jsonParams["token"].GetString());

	return 0;
}

int MySkyway::DMakePeer(MyHttp& mysock, MyUserClass& myuser)
{
	std::string body;
	body = R"({"key":")" + apikey + R"(","domain":"MyGlobalIP","peer_id":")" + myuser.DPeerId() + R"(","turn":true})";

	std::string sendBody = MyHttp::MakeHttpValue("POST", "/peers", skywayHost, "application/json", body);
	mysock.SendStr(sendBody);

	std::pair<std::string, std::string> strs = mysock.ReceiveStrs();
	rapidjson::Document jsonBody;
	jsonBody.Parse(strs.second.c_str());
	if (jsonBody.HasParseError()) {
		return -1;
	}
	rapidjson::Value& jsonParams = jsonBody["params"];
	if (!jsonParams.HasMember("token") || !jsonParams["token"].IsString()) {
		return -1;
	}
	myuser.SetDSkywayToken(jsonParams["token"].GetString());

	return 0;
}

int MySkyway::MWaitBindPeer(MyHttp& mysock, MyUserClass& myuser)
{
	while (1)
	{
		std::string url = "/peers/" + myuser.MPeerId() + "/events?token=" + myuser.MSkywayToken();
		std::string sendBody = MyHttp::MakeHttpValue("GET", url, skywayHost);
		mysock.SendStr(sendBody);
		
		std::pair<std::string, std::string> strs = mysock.ReceiveStrs();
		rapidjson::Document jsonBody;
		jsonBody.Parse(strs.second.c_str());
		if (jsonBody.HasParseError()) {
			return -1;
		}
		if (!jsonBody.HasMember("event") || !jsonBody["event"].IsString()) {
			return -1;
		}
		if (jsonBody["event"] == "OPEN")
		{
			return 0;
		}
	}
}

int MySkyway::DWaitBindPeer(MyHttp& mysock, MyUserClass& myuser)
{
	while (1)
	{
		std::string url = "/peers/" + myuser.DPeerId() + "/events?token=" + myuser.DSkywayToken();
		std::string sendBody = MyHttp::MakeHttpValue("GET", url, skywayHost);
		mysock.SendStr(sendBody);

		std::pair<std::string, std::string> strs = mysock.ReceiveStrs();
		rapidjson::Document jsonBody;
		jsonBody.Parse(strs.second.c_str());
		if (jsonBody.HasParseError()) {
			return -1;
		}
		if (!jsonBody.HasMember("event") || !jsonBody["event"].IsString()) {
			return -1;
		}
		if (jsonBody["event"] == "OPEN")
		{
			return 0;
		}
	}
}

int MySkyway::MOpenSendPort(MyHttp& mysock, MyUserClass& myuser)
{
	std::string body = R"({"is_video":true})";
	std::string sendBody = MyHttp::MakeHttpValue("POST", "/media", skywayHost, "application/json", body);
	mysock.SendStr(sendBody);

	std::pair<std::string, std::string> strs = mysock.ReceiveStrs();
	rapidjson::Document jsonBody;
	jsonBody.Parse(strs.second.c_str());
	if (jsonBody.HasParseError()) {
		return -1;
	}
	if (!jsonBody.HasMember("media_id") || !jsonBody["media_id"].IsString()) {
		return -1;
	}
	myuser.SetMId(jsonBody["media_id"].GetString());

	if (!jsonBody.HasMember("port") || !jsonBody["port"].IsInt()) {
		return -1;
	}
	myuser.SetMSendPort(jsonBody["port"].GetInt());

	return 0;
}

int MySkyway::DOpenSendPort(MyHttp& mysock, MyUserClass& myuser)
{
	std::string body = R"({})";
	std::string sendBody = MyHttp::MakeHttpValue("POST", "/data", skywayHost, "application/json", body);
	mysock.SendStr(sendBody);

	std::pair<std::string, std::string> strs = mysock.ReceiveStrs();

	rapidjson::Document jsonBody;
	jsonBody.Parse(strs.second.c_str());
	if (jsonBody.HasParseError()) {
		return -1;
	}
	if (!jsonBody.HasMember("data_id") || !jsonBody["data_id"].IsString()) {
		return -1;
	}
	myuser.SetDId(jsonBody["data_id"].GetString());
	
	if (!jsonBody.HasMember("port") || !jsonBody["port"].IsInt()) {
		return -1;
	}
	myuser.SetDSendPort(jsonBody["port"].GetInt());

	return 0;
}

int MySkyway::MWaitCall(MyHttp& mysock, MyUserClass& myuser)
{
	rapidjson::Document jsonBody;

	std::string url = "/peers/" + myuser.MPeerId() + "/events?token=" + myuser.MSkywayToken();
	std::string sendBody = MyHttp::MakeHttpValue("GET", url, skywayHost);
	std::pair<std::string, std::string> strs;
	
	int i = 0;
	while (1)
	{
		i++;
		if (i > 50) {
			return -1;
		}
		mysock.SendStr(sendBody);

		strs = mysock.ReceiveStrs();
		jsonBody.Parse(strs.second.c_str());
		if (jsonBody.HasParseError()) {
			return -1;
		}
		if (!jsonBody.HasMember("event") || !jsonBody["event"].IsString()) {
			return -1;
		}
		if (jsonBody["event"] == "CALL")
		{
			std::cout << "GET CALL\n" << std::endl;
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		continue;
	}

	if (jsonBody["event"] != "CALL") {
		return -1;
	}
	rapidjson::Value& jsonparams = jsonBody["call_params"];
	if (!jsonparams.HasMember("media_connection_id") || !jsonparams["media_connection_id"].IsString()) {
		return -1;
	}
	myuser.SetMConnectionId(jsonparams["media_connection_id"].GetString());
	
	return 0;
}

int MySkyway::DWaitConnect(MyHttp& mysock, MyUserClass& myuser)
{
	rapidjson::Document jsonBody;
	while (1)
	{
		std::string url = "/peers/" + myuser.DPeerId() + "/events?token=" + myuser.DSkywayToken();
		std::string sendBody = MyHttp::MakeHttpValue("GET", url, skywayHost);
		mysock.SendStr(sendBody);

		std::pair<std::string, std::string> strs = mysock.ReceiveStrs();
		jsonBody.Parse(strs.second.c_str());
		if (jsonBody.HasParseError()) {
			return -1;
		}
		if (!jsonBody.HasMember("event") || !jsonBody["event"].IsString()) {
			return -1;
		}
		if (jsonBody["event"] == "CONNECTION")
		{
			std::cout << "GET CONNECT\n" << std::endl;
			break;
		}
	}

	rapidjson::Value& jsonparams = jsonBody["data_params"];
	if (!jsonparams.HasMember("data_connection_id") || !jsonparams["data_connection_id"].IsString()) {
		return -1;
	}
	myuser.SetDConnectionId(jsonparams["data_connection_id"].GetString());

	return 0;
}

int MySkyway::MAnswerCall(MyHttp& mysock, MyUserClass& myuser)
{
	std::string body;
	body = R"({"constraints":{"video":true,"videoReceiveEnabled":true,"audio":false,"audioReceiveEnabled":false,
"video_params":{"band_width":0,"codec":"H264","media_id":")" + myuser.MId() + R"(","payload_type":96}},
"redirect_params":{"video":{"ip_v4":"127.0.0.1","port":)" + std::to_string(myuser.MSendPort()) 
+ R"(}}})";
	std::string url = "/media/connections/" + myuser.MConnectionId() + "/answer";
	std::string sendBody = MyHttp::MakeHttpValue("POST", url, skywayHost, "application/json", body);
	mysock.SendStr(sendBody);

	std::pair<std::string, std::string> strs = mysock.ReceiveStrs();
	rapidjson::Document jsonBody;
	jsonBody.Parse(strs.second.c_str());
	if (jsonBody.HasParseError()) {
		return -1;
	}
	if (!jsonBody.HasMember("command_type") || !jsonBody["command_type"].IsString()) {
		return -1;
	}
	if (jsonBody["command_type"] != "MEDIA_CONNECTION_ANSWER")
	{
		return -1;
	}

	return 0;
}

int MySkyway::DWaitOpen(MyHttp& mysock, MyUserClass& myuser)
{
	rapidjson::Document jsonBody;
	while (1)
	{
		std::string url = "/data/connections/" + myuser.DConnectionId() + "/events";
		std::string sendBody = MyHttp::MakeHttpValue("GET", url, skywayHost);
		mysock.SendStr(sendBody);

		std::pair<std::string, std::string> strs = mysock.ReceiveStrs();
		jsonBody.Parse(strs.second.c_str());
		if (jsonBody.HasParseError()) {
			return -1;
		}
		if (!jsonBody.HasMember("event") || !jsonBody["event"].IsString()) {
			return -1;
		}
		if (jsonBody["event"] == "OPEN")
		{
			std::cout << "DATA OPEN\n" << std::endl;
			break;
		}
	}

	return 0;
}

int MySkyway::MStartStream(MyHttp& mysock, MyUserClass& myuser)
{
	rapidjson::Document jsonBody;
	while (1)
	{
		std::string url = "/media/connections/" + myuser.MConnectionId() + "/events";
		std::string sendBody = MyHttp::MakeHttpValue("GET", url, skywayHost);
		mysock.SendStr(sendBody);

		std::pair<std::string, std::string> strs = mysock.ReceiveStrs();
		jsonBody.Parse(strs.second.c_str());
		if (jsonBody.HasParseError()) {
			return -1;
		}
		if (!jsonBody.HasMember("event") || !jsonBody["event"].IsString()) {
			return -1;
		}
		if (jsonBody["event"] == "READY")
		{
			std::cout << "MEDIA READY\n" << std::endl;
			break;
		}
	}

	return 0;
}

int MySkyway::DPutParams(MyHttp& mysock, MyUserClass& myuser)
{
	std::string body;
	body = R"({"feed_params":{"data_id":")" + myuser.DId() + R"("},"redirect_params":
{"ip_v4":"127.0.0.1","port":)" + std::to_string(myuser.DRecvPort()) + R"(}})";
	std::string url = "/data/connections/" + myuser.DConnectionId();
	std::string sendBody = MyHttp::MakeHttpValue("PUT", url, skywayHost, "application/json", body);
	mysock.SendStr(sendBody);

	std::pair<std::string, std::string> strs = mysock.ReceiveStrs();
	
	std::cout << strs.second.c_str();

	rapidjson::Document jsonBody;
	jsonBody.Parse(strs.second.c_str());
	if (jsonBody.HasParseError()) {
		return -1;
	}
	if (!jsonBody.HasMember("command_type") || !jsonBody["command_type"].IsString()) {
		return -1;
	}
	if (jsonBody["command_type"] != "DATA_CONNECTION_PUT")
	{
		return -1;
	}

	return 0;
}

int MySkyway::MCloseStream(MyHttp& mysock, MyUserClass& myuser)
{
	std::string url;
	std::string sendBody;
	std::pair<std::string, std::string> strs;

	url = "/media/connections/" + myuser.MConnectionId();
	sendBody = MyHttp::MakeHttpValue("DELETE", url, skywayHost);
	mysock.SendStr(sendBody);
	strs = mysock.ReceiveStrs();
	std::cout << strs.first.c_str();

	url = "/peers/" + myuser.MPeerId() + "?token=" + myuser.MSkywayToken();
	sendBody = MyHttp::MakeHttpValue("DELETE", url, skywayHost);
	mysock.SendStr(sendBody);
	strs = mysock.ReceiveStrs();
	std::cout << strs.first.c_str();

	return 0;
}

int MySkyway::DCloseConnect(MyHttp& mysock, MyUserClass& myuser)
{
	std::string url;
	std::string sendBody;
	std::pair<std::string, std::string> strs;

	url = "/data/connections/" + myuser.DConnectionId();
	sendBody = MyHttp::MakeHttpValue("DELETE", url, skywayHost);
	mysock.SendStr(sendBody);
	strs = mysock.ReceiveStrs();
	std::cout << strs.first.c_str();

	url = "/peers/" + myuser.DPeerId() + "?token=" + myuser.DSkywayToken();
	sendBody = MyHttp::MakeHttpValue("DELETE", url, skywayHost);
	mysock.SendStr(sendBody);
	strs = mysock.ReceiveStrs();
	std::cout << strs.first.c_str();

	return 0;
}