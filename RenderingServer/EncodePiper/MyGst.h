#pragma once

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>

#include <cmath>
#include <string>
#include <algorithm>
#include <memory>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdio>
#include <time.h>
#include <iostream>
#include <chrono>
#include <stdio.h>
#include <Windows.h>
#include <fstream>

//You must call once before using gstreamer
void GstInit();
//when finish gstreamer
void GstFinish();

class MyGst
{
public:
	MyGst(int ImageWidth, int ImageHeight, int OutPort);
	~MyGst();

	void SetImageData(std::vector<std::uint8_t>& ImageData);
	void StartPlay();


private:
	const int m_ImageWidth;
	const int m_ImageHeight;
	const int m_OutPort;
	
	static std::vector<std::uint8_t>* m_ImageData;

	GstElement* m_Pipeline;
	static void appsrcGetFromBytes (GstElement* appsrc, guint unused_size, gpointer user_data);
};