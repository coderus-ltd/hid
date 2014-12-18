#include "hidmanager.h"
#include "hiddevice.h"
#include "commandparser.h"

extern "C"
{
    #include "builtin.h"
}

/* Members */
static int reportType;

static int getreport_execution_block(std::wstring device, bool* foundDevice)
{
    *foundDevice = true;
    HidDevice hid_device(device);
    HANDLE handle = hid_device.get_device_handle(device);

    if (handle != 0 && handle != INVALID_HANDLE_VALUE)
    {
        HIDP_CAPS caps = hid_device.get_device_capabilities(handle);

        //prepare the report
        const size_t reportDataSize = caps.OutputReportByteLength;
        char* reportData = new char[reportDataSize];

        // set report id
        reportData[0] = 0x01;
        if (HidD_GetInputReport(handle, reportData, reportDataSize)) {
            std::cout << reportData << std::endl;
        } else {
            std::cout << "getreport: get error (Code: "<< GetLastError() << ")" << std::endl;
        }

        CloseHandle(handle);
    }

    return 0;
}

int cmd_getreport(int argc, const char **argv)
{
    HidManager hid_manager;
    return hid_manager.process_devices(argc, argv, &getreport_execution_block);
}