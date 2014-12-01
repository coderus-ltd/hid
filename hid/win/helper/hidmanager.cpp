#include<string>
#include<sstream>
#include<vector>
#include<windows.h>
#include<setupapi.h>

#include "hidmanager.h"
#include "commandparser.h"

// Get the path to the device, used to create a device handle
static std::wstring get_device_path(HDEVINFO hInfoSet, SP_DEVICE_INTERFACE_DATA oInterface)
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

// Format is vid_0000&pid_0000
static std::wstring get_search_string(int argc, const char **argv)
{
    std::wstring ret;
    std::wstring vid = get_w_param(argc, argv, "-v");
    std::wstring pid = get_w_param(argc, argv, "-p");

    if (!vid.empty() && !pid.empty())
    {
        ret = L"vid_" + vid + L"&pid_" + pid;
    }
    else if (!vid.empty() && pid.empty())
    {
        ret = L"vid_" + vid;
    }
    else if (vid.empty() && !pid.empty())
    {
        ret = L"pid_" + pid;
    }

    return ret;
}

// Compare the devicePath and searchLocation strings to see if we should include it in the found devices
static bool compare_path_and_search_location(std::wstring devicePath, std::wstring searchLocation)
{
    bool ret;
    if (devicePath.empty())
    {
        ret = false;
    }
    else if (searchLocation.empty())
    {
        ret = true;
    }
    else
    {
        ret = devicePath.find(searchLocation) != std::string::npos;
    }

    return ret;
}

HidManager::HidManager()
{
    // Nothing yet
}

HidManager::~HidManager()
{
    // Nothing yet
}

int HidManager::process_devices(int argc, const char **argv, ProcessDeviceBlock execution_block)
{
    GUID guid;
    HidD_GetHidGuid(&guid);
    HDEVINFO hInfoSet = SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (hInfoSet == INVALID_HANDLE_VALUE)
        return 1;

    int result = 0;
    int nIndex = 0;

    std::wstring searchString = get_search_string(argc, argv);
    std::vector<std::wstring> foundDevices;

    SP_DEVICE_INTERFACE_DATA oInterface;
    oInterface.cbSize = sizeof(oInterface);

    while (SetupDiEnumDeviceInterfaces(hInfoSet, NULL, &guid, nIndex, &oInterface))
    {
        std::wstring strDevicePath = get_device_path(hInfoSet, oInterface);
        if (compare_path_and_search_location(strDevicePath, searchString))
        {
            foundDevices.push_back(strDevicePath);
        }

        nIndex++;
    }

    bool stopLoop = false;
    if (foundDevices.size() > 0)
    {
        for (unsigned int i = 0; i < foundDevices.size() && !stopLoop; i++)
        {
            result = execution_block(foundDevices[i], &stopLoop);
        }
    }

    SetupDiDestroyDeviceInfoList(hInfoSet);
    return result;
};
