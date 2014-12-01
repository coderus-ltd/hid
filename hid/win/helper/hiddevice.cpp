#include "hiddevice.h"

/* Members */
static std::wstring device_path;

/* Methods */
// Find where null termination occurs in the buffer
static int find_null_termination(unsigned char buffer[], std::wstring searchString)
{
    int retvalue = 0;
    for (unsigned int i = 0; i < searchString.length(); i++)
    {
        if (buffer[i] == '\0' && buffer[i + 1] == '\0')
        {
            retvalue = i;
        }
    }

    return retvalue;
}

HidDevice::HidDevice(std::wstring devicePath)
{
    device_path = devicePath;
}

HidDevice::~HidDevice()
{
    // Nothing yet
}

std::wstring HidDevice::get_device_path()
{
    return device_path;
}

HANDLE HidDevice::get_device_handle(std::wstring devicePath)
{
    HANDLE handle = CreateFile(
        devicePath.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        0,
        OPEN_EXISTING,
        0,
        0);

    return handle;
}

HANDLE HidDevice::get_device_handle()
{
    return get_device_handle(device_path);
}

HIDD_ATTRIBUTES HidDevice::get_device_attributes(HANDLE handle)
{
    HIDD_ATTRIBUTES attributes;
    attributes.Size = sizeof(attributes);

    HidD_GetAttributes(handle, &attributes);
    return attributes;
}

HIDP_CAPS HidDevice::get_device_capabilities(HANDLE handle)
{
    PHIDP_PREPARSED_DATA ppData;
    HIDP_CAPS capabilities;

    HidD_GetPreparsedData(handle, &ppData);
    HidP_GetCaps(ppData, &capabilities);
    HidD_FreePreparsedData(ppData);

    return capabilities;
}

HIDP_VALUE_CAPS HidDevice::get_device_capability_values(HANDLE handle, HIDP_REPORT_TYPE report_type, PUSHORT report_length)
{
    PHIDP_PREPARSED_DATA ppData;
    HIDP_VALUE_CAPS capabilities;

    HidD_GetPreparsedData(handle, &ppData);
    HidP_GetValueCaps(report_type, &capabilities, report_length, ppData);
    HidD_FreePreparsedData(ppData);

    return capabilities;
}

std::wstring HidDevice::get_manufacturer_string(HANDLE handle)
{
    unsigned char buffer[126];
    HidD_GetManufacturerString(handle, &buffer, 126);

    std::wstring buf(buffer, buffer + sizeof buffer / sizeof buffer[0]);
    std::wstring ret = buf.substr(0, find_null_termination(buffer, buf));

    return ret;
}

std::wstring HidDevice::get_product_string(HANDLE handle)
{
    unsigned char buffer[126];
    HidD_GetProductString(handle, &buffer, 126);

    std::wstring buf(buffer, buffer + sizeof buffer / sizeof buffer[0]);
    std::wstring ret = buf.substr(0, find_null_termination(buffer, buf));

    return ret;
}

std::wstring HidDevice::get_serial_string(HANDLE handle)
{
    unsigned char buffer[126];
    HidD_GetSerialNumberString(handle, &buffer, 126);

    std::wstring buf(buffer, buffer + sizeof buffer / sizeof buffer[0]);
    std::wstring ret = buf.substr(0, find_null_termination(buffer, buf));

    return ret;
}

std::wstring HidDevice::clean_string(std::wstring old_string)
{
    std::wstring ret;
    for (unsigned int i = 0; i < old_string.length(); i++)
    {
        if (old_string[i] != '\0')
        {
            ret.push_back(old_string[i]);
        }
    }
    return ret;
}