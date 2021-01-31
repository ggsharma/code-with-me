//
//  Part2_Functions.cpp
//  C++ Tutorials
//
//  Created by Gautam Sharma on 1/31/21.
//  Copyright © 2021 Gautam Sharma. All rights reserved.
//

#include <iostream>
using namespace std;


int sum(int x, int y=0, int z=3);
float sum(float x);


int main(){
    
    float x = 5.0;
    std::cout<<sum(1,2,3)<<" "<<sum(x)<<std::endl;
    
};


int sum(int x, int y,int z){
    return x+y+z;
};



float sum(float x){
    return x+1;
};



