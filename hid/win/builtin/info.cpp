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

    HANDLE handle = hid_manager.create_device_handle(device);

    if (handle != 0 && handle != INVALID_HANDLE_VALUE)
    {
        HIDD_ATTRIBUTES attributes = hid_manager.get_device_attributes(handle);
        HIDP_CAPS capabilities = hid_manager.get_device_capabilities(handle);
        std::wstring manufacturer = hid_manager.get_manufacturer_string(handle);
        std::wstring product = hid_manager.get_product_string(handle);
        std::wstring serial = hid_manager.get_serial_string(handle);

        CloseHandle(handle);
    }

    return 0;
}

int cmd_info(int argc, const char **argv)
{
    return hid_manager.process_devices(argc, argv, &info_execution_block);
}