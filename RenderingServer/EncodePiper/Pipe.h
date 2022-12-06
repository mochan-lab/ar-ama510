#pragma once

#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <iostream>
#include <cstdint>
#include <vector>

#include "MyGst.h"

int ConnectPipe(int resw = 1280, int resh = 720, int pipeNum = 0, int portNum=60008);
int ForceClosePipe();

void disPipeConnect();