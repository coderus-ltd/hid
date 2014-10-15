//
//  hid.c
//  hid
//
//  Created by Eddie Woodley on 14/10/2014.
//  Copyright (c) 2014 Coderus. All rights reserved.
//

#include <stdio.h>
#include <strings.h>
#include "builtin.h"

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

/// Represents a sub command, its handler and any options it needs.
struct cmd_struct
{
  
  const char *cmd;
  int (*fn)(int, const char **);
  int option;
  
};

/// Define all subcommands and their handlers
static struct cmd_struct commands[] =
{
  { "help", cmd_help, 0 },
  { "list", cmd_list, 0 },
  { "setreport", cmd_setreport, 0 }
};

/// Run a command from the struct, performing any setup and applying any options.
static int run_builtin(struct cmd_struct *p, int argc, const char **argv)
{
  // Inspect options and perform any setup here.
  // Currently, there are no options.
  return p->fn(argc, argv);
}

int main(int argc, const char * argv[])
{
  int i;
  
  // Adjust argv for convenience
  argv++;
  argc--;
  
  if(argc > 0)
  {
    // Read subcommand
    const char *cmd = argv[0];
    
    // Map --help to help subcommand, adjust argv as needed
    if(strcmp("--help", argv[argc-1]) == 0)
    {
      argv[1] = argv[0];
      argv[0] = cmd = "help";
    }
    
    // Find and call the matching subcommand handler
    for (i = 0; i < ARRAY_SIZE(commands); i++)
    {
      struct cmd_struct *p = commands+i;
      if (strcmp(p->cmd, cmd) == 0)
      {
        return run_builtin(p, argc, argv);
      }
    }
    
    // No matching subcommand handler found!
    fprintf(stderr, "hid: '%s' is not a valid command. See 'hid --help'.\n", cmd);
    return 1;
  }
  else
  {
    fprintf(stderr, "hid: subcommand required. See 'hid --help'.\n");
    return 1;
  }
  
}
