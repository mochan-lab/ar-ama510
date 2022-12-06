#include "MyUserClass.h"

MyUserClass::MyUserClass()
{
	ZeroMemory(&m_GameEngineSi, sizeof(m_GameEngineSi));
	m_GameEngineSi.cb = sizeof(m_GameEngineSi);
	ZeroMemory(&m_GameEnginePi, sizeof(m_GameEnginePi));
}

MyUserClass::~MyUserClass()
{

}

int MyUserClass::CreateEncodePiper()
{
	ZeroMemory(&m_EncodePiperSi, sizeof(m_EncodePiperSi));
	m_EncodePiperSi.cb = sizeof(m_EncodePiperSi);
	ZeroMemory(&m_EncodePiperPi, sizeof(m_EncodePiperPi));

	std::string lineEP = MyUserClass::EncodePiperUrl +
		" --resw " + std::to_string(this->ResolutionW()) +
		" --resh " + std::to_string(this->ResolutionH()) +
		" --portout " + std::to_string(this->MSendPort()) +
		" --pipe " + std::to_string(this->PipeNum());

	const char* src = lineEP.c_str();
	
	int bufSize = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, src, -1, NULL, 0);
	TCHAR* buf = new TCHAR[bufSize];
	MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, src, -1, buf, bufSize);

	int ret = CreateProcess(
		NULL,
		buf,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&this->m_EncodePiperSi,
		&this->m_EncodePiperPi
	);
	delete[] buf;
	if (!ret) std::cout << "CreateEncodePiper ERROR: " << GetLastError() << std::endl;
	return ret;
}

int MyUserClass::CloseEncodePiper()
{
	BOOL ret=TRUE;

	if (WaitForSingleObject(this->m_EncodePiperPi.hProcess, 5000) == WAIT_TIMEOUT)
	{
		_tprintf(_T("Terminate forcibly"));
		 TerminateProcess(this->m_EncodePiperPi.hProcess, 0);
	}
	ret &= CloseHandle(this->m_EncodePiperPi.hThread);
	ret &= CloseHandle(this->m_EncodePiperPi.hProcess);

	return ret;
}

int MyUserClass::CreateGameEngine()
{
	ZeroMemory(&m_GameEngineSi, sizeof(m_GameEngineSi));
	m_GameEngineSi.cb = sizeof(m_GameEngineSi);
	ZeroMemory(&m_GameEnginePi, sizeof(m_GameEnginePi));

	std::string lineEP = MyUserClass::GameEngineUrlND() +
		" --resw " + std::to_string(this->ResolutionW()) +
		" --resh " + std::to_string(this->ResolutionH()) +
		" --portrecv " + std::to_string(this->DRecvPort()) +
		" --portsend " + std::to_string(this->DSendPort()) +
		" --pipe " + std::to_string(this->PipeNum());
	const char* src = lineEP.c_str();
	int bufSize = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, src, -1, NULL, 0);
	TCHAR* buf = new TCHAR[bufSize];
	MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, src, -1, buf, bufSize);



	//std::string lineEXE = MyUserClass::GameEngineUrl;
	//const char* exe = lineEXE.c_str();
	//int exeBufSize = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, exe, -1, NULL, 0);
	//TCHAR* exeBuf = new TCHAR[bufSize];
	//MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, exe, -1, exeBuf, exeBufSize);

	//std::string argLine =
	//	"gameengine --resw " + std::to_string(this->ResolutionW()) +
	//	" --resh " + std::to_string(this->ResolutionH()) +
	//	" --portrecv " + std::to_string(this->DRecvPort()) +
	//	" --portsend " + std::to_string(this->DSendPort()) +
	//	" --pipe " + std::to_string(this->PipeNum());
	//const char* argsrc = argLine.c_str();
	//int argBufSize = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, argsrc, -1, NULL, 0);
	//TCHAR* argBuf = new TCHAR[argBufSize];
	//MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, argsrc, -1, argBuf, exeBufSize);

	int ret = CreateProcess(
		NULL,
		buf,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&this->m_GameEngineSi,
		&this->m_GameEnginePi
	);
	delete[] buf;

	/*delete[] argBuf;
	delete[] exeBuf;*/

	if (!ret) std::cout << "CreateGameEngine ERROR: " << GetLastError() << std::endl;
	return ret;
}

int MyUserClass::CloseGameEngine()
{
	auto handle = MyUserClass::GetWindowHandle(this->m_GameEnginePi.dwProcessId);
	PostMessage(handle, WM_CLOSE, 0, 0);

	std::cout << m_GameEnginePi.dwProcessId << std::endl;

	// Terminate forcibly if application does not quit in 5sec.
	if (WaitForSingleObject(this->m_GameEnginePi.hProcess, 5000) == WAIT_TIMEOUT)
	{
		_tprintf(_T("Terminate forcibly"));
		//TerminateProcess(this->m_GameEnginePi.hProcess, 0);
	}

	BOOL ret = TRUE;
	UINT exitCode = 0;
	//ret &= TerminateProcess(this->m_GameEnginePi.hProcess, exitCode);
	ret &= CloseHandle(this->m_GameEnginePi.hThread);
	ret &= CloseHandle(this->m_GameEnginePi.hProcess);

	return ret;
}

HWND MyUserClass::GetWindowHandle(const DWORD target_id)
{
	auto hWnd = GetTopWindow(nullptr);
	do {
		if (GetWindowLong(hWnd, GWLP_HWNDPARENT) != 0 || !IsWindowVisible(hWnd))
			continue;

		DWORD process_id;
		GetWindowThreadProcessId(hWnd, &process_id);
		if (target_id == process_id)
			return hWnd;
	} while ((hWnd = GetNextWindow(hWnd, GW_HWNDNEXT)) != nullptr);

	return nullptr;
}

std::string MyUserClass::GameEngineUrlND()
{
	std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(p);
	std::tm now;
	localtime_s(&now,&t);

	if (now.tm_hour >= 17 || 6 > now.tm_hour) {
		return GameEngineUrlNight;
	}
	else {
		return GameEngineUrlDay;
	}

}