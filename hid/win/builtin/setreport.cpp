#include "hidmanager.h"
#include "hiddevice.h"
#include "commandparser.h"
#include <stdio.h>   
#include <stdlib.h>
#include <io.h>
#include <vector>
#include <time.h>
#include <algorithm>

extern "C"
{
#include "builtin.h"
}

/* Members */
static std::wstring reportType = L"output";
static int reportId = 1;
static int waitTime = 0;
static std::vector<std::string> commands;

static std::string dataFromHexString(std::string hexString) {
	std::string _hex = hexString.substr(0, 2) == "0x" ? hexString.substr(2, hexString.length()) : hexString;
	int i = 0, byte = 0, len = _hex.length();
	const int charsLength = (len / 2) + 1 /*+1 = \0*/;
	char *chars = new char[charsLength];
	while (i < len)
	{
		std::string _hexByteStr = _hex.substr(i, 2);
		int decVal = std::stoul(_hexByteStr, nullptr, 16);
		chars[byte] = decVal;
		i = +2;
		byte++;
	}
	chars[byte] = 0;

	return std::string(chars);
}

static char* readData(HANDLE& handle, HidDevice hid_device) {
	char* data = NULL;

	if (handle != 0 && handle != INVALID_HANDLE_VALUE)
	{
		HIDP_CAPS caps = hid_device.get_device_capabilities(handle);
		const size_t outputBufferSize = caps.OutputReportByteLength;

		char* outputBuffer = new char[outputBufferSize];

		DWORD dwRead = 0;
		BOOL res;
		OVERLAPPED overlap = {};
		overlap.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		HANDLE ev = overlap.hEvent;

		if (!ReadFile(handle, outputBuffer, outputBufferSize, &dwRead, &overlap))
		{
			if (GetLastError() != ERROR_IO_PENDING)
			{
				std::cout << "not io pending" << std::endl;
				CloseHandle(ev);
				return 0;
			}
		}

		res = WaitForSingleObject(ev, waitTime == 0 ? 100 : waitTime * 1000);
		if (res != WAIT_OBJECT_0)
		{
			CloseHandle(ev);
			return 0;
		}

		res = GetOverlappedResult(handle, &overlap, &dwRead, TRUE);
		if (res && dwRead > 0)
		{
			data = new char[outputBufferSize];
			memcpy(data, &outputBuffer[1], (size_t)(outputBufferSize > dwRead ? dwRead : outputBufferSize));
		}

		CloseHandle(ev);
	}

	return data;
}

static int readReportData(HANDLE& handle, HidDevice hid_device) {
	unsigned long seconds = waitTime;
	unsigned long beginTime;
	static std::vector<char*> dataValues;

	//start clock
	beginTime = clock();
	while (true)
	{
		//compute elapsed time
		unsigned long elapsedTime = ((unsigned long)clock() - beginTime) / CLOCKS_PER_SEC;
		if (elapsedTime > seconds)
		{
			break;
		}

		char* data = readData(handle, hid_device);
		if (data == NULL)
		{ //no more data
			break;
		}
		else
		{ //got data
			dataValues.push_back(data);
			if (dataValues.size() == 20)
			{
				break;
			}
		}
	}

	bool lineBreak = dataValues.size() > 1;
	for (char* dataValue : dataValues)
	{
		lineBreak ? std::cout << dataValue << std::endl : std::cout << dataValue;
	}

	return 0;
}

static int setreport_execution_block(std::wstring device, bool* foundDevice)
{
	*foundDevice = true;
	HidDevice hid_device(device);
	HANDLE handle = hid_device.get_device_handle(device);

	if (handle != 0 && handle != INVALID_HANDLE_VALUE)
	{
        HIDP_CAPS caps = hid_device.get_device_capabilities(handle);

		for (std::string command : commands)
		{
			const size_t reportDataSize = command.size() + 1; //+1 - termination character (\0)
			const size_t sendingReportSize = (reportType == L"feature" ? caps.FeatureReportByteLength : caps.OutputReportByteLength);
			if (reportDataSize > sendingReportSize)
			{
				std::cout << "warn: report larger than maximum size" << std::endl;
			}

			//prepare report data	
			char* reportData = new char[reportDataSize];

			// copy report data to char array
			memcpy(reportData, command.c_str(), command.size());
			reportData[command.size()] = 0;

			//prepare report
			char* reportBuffer = new char[sendingReportSize];

			// set report id
			reportBuffer[0] = reportId;

			// copy data
			strncpy(&reportBuffer[1], reportData, sendingReportSize);
			if (reportType == L"output")
			{
				if (HidD_SetOutputReport(handle, reportBuffer, sendingReportSize))
				{
					readReportData(handle, hid_device); //read report
				}
				else
				{
					std::cout << "setreport: send error (code: " << GetLastError() << ")" << std::endl;
				}
			}
			else if (reportType == L"feature")
			{
				if (!HidD_SetFeature(handle, reportBuffer, sendingReportSize))
				{
					std::cout << "setreport: send error (code: " << GetLastError() << ")" << std::endl;
				}
			}
		}

		CloseHandle(handle);
	}

	return 0;
}

int cmd_setreport(int argc, const char **argv)
{
	if (_isatty(_fileno(stdin)))
	{ //stdin is a terminal
		//lookup -d
		for (int i = 1; i < argc; i++)
		{
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
					std::string data(str);
					if (data.length() % 2 == 0 && data.substr(0, 2) == "0x")
					{
						commands.push_back(dataFromHexString(data));
					}
					else
					{
						commands.push_back(data);
					}
				}

				break;
			}
		}
	}
	else
	{ //stdin is a file or a pipe
		std::string inputStr;
		std::cin >> inputStr;
		std::string input(inputStr.begin(), inputStr.end());
		if (!input.empty())
		{
			commands.push_back(input);
		}
	}

	if (commands.size() == 0)
	{
		return 0;
	}

	std::wstring _waitTime = get_w_param(argc, argv, "-w");
	if (!_waitTime.empty())
	{
		int wt = std::stoi(_waitTime);
		waitTime = wt > 0 ? wt : 0;
	}

	std::wstring _reportType = get_w_param(argc, argv, "-t");
	if (!_reportType.empty())
	{
		reportType = _reportType;
	}

	std::wstring _reportId = get_w_param(argc, argv, "-i");
	if (!_reportId.empty())
	{
		reportId = std::stoul(_reportId, nullptr, 16);
	}

	HidManager hid_manager;
	return hid_manager.process_devices(argc, argv, &setreport_execution_block);
}