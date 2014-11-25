#include<string>
#include<sstream>
#include<iomanip>
#include "hidmanager.h"
extern "C"
{
    #include "builtin.h"
}

/* Members */
static HidManager hid_manager;

/* Methods */
static int list_execution_block(std::wstring device, bool* foundDevice)
{
    HANDLE handle = hid_manager.Create_Device_Handle(device);
    
    if (handle != 0 && handle != INVALID_HANDLE_VALUE)
    {
        HIDD_ATTRIBUTES attributes = hid_manager.Get_Device_Attributes(handle);
        
        std::cout << "Locn\t";
        std::cout << hid_manager.number_to_hex_string(attributes.VendorID) << "\t";
        std::cout << hid_manager.number_to_hex_string(attributes.ProductID) << "\t";

        std::wstring product = hid_manager.Get_Product_String(handle);
        std::wcout << hid_manager.clean_string(product) << std::endl;

        CloseHandle(handle);
    }

    return 0;
}

int cmd_list(int argc, const char **argv)
{
    std::cout << "LID\tVID\tPID\tDescription" << std::endl;
    return hid_manager.Process_Devices(argc, argv, &list_execution_block);
}