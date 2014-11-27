#include "hidmanager.h"

extern "C"
{
    #include "builtin.h"
}

/* Members */
static HidManager hid_manager;
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

    int ret = hid_manager.Process_Devices(argc, argv, &attached_execution_block);

    if (deviceAttached == 0)
        std::cout << 0;
    else
        std::cout << 1;

    return ret;
}