#include<string>
#include<windows.h>
#include<hidsdi.h>

#ifndef hiddevice_h
#define hiddevice_h

class HidDevice
{
public:
    HidDevice(std::wstring);
    ~HidDevice();

    std::wstring get_device_path();

    // Get a HANDLE to the device
    HANDLE get_device_handle(std::wstring);

    // Get a HANDLE to the device override which uses the path from the constructor
    HANDLE get_device_handle();

    // Get a populated HIDD_ATTRIBUTES object from HANDLE
    HIDD_ATTRIBUTES get_device_attributes(HANDLE);

    // Get a populated HIDP_CAPS object from HANDLE
    HIDP_CAPS get_device_capabilities(HANDLE);

    // Get a populated HIDP_VALUE_CAPS object from HANDLE
    HIDP_VALUE_CAPS get_device_capability_values(HANDLE, HIDP_REPORT_TYPE, PUSHORT);

    // Gets the manufacturer string from HANDLE
    std::wstring get_manufacturer_string(HANDLE);

    // Gets the product string from HANDLE
    std::wstring get_product_string(HANDLE);

    // Gets the serial string from HANDLE
    std::wstring get_serial_string(HANDLE);
};

#endif