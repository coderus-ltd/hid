//
//  info.m
//  hid
//
//  Created by Eddie Woodley on 15/10/2014.
//  Copyright (c) 2014 Coderus. All rights reserved.
//

#include "hidmanager.h"

static char * CFStringCopyUTF8String(CFStringRef aString) {
  if (aString == NULL) {
    return NULL;
  }
  
  CFIndex length = CFStringGetLength(aString);
  CFIndex maxSize =
  CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);
  char *buffer = (char *)malloc(maxSize);
  if (CFStringGetCString(aString, buffer, maxSize, kCFStringEncodingUTF8))
  {
    return buffer;
  }
  return NULL;
}

int cmd_info(int argc, const char **argv)
{
  return process_devices(argc, argv, ^int(IOHIDDeviceRef pDeviceRef, BOOL *pStop)
  {
    
    // Output general info on the device
    fprintf(stdout,
            "================================================================\n"
            "%s via %s\n"
            "================================================================\n"
            "%s: %04x\n"
            "%s: %d\n"
            "%s: %04x\n"
            "%s: %x\n"
            "%s: %s\n"
            "%s: %s\n"
            "%s: %s\n"
            "%s: %d\n"
            "%s: %08x\n"
            "%s: %x\n"
            "%s: %d\n"
            "%s: %d\n"
            "%s: %d\n"
            "%s: %d\n"
            "%s: %d\n"
            "\n",
            CFStringCopyUTF8String(get_product_string(pDeviceRef)),
            CFStringCopyUTF8String(get_transport(pDeviceRef)),
            kIOHIDVendorIDKey, get_int_property(pDeviceRef, CFSTR(kIOHIDVendorIDKey)),
            kIOHIDVendorIDSourceKey, get_int_property(pDeviceRef, CFSTR(kIOHIDVendorIDSourceKey)),
            kIOHIDProductIDKey, get_int_property(pDeviceRef, CFSTR(kIOHIDProductIDKey)),
            kIOHIDVersionNumberKey, get_int_property(pDeviceRef, CFSTR(kIOHIDVersionNumberKey)),
            kIOHIDManufacturerKey, CFStringCopyUTF8String(get_string_property(pDeviceRef, CFSTR(kIOHIDManufacturerKey))),
            kIOHIDProductKey, CFStringCopyUTF8String(get_string_property(pDeviceRef, CFSTR(kIOHIDProductKey))),
            kIOHIDSerialNumberKey, CFStringCopyUTF8String(get_string_property(pDeviceRef, CFSTR(kIOHIDSerialNumberKey))),
            kIOHIDCountryCodeKey, get_int_property(pDeviceRef, CFSTR(kIOHIDCountryCodeKey)),
            kIOHIDLocationIDKey, get_int_property(pDeviceRef, CFSTR(kIOHIDLocationIDKey)),
            kIOHIDPrimaryUsagePageKey, get_int_property(pDeviceRef, CFSTR(kIOHIDPrimaryUsagePageKey)),
            kIOHIDPrimaryUsageKey, get_int_property(pDeviceRef, CFSTR(kIOHIDPrimaryUsageKey)),
            kIOHIDMaxInputReportSizeKey, get_int_property(pDeviceRef, CFSTR(kIOHIDMaxInputReportSizeKey)),
            kIOHIDMaxOutputReportSizeKey, get_int_property(pDeviceRef, CFSTR(kIOHIDMaxOutputReportSizeKey)),
            kIOHIDMaxFeatureReportSizeKey, get_int_property(pDeviceRef, CFSTR(kIOHIDMaxFeatureReportSizeKey)),
            kIOHIDReportIntervalKey, get_int_property(pDeviceRef, CFSTR(kIOHIDReportIntervalKey))
            );
    
    return 0;
  });
  
}