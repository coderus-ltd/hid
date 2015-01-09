#include<string>
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
    // TODO :: We should be able to filter by LID (-l)
    std::wstring ret;
    std::wstring rawVid = get_w_param(argc, argv, "-v");
    std::wstring rawPid = get_w_param(argc, argv, "-p");
    std::wstring vid = rawVid.find(L"0x") == 0 ? rawVid.substr(2, rawVid.length()) : rawVid;
    std::wstring pid = rawPid.find(L"0x") == 0 ? rawPid.substr(2, rawPid.length()) : rawPid;

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

static std::vector<std::wstring> get_search_strings(int argc, const char **argv)
{
    std::vector<std::wstring> ret;
    std::wstring rawVid = get_w_param(argc, argv, "-v");
    std::wstring rawPid = get_w_param(argc, argv, "-p");

    std::wstring vid = rawVid.find(L"0x") == 0 ? rawVid.substr(2, rawVid.length()) : rawVid;
    std::wstring pid = rawPid.find(L"0x") == 0 ? rawPid.substr(2, rawPid.length()) : rawPid;
    std::wstring lid = get_w_param(argc, argv, "-l");
    
    // Vid
    if (!vid.empty())
        ret.push_back(L"vid_" + vid);
        
    // Pid
    if (!pid.empty())
        ret.push_back(L"pid_" + pid);
    
    // Lid
    if (!lid.empty())
        ret.push_back(lid);
    
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

// Compare the devicePath and searchLocation strings to see if we should include it in the found devices
static bool compare_path_and_search_locations(std::wstring devicePath, std::vector<std::wstring> searchLocations)
{
    bool ret;
    if (devicePath.empty())
    {
        ret = false;
    }
    else if (searchLocations.size() == 0)
    {
        ret = true;
    }
    else
    {
        for (unsigned int i = 0; i < searchLocations.size(); i++)
        {
            ret = devicePath.find(searchLocations[i]) != std::string::npos;

            if (!ret) 
                break;
        }
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

    std::vector<std::wstring> searchStrings = get_search_strings(argc, argv);
    std::vector<std::wstring> foundDevices;

    SP_DEVICE_INTERFACE_DATA oInterface;
    oInterface.cbSize = sizeof(oInterface);

    while (SetupDiEnumDeviceInterfaces(hInfoSet, NULL, &guid, nIndex, &oInterface))
    {
        std::wstring strDevicePath = get_device_path(hInfoSet, oInterface);
        if (compare_path_and_search_locations(strDevicePath, searchStrings))
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
