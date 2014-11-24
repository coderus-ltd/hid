#include<iostream>

#ifndef HidManager_h
#define HidManager_h

typedef int(*ProcessDeviceBlock)(std::wstring);

class HidManager
{
private:
public:
    HidManager();
    int Process_Devices(int argc, const char **argv, ProcessDeviceBlock);
};

#endif