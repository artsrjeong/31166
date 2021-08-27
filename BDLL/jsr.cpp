#include "stdafx.h"
#include "common.h"

int k;

extern "C" __declspec(dllexport) void putK(int val)
{
	k=val;
}

extern "C" __declspec(dllexport) int getK()
{
	return k;
}