//
//  help.c
//  hid
//
//  Created by Eddie Woodley on 14/10/2014.
//  Copyright (c) 2014 Coderus. All rights reserved.
//

#include <stdio.h>
#include <strings.h>
#include "builtin.h"

int cmd_help(int argc, const char **argv)
{
  
  if(argc == 1){
    // output the general help
    fprintf(stdout,
            "hid is a command line tool to for interacting with attached HID devices.\n"
            "usage: hid [--help] <command> [args]\n"
            "Basic Commands:\n"
            "    list       List attached HID devices\n"
            "    info       List information on the elements of attached hid devices\n"
            "    getreport  Get a report from an attached device\n"
            "    setreport  Set a report for an attached device\n"
            "    attached   Check whether a matching device is attached or not\n"
            "\n"
            "General Options:\n"
            "    -n <int>   Run the given command <int> times [default: 1]\n"
            "\n"
            "Filtering Devices:\n"
            "Arguments can be provided to filter the devices to run the command against.\n"
            "example: hid list -v 0x19b5\n"
            "    -l <hex>   Filter by the given location ID\n"
            "    -v <hex>   Filter by the given vendor ID\n"
            "    -p <hex>   Filter by the given product ID\n"
            "\n"
            "See 'hid help <command>' for more information on a specific command.\n");
    
  } else {
    
    // TODO: this could be tidied up and possibly modularized into the builtin files
    const char *cmd = argv[1];
    if(strcmp("list", cmd) == 0)
    {
      fprintf(stdout,
              "hid list allows you to list attached hid devices.\n"
              "usage: hid list [--help] [args]\n"
              "\n"
              "List doesn't have any specific arguments, see 'hid --help' for common arguments.\n"
              );
    }
    else if(strcmp("info", cmd) == 0)
    {
      fprintf(stdout,
              "hid info allows you to view information on elements of attached hid devices.\n"
              "usage: hid info [--help] [args]\n"
              "\n"
              "Info doesn't have any specific arguments, see 'hid --help' for common arguments.\n"
              );
    }
    else if(strcmp("attached", cmd) == 0)
    {
      fprintf(stdout,
              "hid attached allows you to determine if a matching hid device is attached.\n"
              "usage: hid attached [--help] [args]\n"
              "\n"
              "The command will output '1' if a matching device is attached, otherwise '0'.\n"
              "\n"
              "Attached doesn't have any specific arguments, see 'hid --help' for common arguments.\n"
              );
    }
    else if(strcmp("getreport", cmd) == 0)
    {
      fprintf(stdout,
              "hid getreport allows you to get a report from an attached hid device.\n"
              "usage: hid getreport [--help] [-t <type>] [-i <report_id>]\n"
              "\n"
              "If more than one device is matched, the first-discovered matching device is used.\n"
              );
    }
    else if(strcmp("setreport", cmd) == 0)
    {
      fprintf(stdout,
              "hid setreport allows you to send a report to an attached hid device.\n"
              "usage: hid setreport [--help] [-t <type>] [-i <id>] [-w <time>] -d <data>\n"
              "\n"
              "This command is a WIP and therefore is incomplete and may change.\n"
              "If more than one device is matched, the first-discovered matching device is used.\n"
              "\n"
              "Arguments:\n"
              "    -d <data>  The data of the report, as hex (prefix with 0x) or as a string\n"
              "    -t <type>  One of 'output' or 'feature' [default: output]\n"
              "    -i <id>    Use the given report ID [default: 0]\n"
              "    -w <time>  If provided, the command will wait for an input report for the\n"
              "               given number of seconds\n"
              "\n"
              "For information on specifying a device, see 'hid --help'.\n"
              );
    }
    else
    {
      fprintf(stderr, "hid: '%s' is not a valid command. See 'hid --help'.\n", cmd);
      return 1;
    }
  }
  
  return 0;
}