#include "main.h"


int main(int argc, char* argv[])
{
	int resw = 1280;
	int resh = 720;
	int portNum = 60008;
	int pipeNum = 0;
	//CommandLine
	std::map<std::string, int>CommandLineValue;

	for (int i = 1; i+1<=argc; i+=2)
	{
		char* key = argv[i];
		int val = std::atoi(argv[i + 1]);
		CommandLineValue[key] = val;
	}
	if (CommandLineValue.count("--resw"))
	{
		resw = CommandLineValue["--resw"];
	}
	if (CommandLineValue.count("--resh"))
	{
		resh = CommandLineValue["--resh"];
	}
	if (CommandLineValue.count("--portout"))
	{
		portNum = CommandLineValue["--portout"];
	}
	if (CommandLineValue.count("--pipe"))
	{
		pipeNum = CommandLineValue["--pipe"];
	}

	std::printf("set width:%d, height:%d, portout:%d, pipeNum: %d", resw, resh, portNum, pipeNum);

	bool isQuit = false;

	std::thread t0([&]
		{
			ConnectPipe(resw, resh, pipeNum,portNum);
			isQuit = true;
		});

	while (!isQuit)
	{
		std::string line;
		std::getline(std::cin, line);
		if (line == "quitpipe")
		{
			isQuit = true;
			disPipeConnect();
		}
		else
		{
			std::cout << "unknown command\n" << std::endl;
		}
	}

	t0.join();
	return 0;
}