//
//  main.cpp
//  C++ Tutorials
//
//  Created by Gautam Sharma on 1/30/21.
//  Copyright © 2021 Gautam Sharma. All rights reserved.
//

// Basics

#include <iostream>
using namespace std;


int a = 1;
int b = 2.5;
float f = 3.14;
double g = 9.81;


int main(int argc, const char * argv[]) {
    
    cout << "Before "<< b << endl;
    b = (int)f; // casting
    cout << "After "<<b << endl;
    return 0;
}
