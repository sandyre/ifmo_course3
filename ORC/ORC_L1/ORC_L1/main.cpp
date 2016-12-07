//
//  main.cpp
//  ORC_L1
//
//  Created by Aleksandr Borzikh on 10.11.16.
//  Copyright © 2016 sandyre. All rights reserved.
//

#include <iostream>
#include "lexical_analyzer.hpp"

int main(int argc, const char * argv[])
{
    LexicalAnalyzer LexAnal("/users/aleksandr/Desktop/test.lang");
    LexAnal.Analyze();
    LexAnal.PrintContent();
}
