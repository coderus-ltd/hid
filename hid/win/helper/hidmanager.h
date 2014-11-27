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

    HANDLE create_device_handle(std::wstring);
    HIDD_ATTRIBUTES get_device_attributes(HANDLE);
    HIDP_CAPS get_device_capabilities(HANDLE);
    HIDP_VALUE_CAPS get_device_capability_values(HANDLE, HIDP_REPORT_TYPE, PUSHORT);
    std::wstring get_manufacturer_string(HANDLE);
    std::wstring get_product_string(HANDLE);
    std::wstring get_serial_string(HANDLE);

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