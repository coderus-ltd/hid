@echo off
for /f "delims=" %%a in ('git describe --tags --always') do @set _GIT_DESCRIBE=%%a
for /f "delims=" %%b in ('echo %date:~10,4%-%date:~7,2%-%date:~4,2% %time:~0,8%') do @set _DATE=%%b
echo #ifndef VERSION > ../common/version.h
echo #define VERSION "hid version %_GIT_DESCRIBE%\nBuilt %_DATE%" >> ../common/version.h
echo #endif VERSION >> ../common/version.h
