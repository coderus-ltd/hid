//
//  setreport.m
//  hid
//
//  Created by Eddie Woodley on 14/10/2014.
//  Copyright (c) 2014 Coderus. All rights reserved.
//
#import "hidmanager.h"

#include "builtin.h"

static NSData* dataFromHexString(NSString *string)
{
  if([string rangeOfString:@"0x"].location == 0)
  {
    string = [string substringFromIndex:2];
  }
  const char *chars = [string UTF8String];
  int i = 0, len = (int)string.length;
  
  NSMutableData *data = [NSMutableData dataWithCapacity:len / 2];
  char byteChars[3] = {'\0','\0','\0'};
  unsigned long wholeByte;
  
  while (i < len) {
    byteChars[0] = chars[i++];
    byteChars[1] = chars[i++];
    wholeByte = strtoul(byteChars, NULL, 16);
    [data appendBytes:&wholeByte length:1];
  }
  
  return data;
}

static void theIOHIDReportCallback (int*                    counter,
                             IOReturn                result,
                             void *                  sender,
                             IOHIDReportType         type,
                             uint32_t                reportID,
                             uint8_t *               report,
                             CFIndex                 reportLength)
{
  // the device responded, so we can stop the runloop
  if ( type == kIOHIDReportTypeInput )
  {
    unsigned long len = strlen((const char*)report);
    NSMutableString *value = [NSMutableString stringWithCapacity:len];
    
    // skip first byte
    for (NSUInteger i = 1; i < len; ++i){
      [value appendFormat:@"%c", report[i]];
    }
    
    NSLog(@"%@", value);
    
    *counter = *counter +1;
  }
}

int cmd_setreport(int argc, const char **argv)
{
  
  // Do any prep
  
  // Work with the devices
  return process_devices(argc, argv, ^int(IOHIDDeviceRef pDeviceRef, BOOL *pStop) {
    
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
      [defaults registerDefaults:@{ @"i": @"0", @"w": @"0", @"t": @"output" }];
      
      // Parse the report data
      // TODO: Allow reading from STDIN
      NSMutableArray *setReportCommands = [NSMutableArray arrayWithCapacity:1];
      
      if(!isatty(fileno(stdin)))
      {
        // STDIN is not a tty, read from STDIN
        NSFileHandle *input = [NSFileHandle fileHandleWithStandardInput];
        NSData *inputData = [NSData dataWithData:[input readDataToEndOfFile]];
        if(inputData)
        {
          [setReportCommands addObject:inputData];
        }
      }
      else
      {
        // STDIN is a tty, read from CLI
        for (int i = 1; i < argc; i++)
        {
          if(strcmp(argv[i], "-d" ) == 0 )
          {
            for (int j = i+1; j < argc; j++)
            {
              if ( argv[j][0] != '-')
              {
                NSString *dataArg = [NSString stringWithCString:argv[j] encoding:NSUTF8StringEncoding];
                if([dataArg length] % 2 == 0 && [dataArg rangeOfString:@"0x"].location == 0)
                {
                  // Hex value
                  [setReportCommands addObject:dataFromHexString(dataArg)];
                }
                else
                {
                  // String value
                  [setReportCommands addObject:[dataArg dataUsingEncoding:NSUTF8StringEncoding]];
                }
              }
              else
              {
                // not what we expected quit
                break;
              }
            }
            break;
          }
        }

      }
      
      if(![setReportCommands count])
      {
        fprintf(stderr, "Error: Please specify report data. See 'hid setreport --help'.\n");
        return 255;
      }
      
      // Determine the type of report
      IOHIDReportType type = kIOHIDReportTypeOutput;
      NSString *typeArg = [defaults stringForKey:@"t"];
      if([typeArg isEqualToString:@"feature"])
      {
        type = kIOHIDReportTypeFeature;
      }
      
      
      // If we are waiting for an input report (w option) then register
      // callback and schedule runloop
      NSInteger waitTime = [defaults integerForKey:@"w"];
      int  nosReportsReceived = 0;
      if(waitTime)
      {
        const unsigned short inputReportSize = get_int_property(pDeviceRef, CFSTR(kIOHIDMaxInputReportSizeKey));
        char *inputBuffer =  calloc(inputReportSize, sizeof(char));
        
        // register our report callback
        IOHIDDeviceRegisterInputReportCallback(pDeviceRef, (uint8_t *)inputBuffer, inputReportSize, (IOHIDReportCallback)theIOHIDReportCallback, &nosReportsReceived);
        IOHIDDeviceScheduleWithRunLoop(pDeviceRef, CFRunLoopGetCurrent( ), kCFRunLoopDefaultMode );
      }
      
      // Prepare the report
      #define cReportIDPad 1 // first byte is used for report ID
      for(NSData *data in setReportCommands)
      {
        const unsigned short cOutputReportSize = get_int_property(pDeviceRef, CFSTR(kIOHIDMaxOutputReportSizeKey));
        NSUInteger reportSize = [data length];
        const char* reportData = [data bytes];
        
        size_t sendingReportSize = MIN(reportSize, cOutputReportSize - cReportIDPad);
        char *reportBuffer =  calloc(cOutputReportSize,sizeof(char));
        if(sendingReportSize < reportSize)
        {
          // TODO: verbose mode
          NSLog(@"%@", @"warn: report larger than maximum size");
        }
        
        // get the report id
        unsigned short reportId = (unsigned short)[defaults integerForKey:@"i"];
        reportBuffer[0] = reportId;
        
        // Copy data into the buffer
        strncpy(&reportBuffer[1], reportData, sendingReportSize);
        
        // Send the report
        res = IOHIDDeviceSetReport (pDeviceRef, type, reportBuffer[0], (const unsigned char *)reportBuffer, cOutputReportSize );
        if ( res == kIOReturnSuccess )
        {
          // if waiting, then wait
          if ( waitTime )
          {
            SInt32 runLoopRes = CFRunLoopRunInMode(kCFRunLoopDefaultMode, waitTime, false);
            if ( runLoopRes == kCFRunLoopRunTimedOut && nosReportsReceived == 0 )
            {
              fprintf(stderr, "error: no input reports recieved after %lds.\n", (long)waitTime);
            }
          }
          else
          {
            // TODO: verbose mode
            fprintf(stdout, "report sent\n");
          }
        }
        else
        {
          fprintf(stderr, "error: report not sent (code %d)", res);
          return 1;
        }
      }
      return 0;
    }
  });
  
}