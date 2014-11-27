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

    template< typename T >
    std::string number_to_hex_string(T i)
    {
        std::stringstream stream;
        stream << "0x"
            << std::setfill('0') << std::setw(sizeof(T) * 2)
            << std::hex << i;
        return stream.str();
    }

    std::wstring clean_string(std::wstring old_string);
};

#endif