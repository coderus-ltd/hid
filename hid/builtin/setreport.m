//
//  setreport.m
//  hid
//
//  Created by Eddie Woodley on 14/10/2014.
//  Copyright (c) 2014 Coderus. All rights reserved.
//
#import "hidmanager.h"

#include "builtin.h"

int cmd_setreport(int argc, const char **argv)
{
  
  // Do any prep
  
  // Work with the devices
  return process_devices(argc, argv, ^int(int pNumDevices, IOHIDDeviceRef pDeviceRef, BOOL *pStop) {
    
    return 0;
    
  });
  
}