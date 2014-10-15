//
//  attached.m
//  hid
//
//  Created by Eddie Woodley on 15/10/2014.
//  Copyright (c) 2014 Coderus. All rights reserved.
//
#include <stdio.h>
#import "hidmanager.h"

int cmd_attached(int argc, const char **argv)
{
   __block BOOL foundDevice = NO;
  int res = process_devices(argc, argv, ^int(int pNumDevices, IOHIDDeviceRef pDeviceRef, BOOL *pStop)
  {
    *pStop = YES;
    foundDevice = YES;
    return 0;
  });
  
  if(foundDevice)
  {
    // TODO: Verbose mode
    fprintf(stdout, "1\n");
    return res;
  }
  else
  {
    // TODO: Verbose mode
    fprintf(stdout, "0\n");
    return res;
  }
  
}