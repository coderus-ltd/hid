//
//  list.m
//  hid
//
//  Created by Eddie Woodley on 14/10/2014.
//  Copyright (c) 2014 Coderus. All rights reserved.
//
#import "hidmanager.h"

#include "builtin.h"

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

int cmd_list(int argc, const char **argv)
{
  
  fprintf(stdout, "LID     \tVID \tPID \tDescription\n");
  return process_devices(argc, argv, ^int(IOHIDDeviceRef pDeviceRef, BOOL *pStop)
  {
    fprintf(stdout,"%08x\t%04x\t%04x\t%s via %s\n",
            get_location_id(pDeviceRef),
            get_vendor_id(pDeviceRef),
            get_product_id(pDeviceRef),
            CFStringCopyUTF8String(get_product_string(pDeviceRef)),
            CFStringCopyUTF8String(get_transport(pDeviceRef)));
    return 0;
  });
  
}