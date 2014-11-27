#include<string>
#include<windows.h>

#include "commandparser.h"

// Change a string to a wide string
static std::wstring string_to_wide_string(const std::string& str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);

    return wstrTo;
}

// Get the data for the paramType
std::wstring get_w_param(int argc, const char **argv, std::string paramType)
{
    const char* paramData = '\0';

    for (int i = 0; i < argc; i++)
    {
        if (i + 1 != argc)
        {
            if (argv[i] == paramType)
            {
                paramData = argv[i + 1];
                break;
            }
        }
    }

    if (paramData != '\0')
    {
        std::string ret(paramData);
        return string_to_wide_string(ret);
    }
    else
    {
        return L"";
    }
}

// Get the data for the paramType
std::string get_param(int argc, const char **argv, std::string paramType)
{
    const char* paramData = '\0';

    for (int i = 0; i < argc; i++)
    {
        if (i + 1 != argc)
        {
            if (argv[i] == paramType)
            {
                paramData = argv[i + 1];
                break;
            }
        }
    }

    if (paramData != '\0')
    {
        std::string ret(paramData);
        return ret;
    }
    else
    {
        return "";
    }
}