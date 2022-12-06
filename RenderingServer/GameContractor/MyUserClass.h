#pragma once

#include <WinSock2.h>
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <chrono>
#include <ctime>

#include "MyHttp.h"

class MyUserClass
{
public:
	MyUserClass();
	~MyUserClass();

private: //D->data, M->media
	std::string m_userName;
	std::string m_DPeerId;
	std::string m_MPeerId;
	int m_pipeNum;
	int m_resolution[2] = {1280,720}; //[w,h]
	int m_DSendPort;
	int m_MSendPort;
	int m_DRecvPort;
	std::string m_MId;
	std::string m_MConnectionId;
	std::string m_DId;
	std::string m_DConnectionId;
	std::string m_MSkywayToken;
	std::string m_DSkywayToken;
	STARTUPINFO m_EncodePiperSi;
	PROCESS_INFORMATION m_EncodePiperPi;
	STARTUPINFO m_GameEngineSi;
	PROCESS_INFORMATION m_GameEnginePi;

public:
	//Getter
	std::string UserName() { return this->m_userName; }
	std::string DPeerId() { return this->m_DPeerId; }
	std::string MPeerId() { return this->m_MPeerId; }
	int PipeNum() { return this->m_pipeNum; }
	void Resolution(int ret[2])
	{
		ret[0] = this->m_resolution[0];
		ret[1] = this->m_resolution[1];
	}
	int ResolutionW() { return this->m_resolution[0]; }
	int ResolutionH() { return this->m_resolution[1]; }
	int DSendPort() { return this->m_DSendPort; }
	int MSendPort() { return this->m_MSendPort; }
	int DRecvPort() { return this->m_DRecvPort; }
	std::string MId() { return this->m_MId; }
	std::string MConnectionId() { return this->m_MConnectionId; }
	std::string DId() { return this->m_DId; }
	std::string DConnectionId() { return this->m_DConnectionId; }
	std::string MSkywayToken() { return this->m_MSkywayToken; }
	std::string DSkywayToken() { return this->m_DSkywayToken; }

	//Setter
	void SetUserName(std::string s) { this->m_userName = s; }
	void SetDPeerId(std::string s) { this->m_DPeerId = s; }
	void SetMPeerId(std::string s) { this->m_MPeerId = s; }
	void SetPipeNum(int i) { this->m_pipeNum = i; }
	void SetResolution(int ret[2])
	{
		this->m_resolution[0] = ret[0];
		this->m_resolution[1] = ret[1];
	}
	void SetDSendPort(int i) { this->m_DSendPort = i; }
	void SetMSendPort(int i) { this->m_MSendPort = i; }
	void SetDRecvPort(int i) { this->m_DRecvPort = i; }
	void SetMId(std::string s) { this->m_MId = s; }
	void SetMConnectionId(std::string s) { this->m_MConnectionId = s; }
	void SetDId(std::string s) { this->m_DId = s; }
	void SetDConnectionId(std::string s) { this->m_DConnectionId = s; }
	void SetMSkywayToken(std::string s) { this->m_MSkywayToken = s; }
	void SetDSkywayToken(std::string s) { this->m_DSkywayToken = s; }

	int CreateEncodePiper();
	int CloseEncodePiper();

	int CreateGameEngine();
	int CloseGameEngine();

	static std::string EncodePiperUrl;
	static std::string GameEngineUrlNight;
	static std::string GameEngineUrlDay;

	static HWND GetWindowHandle(const DWORD target_id);

	static std::string GameEngineUrlND();
};

