#include<string>
#include "hidmanager.h"
#include "hiddevice.h"

extern "C"
{
    #include "builtin.h"
}

/* Methods */
static int info_execution_block(std::wstring device, bool* foundDevice)
{
    HidDevice hid_device(device);
    HANDLE handle = hid_device.get_device_handle(device);

    if (handle != 0 && handle != INVALID_HANDLE_VALUE)
    {
        HIDD_ATTRIBUTES attributes = hid_device.get_device_attributes(handle);
        HIDP_CAPS capabilities = hid_device.get_device_capabilities(handle);
        std::wstring manufacturer = hid_device.get_manufacturer_string(handle);
        std::wstring product = hid_device.get_product_string(handle);
        std::wstring serial = hid_device.get_serial_string(handle);

        CloseHandle(handle);
    }

    return 0;
}

int cmd_info(int argc, const char **argv)
{
    HidManager hid_manager;
    return hid_manager.process_devices(argc, argv, &info_execution_block);
}