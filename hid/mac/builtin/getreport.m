//
//  getreport.m
//  hid
//
//  Created by Eddie Woodley on 15/10/2014.
//  Copyright (c) 2014 Coderus. All rights reserved.
//

#import "hidmanager.h"
#import "util.h"

int cmd_getreport(int argc, const char **argv)
{
  return process_devices(argc, argv, ^int(IOHIDDeviceRef pDeviceRef, BOOL *pStop){
    // For now, we wil always just use the first device
    *pStop = YES;
    
    // open the device
    IOReturn res = IOHIDDeviceOpen(pDeviceRef, kIOHIDOptionsTypeNone); // kIOHIDOptionsTypeSeizeDevice
    
    if (res != kIOReturnSuccess)
    {
      // Error opening device
      switch (res) {
        case kIOReturnExclusiveAccess:
          fprintf(stderr, "kIOReturnExclusiveAccess: Exclusive access and device already open. Restart device.\n");
          break;
        case kIOReturnNotOpen:
          fprintf(stderr, "kIOReturnNotOpen: Device not open. Restart device.\n");
          break;
        default:
          fprintf(stderr, "Error: Could not open device (code %d).\n", res);
          break;
      }
      return 1;
    }
    else
    {
      NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
      [defaults registerDefaults:@{ @"i": @"0", @"t": @"input" }];
      
      // Determine the type of report
      IOHIDReportType type = kIOHIDReportTypeInput;
      CFStringRef reportSizeKey = CFSTR(kIOHIDMaxInputReportSizeKey);
      NSString *typeArg = [defaults stringForKey:@"t"];
      if([typeArg isEqualToString:@"feature"])
      {
        type = kIOHIDReportTypeFeature;
        reportSizeKey = CFSTR(kIOHIDMaxFeatureReportSizeKey);
      }
      else if([typeArg isEqualToString:@"output"])
      {
        type = kIOHIDReportTypeOutput;
        reportSizeKey = CFSTR(kIOHIDMaxOutputReportSizeKey);
      }
      
      CFIndex inputReportSize = get_int_property(pDeviceRef, reportSizeKey);
      uint8_t *reportBuffer =  calloc(inputReportSize, sizeof(char));
      
      const unsigned short reportId = (unsigned short)[defaults integerForKey:@"i"];
      
      IOReturn getRes = IOHIDDeviceGetReport(pDeviceRef,
                                             kIOHIDReportTypeOutput,
                                             reportId,
                                             reportBuffer,
                                             &inputReportSize);
      
     
      if(getRes != kIOReturnSuccess)
      {
        fprintf(stderr, "error: could not get report (code %d).\n", getRes);
        return 1;
      }
      else
      {
        output_report(reportBuffer, (int)inputReportSize, argc, argv);
      }
      
    }
    return 0;
  });
}