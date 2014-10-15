//
//  hidmanager.h
//  hid
//
//  Created by Eddie Woodley on 14/10/2014.
//  Copyright (c) 2014 Coderus. All rights reserved.
//
#import <IOKit/hid/IOHIDManager.h>

#ifndef hid_hidmanager_h
#define hid_hidmanager_h

/// Block that is called for every HID device matching the criteria provided to the command.
/// If pStop is set to true, the loop will exit early.
typedef int (^ProcessDeviceBlock)(int pNumDevices, IOHIDDeviceRef pDeviceRef, BOOL *pStop);

int32_t get_int_property(IOHIDDeviceRef device, CFStringRef key);
CFStringRef get_string_property(IOHIDDeviceRef device, CFStringRef prop);
unsigned short get_vendor_id(IOHIDDeviceRef device);
unsigned short get_product_id(IOHIDDeviceRef device);
int32_t get_location_id(IOHIDDeviceRef device);
CFStringRef get_product_string(IOHIDDeviceRef device);
CFStringRef get_transport(IOHIDDeviceRef device);

int process_devices(int argc, const char **argv, ProcessDeviceBlock block);

#endif
