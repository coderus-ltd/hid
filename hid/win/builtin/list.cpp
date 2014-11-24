#include "hidmanager.h"
extern "C"
{
    #include "builtin.h"
}

int execution_block(std::wstring path)
{
    std::cout << "Something is happening";
    return 0;
}

int cmd_list(int argc, const char **argv)
{
    HidManager hid_manager;
    hid_manager.Process_Devices(argc, argv, &execution_block);

    return 0;
}