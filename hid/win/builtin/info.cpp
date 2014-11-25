#include<string>
#include "hidmanager.h"

extern "C"
{
    #include "builtin.h"
}

/* Members */
static HidManager hid_manager;

/* Methods */
static int info_execution_block(std::wstring device, bool* foundDevice)
{
    // if not the right device
    //*foundDevice = true;

    HANDLE handle = hid_manager.Create_Device_Handle(device);

    if (handle != 0 && handle != INVALID_HANDLE_VALUE)
    {
        HIDD_ATTRIBUTES attributes = hid_manager.Get_Device_Attributes(handle);
        HIDP_CAPS capabilities = hid_manager.Get_Device_Capabilities(handle);
        std::wstring manufacturer = hid_manager.Get_Manufacturer_String(handle);
        std::wstring product = hid_manager.Get_Product_String(handle);
        std::wstring serial = hid_manager.Get_Serial_String(handle);

        CloseHandle(handle);
    }

    return 0;
}

int cmd_info(int argc, const char **argv)
{
    return hid_manager.Process_Devices(argc, argv, &info_execution_block);
}