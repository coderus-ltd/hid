#include<string>
#include<sstream>
#include<iomanip>
#include "hidmanager.h"
#include "hiddevice.h"

extern "C"
{
    #include "builtin.h"
}

/* Methods */
static int list_execution_block(std::wstring device, bool* foundDevice)
{
    HidDevice hid_device(device);
    HANDLE handle = hid_device.get_device_handle(device);
    
    if (handle != 0 && handle != INVALID_HANDLE_VALUE)
    {
        HIDD_ATTRIBUTES attributes = hid_device.get_device_attributes(handle);
        
        std::wcout << hid_device.get_sanitised_device_path(hid_device.get_device_path()) << "\t";
        std::cout << hid_device.number_to_hex_string(attributes.VendorID) << "\t";
        std::cout << hid_device.number_to_hex_string(attributes.ProductID) << "\t";

        std::wstring product = hid_device.get_product_string(handle);
        if (product.empty())
        {
            std::wcout << hid_device.clean_string(product) << std::endl;
        }
        else
        {
            // If we were to support Bluetooth or another type of HID device this could be changed
            // to include that device type instead of hard coded USB
            std::wcout << hid_device.clean_string(product) << " via USB" << std::endl;
        }

        CloseHandle(handle);
    }

    return 0;
}

int cmd_list(int argc, const char **argv)
{
    HidManager hid_manager;
    std::cout << "LID\t\t\tVID\tPID\tDescription" << std::endl;
    return hid_manager.process_devices(argc, argv, &list_execution_block);
}