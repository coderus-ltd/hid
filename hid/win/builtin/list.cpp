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
    HANDLE handle = hid_manager.create_device_handle(device);
    
    if (handle != 0 && handle != INVALID_HANDLE_VALUE)
    {
        HIDD_ATTRIBUTES attributes = hid_manager.get_device_attributes(handle);
        
        std::cout << "Locn\t";
        std::cout << hid_manager.number_to_hex_string(attributes.VendorID) << "\t";
        std::cout << hid_manager.number_to_hex_string(attributes.ProductID) << "\t";

        std::wstring product = hid_manager.get_product_string(handle);
        std::wcout << hid_manager.clean_string(product) << std::endl;

        CloseHandle(handle);
    }

    return 0;
}

int cmd_list(int argc, const char **argv)
{
    std::cout << "LID\tVID\tPID\tDescription" << std::endl;
    return hid_manager.process_devices(argc, argv, &list_execution_block);
}