#include "hidmanager.h"
#include "hiddevice.h"

extern "C"
{
    #include "builtin.h"
}

static int getreport_execution_block(std::wstring device, bool* foundDevice)
{
    *foundDevice = true;
    HidDevice hid_device(device);
    HANDLE handle = hid_device.get_device_handle(device);

    if (handle != 0 && handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(handle);
    }

    return 0;
}

int cmd_getreport(int argc, const char **argv)
{
    HidManager hid_manager;
    return hid_manager.process_devices(argc, argv, &getreport_execution_block);
}