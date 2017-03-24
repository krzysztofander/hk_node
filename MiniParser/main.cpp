//
//  main.cpp
//  Parsers
//
//  Created by Bartłomiej on 23/03/17.
//  Copyright © 2017 Imagination Systems. All rights reserved.
//

#include <iostream>
#include "MiniInParserTests.h"

int main(int argc, const char * argv[]) {
    if (testMiniInParser() == true) {
      printf("SUCCESS\n");
    } else {
      printf("FAILURE\n");
    }
    return 0;
}
