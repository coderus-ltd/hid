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

    int Process_Devices(int argc, const char **argv, ProcessDeviceBlock);

    HANDLE Create_Device_Handle(std::wstring);
    HIDD_ATTRIBUTES Get_Device_Attributes(HANDLE);
    HIDP_CAPS Get_Device_Capabilities(HANDLE);
    HIDP_VALUE_CAPS Get_Device_Capability_Values(HANDLE, HIDP_REPORT_TYPE, PUSHORT);
    std::wstring Get_Manufacturer_String(HANDLE);
    std::wstring Get_Product_String(HANDLE);
    std::wstring Get_Serial_String(HANDLE);

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