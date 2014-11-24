//
//  util.c
//  hid
//
//  Created by Eddie Woodley on 24/11/2014.
//  Copyright (c) 2014 Coderus. All rights reserved.
//

#include <stdio.h>
#include <string.h>

#include "util.h"

void output_report(unsigned char * report, int reportLength, int argc, const char **argv)
{
  
  // Parse out options
  int addNewline = 1;
  int removeReportId = 1;
  int outputAsHex = 0;
  
  for (int i = 0; i < argc; i++)
  {
    if(strcmp("-r", argv[i]) == 0)
    {
      removeReportId = 0;
    }
    
    if(strcmp("-o", argv[i]) == 0)
    {
      addNewline = 0;
    }
    
    if(strcmp("-x", argv[i]) == 0)
    {
      outputAsHex = 1;
    }
  }
  
  
  // Remove the report ID byte
  if(removeReportId)
  {
    report++;
    reportLength--;
  }
  
  if(outputAsHex)
  {
    char str[reportLength*3];
    
    unsigned char * pin = report;
    const char * hex = "0123456789ABCDEF";
    char * pout = str;
    for(int i = 1; pin < report+reportLength; i++, pout+=3, pin++){
      pout[0] = hex[(*pin>>4) & 0xF];
      pout[1] = hex[ *pin     & 0xF];
      pout[2] = (i % 16 == 0)? '\n' : ' ';
    }
    pout[-1] = 0;
    
    printf((addNewline)? "%s\n" : "%s", str);
  }
  else
  {
    fprintf(stdout, (addNewline)? "%s\n" : "%s", report);
  }

  
}