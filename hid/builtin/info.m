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
    
    // Output general info
    fprintf(stdout,
            "================================================================\n"
            "%s via %s\n"
            "================================================================\n",
            CFStringCopyUTF8String(get_product_string(pDeviceRef)),
            CFStringCopyUTF8String(get_transport(pDeviceRef)));
    
    CFArrayRef elements =IOHIDDeviceCopyMatchingElements(pDeviceRef,
                                                         NULL,
                                                         0);
    CFIndex num_elements = CFArrayGetCount(elements);
    IOHIDElementRef *elements_array = calloc(num_elements, sizeof(IOHIDElementRef));
    CFArrayGetValues(elements, CFRangeMake(0, num_elements), (const void **)elements_array);
    
    for(int i = 0; i < num_elements; i++)
    {
      IOHIDElementRef e = elements_array[i];
      fprintf(stdout,
              "Element %s\n"
              "\t%s: %d\n"
              "\t%s: %d\n"
              "\t%s: %d\n"
              "\t%s: %d\n"
              "\t%s: %d\n",
              CFStringCopyUTF8String(IOHIDElementGetName(e)),
              "Usage Page", IOHIDElementGetUsagePage(e),
              "Usage", IOHIDElementGetUsage(e),
              "Report ID", IOHIDElementGetReportID(e),
              "Report Size", IOHIDElementGetReportSize(e),
              "Report Count", IOHIDElementGetReportCount(e)
              );
    }
    
    fprintf(stdout, "\n");
    
    return 0;
  });
  
}