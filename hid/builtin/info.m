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
  return process_devices(argc, argv, ^int(int pNumDevices, IOHIDDeviceRef pDeviceRef, BOOL *pStop)
  {
    
    // Output general info
    fprintf(stdout, "================================================================\n");
    fprintf(stdout,"%s via %s\n",
            CFStringCopyUTF8String(get_product_string(pDeviceRef)),
            CFStringCopyUTF8String(get_transport(pDeviceRef)));
    fprintf(stdout, "================================================================\n");
    CFArrayRef elements =IOHIDDeviceCopyMatchingElements(pDeviceRef,
                                                         NULL,
                                                         0);
    CFIndex num_elements = CFArrayGetCount(elements);
    IOHIDElementRef *elements_array = calloc(num_elements, sizeof(IOHIDElementRef));
    CFArrayGetValues(elements, CFRangeMake(0, num_elements), (const void **)elements_array);
    
    for(int i = 0; i < num_elements; i++)
    {
      IOHIDElementRef e = elements_array[i];
      fprintf(stdout, "Element %s\n", CFStringCopyUTF8String(IOHIDElementGetName(e)));
      fprintf(stdout, "\t%s: %d\n", "Usage Page", IOHIDElementGetUsagePage(e));
      fprintf(stdout, "\t%s: %d\n", "Usage", IOHIDElementGetUsage(e));
      fprintf(stdout, "\t%s: %d\n", "Report ID", IOHIDElementGetReportID(e));
      fprintf(stdout, "\t%s: %d\n", "Report Size", IOHIDElementGetReportSize(e));
      fprintf(stdout, "\t%s: %d\n", "Report Count", IOHIDElementGetReportCount(e));
    }
    
    fprintf(stdout, "\n");
    
    return 0;
  });
  
}