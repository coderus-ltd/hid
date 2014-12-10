//
//  version.c
//  hid
//
//  Created by Eddie Woodley on 10/12/2014.
//  Copyright (c) 2014 Coderus. All rights reserved.
//

#include <stdio.h>
#include "../version.h"

int cmd_version(int argc, const char **argv)
{
  printf("%s\n", VERSION);
  return 0;
}