//
//  util.h
//  hid
//
//  Created by Eddie Woodley on 24/11/2014.
//  Copyright (c) 2014 Coderus. All rights reserved.
//

#ifndef hid_util_h
#define hid_util_h

// Output the given report buffer using options from argv
void output_report(unsigned char * report, int reportLength, int argc, const char **argv);

#endif
