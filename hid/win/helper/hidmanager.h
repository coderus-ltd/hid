#include<iostream>
#include<windows.h>
#include<hidsdi.h>

#ifndef HidManager_h
#define HidManager_h

typedef int(*ProcessDeviceBlock)(std::wstring, bool*);

class HidManager
{
public:
    HidManager();
    ~HidManager();

    int process_devices(int argc, const char **argv, ProcessDeviceBlock);
};

#endif