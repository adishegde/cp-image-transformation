#include "image/composite_image.h"
#include "image/image.h"
#include<stdlib.h>
#include<iostream>

int main(int argv, char *argc[]){
    Image a(argc[1]), b(argc[2]);
    CompositeImage res(a, b, atof(argc[3]));

    res.writeTo("compositeImage.ppm");

    return 0;
}

