# USB Human Interface Device CLI (hid)


#### Description
hid is a command line tool to send commands to an attached HID device. hid can also list all attached USB devices.

#### Usage 
For a list of all available options run `./hid -h`:

	$ ./hid -h

    Usage: hid -h
           hid list
           hid -l
           hid -v <vendorId> -p <productId> -setReport "exampleCommand1" ["exampleCommand2" ...] [-t <seconds_to_wait_for_response>] [-n <iterations>]

    -h                                  Help
    list                                Lists all HID devices
    -v <vendorId>                       Connect to a device with name
    -p <productId>                      Connect to a device with name
    -t <seconds_to_wait_for_response>   Seconds to wait for the device to respond to the command
    -n <iterations>                     Number of times to attempt the command
    -setReport "exampleCommand"         Quoted list of commands to send to the device

#### Example

`hid -v 0x05ac -p 0x850b -setReport "command" "another command" -t 2 -n 5`

#### Return Codes
hid can return the following exit codes:

	0		Success
	-1	Invalid Options/Invalid Parameters

### License
MIT