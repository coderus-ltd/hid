//
//  help.c
//  hid
//
//  Created by Eddie Woodley on 14/10/2014.
//  Copyright (c) 2014 Coderus. All rights reserved.
//

#include <stdio.h>
#include "builtin.h"

int cmd_help(int argc, const char **argv)
{
  
  if(argc == 1){
    // output the general help
    fprintf(stdout, "General help here.\n");
  } else {
    // output help for the specific command
    fprintf(stdout, "Help on %s here.\n", argv[1]);
  }
  
  return 0;
}