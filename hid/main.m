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
    NSLog(@"\n");
    NSLog(@"-h                                  Help");
    NSLog(@"list                                Lists all paired devices");
    NSLog(@"-v <vendorId>                       Connect to a device with name");
    NSLog(@"-p <productId>                      Connect to a device with name");
    NSLog(@"-t <seconds_to_wait_for_response>   Seconds to wait for the device to respond to the command");
    NSLog(@"-n <iterations>                     Number of times to attempt the command");
    NSLog(@"-setReport  \"exampleCommand\"        Quoted list of commands to send to the device");
    
    return 0;
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


void theIOHIDReportCallback ( void *                 context,
                             IOReturn                result,
                             void *                  sender,
                             IOHIDReportType         type,
                             uint32_t                reportID,
                             uint8_t *               report,
                             CFIndex                 reportLength)
{
    // the device responded, so we can stop the runloop
    CFRunLoopStop(CFRunLoopGetCurrent());
    
    unsigned long len = strlen((const char*)report);
    NSMutableString *value = [NSMutableString stringWithCapacity:len];
    
    // skip first byte
    for (NSUInteger i = 1; i < len; ++i){
        [value appendFormat:@"%c", report[i]];
    }
    
    NSLog(@"%@", value);
}

int main(int argc, const char * argv[])
{
    NSInteger waitTime = 0;
    NSInteger numIterations = 0;
    
    NSMutableArray *setReportCommands = [NSMutableArray arrayWithCapacity:1];
    
    unsigned int vendorId = 0;
    unsigned int productId = 0;
    
    BOOL showList = NO;
    
    @autoreleasepool
    {
        // handle -h|--help or list
        if(argc == 2){
            
            NSString *arg = [[NSString stringWithCString:argv[1] encoding:NSUTF8StringEncoding] lowercaseString];
            
            if([arg isEqualToString:@"-h"]){
                usage();
                return kIOReturnNotOpen;
            }
            // this means they just passed 'list' in
            if([arg isEqualToString:@"list"]){
                showList = YES;
            }
        }
        
        if (showList == NO) {
            // 3 arguments with params requried
            if(argc < 7) {
                NSLog(@"Invalid Params");
                usage();
                return -1;
            }
            
            // figure out where the setReport params start and finish
            int setReportIndex = argc; // start at the end
            int nextParamIndex = 0;
            
            for (int i = 1; i < argc; i++) {
                NSString *arg = [NSString stringWithCString:argv[i] encoding:NSUTF8StringEncoding];
                
                if([arg isEqualToString:@"-setReport"]){
                    setReportIndex = i;
                }
                
                if([arg hasPrefix:@"-"] && i > setReportIndex){
                    nextParamIndex = i;
                    break;
                }
            }
            
            // if we got to the end, set index to the last arg index
            if(nextParamIndex == 0){
                nextParamIndex = argc;
            }
            
            for (int i = 1; i < argc; i++) {
                
                if(i > setReportIndex && i < nextParamIndex){
                    NSString *arg = [NSString stringWithCString:argv[i] encoding:NSUTF8StringEncoding];
                    [setReportCommands addObject:arg];
                }
            }
            
            //get at the other args
            NSUserDefaults *standardDefaults = [NSUserDefaults standardUserDefaults];
            
            // vendorId and productId are passed in as hex strings
            // we need to get the string then scan to an int
            NSString *vendorIdStr = [standardDefaults stringForKey:@"v"];
            NSString *productIdStr = [standardDefaults stringForKey:@"p"];
            
            NSScanner* scanner = [NSScanner scannerWithString:vendorIdStr];
            [scanner scanHexInt:&vendorId];
            
            scanner = [NSScanner scannerWithString:productIdStr];
            [scanner scanHexInt:&productId];
            
            waitTime = [standardDefaults integerForKey:@"t"];
            numIterations = [standardDefaults integerForKey:@"n"];
            
            // default wait and iterations
            if(waitTime == 0){
                waitTime = 2;
            }
            if(numIterations == 0){
                numIterations = 1;
            }
            
            // if one of the integer keys fails to be retrieved, it will be zero
            // if they overflow, they will be -1
            // the setReportCommands array would be empty if not parsed
            if(showList == NO &&( (int)productId <= 0 || (int)vendorId <= 0 || [setReportCommands count] == 0)){
                NSLog(@"Invalid parameter");
                usage();
                return -1;
            }
        }
        
        IOHIDManagerRef hid_mgr = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
        
        if (hid_mgr){
            IOHIDManagerSetDeviceMatching(hid_mgr, NULL);
            IOHIDManagerScheduleWithRunLoop(hid_mgr, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
            
            // see how many devices we have
            CFSetRef device_set = IOHIDManagerCopyDevices(hid_mgr);
            
            /* Convert the list into a C array so we can iterate easily. */
            CFIndex num_devices = CFSetGetCount(device_set);
            
            if (num_devices){
                IOHIDDeviceRef *device_array = calloc(num_devices, sizeof(IOHIDDeviceRef));
                CFSetGetValues(device_set, (const void **) device_array);
                
                for (int i = 0; i < num_devices; i++)
                {
                    unsigned short dev_vid;
                    unsigned short dev_pid;
                    
                    IOHIDDeviceRef dev = device_array[i];
                    
                    if (!dev) {
                        continue;
                    }
                    
                    // get the vendor/product identifiers
                    dev_vid = get_vendor_id(dev);
                    dev_pid = get_product_id(dev);
                    
                    if (showList == YES) {
                        
                        CFStringRef prod = get_product_string(dev);
                        CFStringRef transport = get_transport(dev);
                        
                        NSLog(@"0x%04x 0x%04x - %@ via %@",dev_vid, dev_pid, prod, transport);
                    }
                    else{
                        // this is a device matching our program parameters
                        if (dev_vid == vendorId && dev_pid == productId ){
                            
                            // open the device
                            IOReturn res = IOHIDDeviceOpen(dev, kIOHIDOptionsTypeNone); //  ... kIOHIDOptionsTypeSeizeDevice
                            
                            //error check
                            if (res != kIOReturnSuccess){
                                switch (res) {
                                    case kIOReturnExclusiveAccess:
                                        NSLog(@"kIOReturnExclusiveAccess: exclusive access and device already open. Restart device.");
                                        return res;
                                        break;
                                    case kIOReturnNotOpen:
                                        NSLog(@"kIOReturnNotOpen: device not open. Restart device.");
                                        return res;
                                        break;
                                        
                                    default:
                                        NSLog(@"Ret code: %d", res);
                                        break;
                                }
                            }
                            
                            const unsigned short outputReportSize = get_int_property(dev, CFSTR(kIOHIDMaxOutputReportSizeKey));
                            
                            size_t bufferSize = outputReportSize;
                            char *inputBuffer = malloc(bufferSize);
                            
                            // register our report callback
                            IOHIDDeviceRegisterInputReportCallback(dev, (uint8_t *)inputBuffer,
                                                                   bufferSize, theIOHIDReportCallback, NULL);
                            
                            
                            for(NSInteger x=0; x < numIterations; ++x){
                            
                                // loop through each command for each iteration
                                for (NSString * setReportCommand in setReportCommands){
                                    
                                    char theMsg[33];
                                    theMsg[0] = kIOHIDReportTypeOutput;
                                    strcpy(&theMsg[1],[setReportCommand cStringUsingEncoding: NSASCIIStringEncoding]);
                                    
                                    const uint8_t *data_to_send = (const unsigned char *)&theMsg[0];
                                    size_t length_to_send = outputReportSize;
                                    
                                    NSLog(@"Cmd: %@", setReportCommand);
                                    
                                    
                                    // use this run loop
                                    IOHIDDeviceScheduleWithRunLoop(dev, CFRunLoopGetCurrent( ), kCFRunLoopDefaultMode );
                                    
                                    // send command to device
                                    res = IOHIDDeviceSetReport (dev, kIOHIDReportTypeOutput, theMsg[0], data_to_send, length_to_send );
                                    IOHIDDeviceSetProperty(dev, CFSTR(kIOHIDResetKey), @1 );
                                    
                                    if ( res != kIOReturnSuccess )
                                    {
                                        NSLog(@"write result 0x%x", res);
                                    }
                                    
                                    // run current thread runloop for waitTime
                                    // setting returnAfterSourceHandled to true returns immediately
                                    // it doesn't wait for the callback as expected
                                    SInt32 res = CFRunLoopRunInMode(kCFRunLoopDefaultMode, waitTime, false);
                                    if ( res == kCFRunLoopRunTimedOut )
                                    {
                                      NSLog(@"Command timed out");
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    return 0;
}

