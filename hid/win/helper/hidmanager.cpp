#include<string>
#include<vector>
#include<windows.h>
#include<setupapi.h>
#include<hidsdi.h>

#include "hidmanager.h"

std::wstring Get_Device_Path(HDEVINFO hInfoSet, SP_DEVICE_INTERFACE_DATA oInterface)
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

HidManager::HidManager()
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