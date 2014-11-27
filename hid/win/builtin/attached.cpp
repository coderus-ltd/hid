#include "hidmanager.h"

extern "C"
{
    #include "builtin.h"
}

/* Members */
static bool deviceAttached;

/* Methods */
static int attached_execution_block(std::wstring device, bool* foundDevice)
{
    deviceAttached = true;
    return 0;
}

int cmd_attached(int argc, const char**argv)
{
    deviceAttached = false;

    HidManager hid_manager;
    int ret = hid_manager.process_devices(argc, argv, &attached_execution_block);

    if (!deviceAttached)
        std::cout << 0;
    else
        std::cout << 1;

    return ret;
}