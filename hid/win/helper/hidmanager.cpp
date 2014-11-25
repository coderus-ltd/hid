#include<string>
#include<sstream>
#include<vector>
#include<windows.h>
#include<setupapi.h>

#include "hidmanager.h"

static std::wstring Get_Device_Path(HDEVINFO hInfoSet, SP_DEVICE_INTERFACE_DATA oInterface)
{
    std::wstring retvalue;
    DWORD nRequiredSize = 0;

    if (!SetupDiGetDeviceInterfaceDetail(hInfoSet, &oInterface, NULL, 0, &nRequiredSize, NULL))
    {
        PSP_DEVICE_INTERFACE_DETAIL_DATA oDetail =
            (PSP_DEVICE_INTERFACE_DETAIL_DATA)HeapAlloc(GetProcessHeap(),
            HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY,
            nRequiredSize);

        oDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        if (SetupDiGetDeviceInterfaceDetail(hInfoSet, &oInterface, oDetail, nRequiredSize, &nRequiredSize, NULL))
        {
            if (oDetail->DevicePath != NULL)
            {
                retvalue = (oDetail->DevicePath);
            }
        }

        HeapFree(GetProcessHeap(), 0, oDetail);
    }

    return retvalue;
}

static int Find_Null_Termination(unsigned char buffer[], std::wstring searchString)
{
    int retvalue = 0;
    for (int i = 0; i < searchString.length(); i++)
    {
        if (buffer[i] == '\0' && buffer[i + 1] == '\0')
        {
            retvalue = i;
        }
    }

    return retvalue;
}

HidManager::HidManager()
{
    // Nothing yet
}

HidManager::~HidManager()
{
    // Nothing yet
}

int HidManager::Process_Devices(int argc, const char **argv, ProcessDeviceBlock execution_block)
{
    GUID guid;
    HidD_GetHidGuid(&guid);
    HDEVINFO hInfoSet = SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (hInfoSet == INVALID_HANDLE_VALUE)
        return 1;

    int result = 0;
    int nIndex = 0;

    std::vector<std::wstring> foundDevices;

    SP_DEVICE_INTERFACE_DATA oInterface;
    oInterface.cbSize = sizeof(oInterface);

    while (SetupDiEnumDeviceInterfaces(hInfoSet, NULL, &guid, nIndex, &oInterface))
    {
        std::wstring strDevicePath = Get_Device_Path(hInfoSet, oInterface);
        if (!strDevicePath.empty())
        {
            foundDevices.push_back(strDevicePath);
        }

        nIndex++;
    }

    if (foundDevices.size() > 0)
    {
        for (int i = 0; i < foundDevices.size(); i++)
        {
            execution_block(foundDevices[i]);
        }
    }

    SetupDiDestroyDeviceInfoList(hInfoSet);
    return result;
};

HANDLE HidManager::Create_Device_Handle(std::wstring devicePath)
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

HIDD_ATTRIBUTES HidManager::Get_Device_Attributes(HANDLE handle)
{
    HIDD_ATTRIBUTES attributes;
    attributes.Size = sizeof(attributes);

    HidD_GetAttributes(handle, &attributes);
    return attributes;
}

HIDP_CAPS HidManager::Get_Device_Capabilities(HANDLE handle)
{
    PHIDP_PREPARSED_DATA ppData;
    HIDP_CAPS capabilities;

    HidD_GetPreparsedData(handle, &ppData);
    HidP_GetCaps(ppData, &capabilities);
    HidD_FreePreparsedData(ppData);
    
    return capabilities;
}

HIDP_VALUE_CAPS HidManager::Get_Device_Capability_Values(HANDLE handle, HIDP_REPORT_TYPE report_type, PUSHORT report_length)
{
    PHIDP_PREPARSED_DATA ppData;
    HIDP_VALUE_CAPS capabilities;

    HidD_GetPreparsedData(handle, &ppData);
    HidP_GetValueCaps(report_type, &capabilities, report_length, ppData);
    HidD_FreePreparsedData(ppData);

    return capabilities;
}

std::wstring HidManager::Get_Manufacturer_String(HANDLE handle)
{
    unsigned char buffer[126];
    HidD_GetManufacturerString(handle, &buffer, 126);

    std::wstring buf(buffer, buffer + sizeof buffer / sizeof buffer[0]);
    std::wstring ret = buf.substr(0, Find_Null_Termination(buffer, buf));

    return ret;
}

std::wstring HidManager::Get_Product_String(HANDLE handle)
{
    unsigned char buffer[126];
    HidD_GetProductString(handle, &buffer, 126);

    std::wstring buf(buffer, buffer + sizeof buffer / sizeof buffer[0]);
    std::wstring ret = buf.substr(0, Find_Null_Termination(buffer, buf));

    return ret;
}

std::wstring HidManager::Get_Serial_String(HANDLE handle)
{
    unsigned char buffer[126];
    HidD_GetSerialNumberString(handle, &buffer, 126);

    std::wstring buf(buffer, buffer + sizeof buffer / sizeof buffer[0]);
    std::wstring ret = buf.substr(0, Find_Null_Termination(buffer, buf));

    return ret;
}

std::wstring HidManager::clean_string(std::wstring old_string)
{
    std::wstring ret;
    for (int i = 0; i < old_string.length(); i++)
    {
        if (old_string[i] != '\0')
        {
            ret.push_back(old_string[i]);
        }
    }
    return ret;
}
