//
//  main.m
//  HID
//
//  Created by Mark Thomas on 03/06/2014.
//  Copyright (c) Coderus Ltd. All rights reserved.
//

#import <Foundation/Foundation.h>
#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/hid/IOHIDKeys.h>
#import <limits.h>

int usage()
{
  NSString *name = [[NSProcessInfo.processInfo.arguments objectAtIndex:0] lastPathComponent];
  
  NSLog(@"\n");
  NSLog(@"Usage: %@ -h", name);
  NSLog(@"       %@ list", name);
  NSLog(@"       %@ -l", name);
  NSLog(@"       %@ -v <vendorId> -p <productId> -setReport \"exampleCommand1\" [\"exampleCommand2\" ...] [-t <seconds_to_wait_for_response>] [-n <iterations>]", name);
  NSLog(@"       %@ -v <vendorId> -p <productId> -isConnected", name);
  NSLog(@"\n");
  NSLog(@"-h                                  Help");
  NSLog(@"list                                Lists all HID devices");
  NSLog(@"-v <vendorId>                       Connect to a device with name");
  NSLog(@"-p <productId>                      Connect to a device with name");
  NSLog(@"-t <seconds_to_wait_for_response>   Seconds to wait for the device to respond to the command");
  NSLog(@"-n <iterations>                     Number of times to attempt the command");
  NSLog(@"-setReport  \"exampleCommand\"      Quoted list of commands to send to the device");
  NSLog(@"-isAtached < YES | NO >             Check to test whether a HID device is attached or not.");
  
  return TRUE;
}

static int32_t get_int_property(IOHIDDeviceRef device, CFStringRef key)
{
	CFTypeRef ref;
	int32_t value;
  
	ref = IOHIDDeviceGetProperty(device, key);
  
	if (ref) {
		if (CFGetTypeID(ref) == CFNumberGetTypeID()) {
			CFNumberGetValue((CFNumberRef) ref, kCFNumberSInt32Type, &value);
			return value;
		}
	}
	return 0;
}

static CFStringRef get_string_property(IOHIDDeviceRef device, CFStringRef prop )
{
  CFTypeRef ref;
  
	ref = IOHIDDeviceGetProperty(device, prop);
  
	if (ref) {
		if (CFGetTypeID(ref) == CFStringGetTypeID()) {
			return ref;
		}
	}
	return 0;
}

// helpers
static unsigned short get_vendor_id(IOHIDDeviceRef device)
{
	return get_int_property(device, CFSTR(kIOHIDVendorIDKey));
}

static unsigned short get_product_id(IOHIDDeviceRef device)
{
	return get_int_property(device, CFSTR(kIOHIDProductIDKey));
}

static CFStringRef get_product_string(IOHIDDeviceRef device)
{
	return get_string_property(device, CFSTR(kIOHIDProductKey));
}

static CFStringRef get_transport(IOHIDDeviceRef device)
{
	return get_string_property(device, CFSTR(kIOHIDTransportKey));
}


void theIOHIDReportCallback (int*                    counter,
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

typedef int (^hidItem)(IOHIDDeviceRef pDeviceRef, unsigned short pVendorID, unsigned short pProductID, BOOL *pStop);

int processHIDs(hidItem pItem)
{
  int result = 0 ;
  
  IOHIDManagerRef hid_mgr = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
  
  if (hid_mgr){
    IOHIDManagerSetDeviceMatching(hid_mgr, NULL);
    IOHIDManagerScheduleWithRunLoop(hid_mgr, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    
    // see how many devices we have
    CFSetRef device_set = IOHIDManagerCopyDevices(hid_mgr);
    
    /* Convert the list into a C array so we can iterate easily. */
    CFIndex num_devices = CFSetGetCount(device_set);
    
    if (num_devices)
    {
      BOOL shouldStop = FALSE;
      
      IOHIDDeviceRef *device_array = calloc(num_devices, sizeof(IOHIDDeviceRef));
      CFSetGetValues(device_set, (const void **) device_array);
      
      for (int i = 0; i < num_devices && !shouldStop ; i++)
      {
        IOHIDDeviceRef dev = device_array[i];
        
        if (dev) {
          unsigned short dev_vid = get_vendor_id(dev);
          unsigned short dev_pid = get_product_id(dev);
          
          result = pItem(dev, dev_vid, dev_pid, &shouldStop);
        }
        
      }
      free(device_array);
    }
    
    CFRelease(device_set);
  }
  return result;
}

int main(int argc, const char * argv[])
{
  int result;
  
  if ( argc == 1 )
  {
    result = usage();
  }
  else if ( argc == 2 )
  {
    if ( strcmp(argv[1],"-h") == 0 )
    {
      result = usage();
    }
    else if ( strcmp(argv[1],"list") == 0 )
    {
      result = processHIDs( ^(IOHIDDeviceRef pDeviceRef, unsigned short pVendorID, unsigned short pProductID, BOOL *pStop){
        
        CFStringRef prod = get_product_string(pDeviceRef);
        CFStringRef transport = get_transport(pDeviceRef);
        
        NSLog(@"0x%04x 0x%04x - %@ via %@",pVendorID, pProductID, prod, transport);
        
        return TRUE;
      });
    }
  }
  else
  {
    NSUserDefaults *standardDefaults = [NSUserDefaults standardUserDefaults];
    
    unsigned int vendorId = 0, productId = 0;
    
    NSString *vendorIdStr = [standardDefaults stringForKey:@"v"];
    if ( vendorIdStr )
    {
      NSScanner* scanner = [NSScanner scannerWithString:vendorIdStr];
      [scanner scanHexInt:&vendorId];
    }
    
    NSString *productIdStr = [standardDefaults stringForKey:@"p"];
    if ( productIdStr )
    {
      NSScanner* scanner = [NSScanner scannerWithString:productIdStr];
      [scanner scanHexInt:&productId];
    }
    
    if ( [standardDefaults stringForKey:@"setReport"] )
    { //
      //  This command sends setReport's.
      //
      NSMutableArray *setReportCommands = [NSMutableArray arrayWithCapacity:1];
      
      for (int i = 1; i < argc; i++)
      {
        if(strcmp( argv[i], "-setReport" ) == 0 )
        {
          for (int j = i+1; j < argc; j++)
          {
            if ( argv[j][0] != '-')
            {
              [setReportCommands addObject:[NSString stringWithCString:argv[j] encoding:NSUTF8StringEncoding]];
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
      
      if ( vendorId && productId && [setReportCommands count] > 0 )
      {
        // optional arguments
        [standardDefaults registerDefaults:@{ @"t": @2, @"n":@1 }];
        
        NSInteger waitTime = [standardDefaults integerForKey:@"t"];
        NSInteger numIterations = [standardDefaults integerForKey:@"n"];
        
        result = processHIDs( ^(IOHIDDeviceRef pDeviceRef, unsigned short pVendorID, unsigned short pProductID, BOOL *pStop){
          
          if ( vendorId == pVendorID && productId == pProductID)
          {
            // open the device
            IOReturn res = IOHIDDeviceOpen(pDeviceRef, kIOHIDOptionsTypeNone); // kIOHIDOptionsTypeSeizeDevice
            
            //error check
            if (res != kIOReturnSuccess)
            {
              switch (res) {
                case kIOReturnExclusiveAccess:
                  NSLog(@"kIOReturnExclusiveAccess: exclusive access and device already open. Restart device.");
                  break;
                case kIOReturnNotOpen:
                  NSLog(@"kIOReturnNotOpen: device not open. Restart device.");
                  break;
                  
                default:
                  NSLog(@"Ret code: %d", res);
                  break;
              }
            }
            else
            {
              const unsigned short outputReportSize = get_int_property(pDeviceRef, CFSTR(kIOHIDMaxOutputReportSizeKey));
              
              size_t bufferSize = outputReportSize;
              char *inputBuffer = malloc(bufferSize);
              int  nosReportsReceived;
              
              // register our report callback
              IOHIDDeviceRegisterInputReportCallback(pDeviceRef, (uint8_t *)inputBuffer,
                                                     bufferSize, (IOHIDReportCallback)theIOHIDReportCallback, &nosReportsReceived);
              
              // use this run loop
              IOHIDDeviceScheduleWithRunLoop(pDeviceRef, CFRunLoopGetCurrent( ), kCFRunLoopDefaultMode );
              
              for(NSInteger x=0; x < numIterations; ++x)
              {
                // loop through each command for each iteration
                for (NSString * setReportCommand in setReportCommands)
                {
                  size_t setReportSize = MIN([setReportCommand length], bufferSize-1);
                  
                  inputBuffer[0] = kIOHIDReportTypeOutput;
                  strncpy(&inputBuffer[1],[setReportCommand cStringUsingEncoding: NSASCIIStringEncoding], setReportSize );
                  
                  const uint8_t *data_to_send = (const unsigned char *)inputBuffer;
                  
                  NSLog(@"Cmd: %@", setReportCommand);
                  
                  // send command to device
                  nosReportsReceived = 0;
                  res = IOHIDDeviceSetReport (pDeviceRef, kIOHIDReportTypeOutput, inputBuffer[0], data_to_send, setReportSize+1);
                  
                  // run current thread runloop for waitTime
                  // setting returnAfterSourceHandled to true returns immediately
                  // it doesn't wait for the callback as expected
                  if ( res == kIOReturnSuccess )
                  {
                    if ( waitTime )
                    {
                      SInt32 runLoopRes = CFRunLoopRunInMode(kCFRunLoopDefaultMode, waitTime, false);
                      if ( runLoopRes == kCFRunLoopRunTimedOut && nosReportsReceived == 0 )
                      {
                        NSLog(@"No Input report recieved back.");
                      }
                    }
                  }
                  else
                  {
                    NSLog(@"Problem sending the Output report.");
                  }
                }
              }
              
              free(inputBuffer);
              res = IOHIDDeviceClose(pDeviceRef, kIOHIDOptionsTypeNone);
            }
            *pStop = TRUE;
            return TRUE;
          }
          
          return FALSE;
        });
        
        if ( !result )
        {
          NSLog(@"0x%04x 0x%04x - is not Connected",vendorId, productId);
        }
      }
      else
      { //
        // missing arguments
        //
        NSLog(@"Missing Params");
        usage();
        result = -1;
      }
    }
    else if ( [standardDefaults stringForKey:@"isAttached"] )
    { //
      //  This command just check to see if the device is present.
      //
      BOOL testingForAttached = [standardDefaults boolForKey:@"isAttached"];
      
      if ( vendorId && productId )
      {
        result = processHIDs( ^(IOHIDDeviceRef pDeviceRef, unsigned short pVendorID, unsigned short pProductID, BOOL *pStop){
          
          if ( pVendorID == vendorId && pProductID == productId)
          {
            *pStop = TRUE;
            return TRUE;
          }
          
          return FALSE;
        });
        
        
        if ( testingForAttached )
        {
          if ( result  )
          {
            NSLog(@"0x%04x 0x%04x - is attached",vendorId, productId);
            result = TRUE;
          }
          else
          {
            NSLog(@"0x%04x 0x%04x - error wasn't attached",vendorId, productId);
          }
        }
        else
        {
          if ( result  )
          {
            NSLog(@"0x%04x 0x%04x - error was still attached ",vendorId, productId);
            result = FALSE;
          }
          else
          {
            NSLog(@"0x%04x 0x%04x - is not attached",vendorId, productId);
            result = TRUE;
          }
        }
      }
      else
      { //
        // missing arguments
        //
        NSLog(@"Missing Params");
        usage();
        result = -1;
      }
    }
    else
    { //
      // bad arguments
      //
      NSLog(@"Invalid Params");
      usage();
      result = -1;
    }
  }
  
  return result;
}

