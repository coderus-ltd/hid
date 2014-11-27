#include<string>
#include<windows.h>

#ifndef commandparser_h
#define commandparser_h

std::wstring get_w_param(int argc, const char **argv, std::string paramType);
std::string get_param(int argc, const char **argv, std::string paramType);

#endif