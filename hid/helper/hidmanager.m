//
//  hidmanager.m
//  hid
//
//  Created by Eddie Woodley on 14/10/2014.
//  Copyright (c) 2014 Coderus. All rights reserved.
//
#import <IOKit/hid/IOHIDKeys.h>

#import "hidmanager.h"

int32_t get_int_property(IOHIDDeviceRef device, CFStringRef key)
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

CFStringRef get_string_property(IOHIDDeviceRef device, CFStringRef prop )
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

int32_t get_int_element_property(IOHIDElementRef element, CFStringRef key)
{
  CFTypeRef ref;
  int32_t value;
  
  ref = IOHIDElementGetProperty(element, key);
  
  if (ref) {
    if (CFGetTypeID(ref) == CFNumberGetTypeID()) {
      CFNumberGetValue((CFNumberRef) ref, kCFNumberSInt32Type, &value);
      return value;
    }
  }
  return 0;
}

CFStringRef get_string_element_property(IOHIDElementRef element, CFStringRef prop )
{
  CFTypeRef ref;
  
  ref = IOHIDElementGetProperty(element, prop);
  
  if (ref) {
    if (CFGetTypeID(ref) == CFStringGetTypeID()) {
      return ref;
    }
  }
  return 0;
}

int32_t get_location_id(IOHIDDeviceRef device)
{
  return get_int_property(device, CFSTR(kIOHIDLocationIDKey));
}

unsigned short get_vendor_id(IOHIDDeviceRef device)
{
  return get_int_property(device, CFSTR(kIOHIDVendorIDKey));
}

unsigned short get_product_id(IOHIDDeviceRef device)
{
  return get_int_property(device, CFSTR(kIOHIDProductIDKey));
}

CFStringRef get_product_string(IOHIDDeviceRef device)
{
  return get_string_property(device, CFSTR(kIOHIDProductKey));
}

CFStringRef get_transport(IOHIDDeviceRef device)
{
  return get_string_property(device, CFSTR(kIOHIDTransportKey));
}

static CFMutableDictionaryRef create_matching_dictionary(UInt32 vendorId, UInt32 productId)
{
  CFMutableDictionaryRef result = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  if(result)
  {
    if(vendorId)
    {
      CFNumberRef vendorCFNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &vendorId);
      if (vendorCFNumberRef) {
        CFDictionarySetValue(result, CFSTR(kIOHIDVendorIDKey), vendorCFNumberRef);
        CFRelease(vendorCFNumberRef);
      }
    }
    if(productId)
    {
      CFNumberRef productCFNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &productId);
      if (productCFNumberRef) {
        CFDictionarySetValue(result, CFSTR(kIOHIDProductIDKey), productCFNumberRef);
        CFRelease(productCFNumberRef);
      }
    }
  }
  return result;
}

int process_devices(int argc, const char **argv, ProcessDeviceBlock block)
{
  int result = 0;
  
  // Register default options
  NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
  [defaults registerDefaults:@{ @"v": @"0", @"p": @"0", @"l": @"0" }];
  
  IOHIDManagerRef hid_mgr = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
  
  if (!hid_mgr){
    // Could not get a HID manager, return error.
    result = 1;
  } else {
    
    // Parse any device-matching options from the command line
    unsigned int vendorId = 0, productId = 0, locationId = 0;
    NSString *vendorIdStr = [defaults stringForKey:@"v"];
    NSString *productIdStr = [defaults stringForKey:@"p"];
    NSString *locationIdStr = [defaults stringForKey:@"l"];
    if ( vendorIdStr )
    {
      NSScanner* scanner = [NSScanner scannerWithString:vendorIdStr];
      [scanner scanHexInt:&vendorId];
    }
    if ( productIdStr )
    {
      NSScanner* scanner = [NSScanner scannerWithString:productIdStr];
      [scanner scanHexInt:&productId];
    }
    if ( locationIdStr )
    {
      NSScanner* scanner = [NSScanner scannerWithString:locationIdStr];
      [scanner scanHexInt:&locationId];
    }
    CFMutableDictionaryRef matchingDictionary = create_matching_dictionary(vendorId, productId);
    
    
    IOHIDManagerSetDeviceMatching(hid_mgr, matchingDictionary);
    IOHIDManagerScheduleWithRunLoop(hid_mgr, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    
    // see how many devices we have
    CFSetRef device_set = IOHIDManagerCopyDevices(hid_mgr);
    CFRelease(hid_mgr);
    
    /* Convert the list into a C array so we can iterate easily. */
    CFIndex num_devices = (device_set == NULL)? 0 : CFSetGetCount(device_set);
    
    if (num_devices)
    {
      BOOL shouldStop = FALSE;
      
      IOHIDDeviceRef *device_array = calloc(num_devices, sizeof(IOHIDDeviceRef));
      CFSetGetValues(device_set, (const void **) device_array);
      
      for (int i = 0; i < num_devices && !shouldStop ; i++)
      {
        IOHIDDeviceRef dev = device_array[i];
        
        if (dev) {
          result = block(dev, &shouldStop);
        }
        
      }
      free(device_array);
    }
    
    if(device_set != NULL)
    {
      CFRelease(device_set);      
    }
  }
  
  return result;
}
