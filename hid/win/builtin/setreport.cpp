#include "hidmanager.h"
#include "hiddevice.h"
#include "commandparser.h"
#include <stdio.h>   
#include <stdlib.h>
#include <io.h>
#include <vector>

extern "C"
{
#include "builtin.h"
}

/* Members */
static int waitTime = 0;
static std::vector<std::wstring> commands;

static int readReport(std::wstring device) {
	HidDevice hid_device(device);
	HANDLE handle = hid_device.get_device_handle(device);

	if (handle != 0 && handle != INVALID_HANDLE_VALUE)
	{
		HIDP_CAPS caps = hid_device.get_device_capabilities(handle);

		const int outputBufferSize = caps.OutputReportByteLength;
		char *outputBuffer = (char*)malloc(outputBufferSize * sizeof(char));
		DWORD dwRead = 0;
		/*
		// also needs new params on the CreateFile (overlap flag, etc)
		OVERLAPPED overlap = { 0 };
		overlap.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		*/

		int rf = ReadFile(handle, outputBuffer, outputBufferSize, &dwRead, NULL /* &overlap*/);
		if (!rf && GetLastError() != ERROR_IO_PENDING) {
			std::cout << "sendreport: read error (code: " << GetLastError() << ")" << std::endl;
		} else {
			/*
			WaitForSingleObject(&(overlap.hEvent), waitTime * 1000);
			*/

			//format buffer (remove initial position)
			const int formatedBufferSize = outputBufferSize - 1;
			char *formatedBuffer = (char*)malloc(formatedBufferSize * sizeof(char));

#pragma warning( push )
#pragma warning( disable : 4996)
			strncpy(formatedBuffer, &outputBuffer[1], formatedBufferSize);
#pragma warning( pop ) 

			std::cout << formatedBuffer << std::endl;
		}
	}

	return 0;
}

static int setreport_execution_block(std::wstring device, bool* foundDevice)
{
	*foundDevice = true;
	HidDevice hid_device(device);
	HANDLE handle = hid_device.get_device_handle(device);
	HIDP_CAPS caps = hid_device.get_device_capabilities(handle);

	if (handle != 0 && handle != INVALID_HANDLE_VALUE)
	{
		for (std::wstring command : commands) {
			const int reportDataSize = command.size() + 1; //+1 - termination character (\0)
			const int sendingReportSize = caps.OutputReportByteLength;

			if (reportDataSize > sendingReportSize) {
				std::cout << "warn: report larger than maximum size" << std::endl;
			}

			//prepare report data	
			char* reportData = (char*)malloc(reportDataSize * sizeof(char));

#pragma warning( push )
#pragma warning( disable : 4996)
			wcstombs(reportData, command.c_str(), reportDataSize);
#pragma warning( pop ) 

			//prepare report
			char* reportBuffer = (char*)malloc(sendingReportSize * sizeof(char));

			// set report id
			reportBuffer[0] = 0x01;

#pragma warning( push )
#pragma warning( disable : 4996)
			// copy data into the report buffer
			strncpy(&reportBuffer[1], reportData, sendingReportSize);
#pragma warning( pop ) 

			// send the report
			if (HidD_SetOutputReport(handle, reportBuffer, sendingReportSize)) {
				readReport(device); //read report
			} else {
				std::cout << "setreport: send error (code: " << GetLastError() << ")" << std::endl;
			}
		}

		CloseHandle(handle);
	}

	return 0;
}

int cmd_setreport(int argc, const char **argv)
{
#pragma warning( push )
#pragma warning( disable : 4996)
	if (isatty(fileno(stdin))) { //stdin is a terminal
#pragma warning( pop ) 
		//lookup -d
		for (int i = 1; i < argc; i++) {
			if (strcmp(argv[i], "-d") == 0)
			{
				for (int j = i + 1; j < argc; j++)
				{
					if (argv[j][0] == '-')
					{
						break;
					}

					//parse commands
					const char* str = argv[j];
					std::wstring data(str, str + strlen(str));
					if (data.length() % 2 == 0 && data.substr(0, 2) == L"0x") {
						commands.push_back(data.substr(2, data.length()));
					}
					else {
						commands.push_back(data);
					}
				}

				break;
			}
		}
	} else { //stdin is a file or a pipe
		const int size = 255;
		std::vector<char> buf(size);
		std::cin.read(buf.data(), buf.size());
		if (!buf.empty()) {
			std::wstring str(buf.begin(), buf.end());
			commands.push_back(str);
		}
	}
	
	if (commands.size() == 0) {
		std::cout << 0 << std::endl;
		return 0;
	}
	
	std::wstring _waitTime = get_w_param(argc, argv, "-w");
	if (!_waitTime.empty()) {
		int wt = std::stoi(_waitTime);
		waitTime = wt > 1 ? wt : 1;
	}

	HidManager hid_manager;
	return hid_manager.process_devices(argc, argv, &setreport_execution_block);
}