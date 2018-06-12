//
// Created by Administrator on 2018/6/1.
//

#include <stdio.h>
#include <stdlib.h>



//C/C++混编，指示编译器按照C语言进行编译
extern "C"
{
#include "include/libavcodec/avcodec.h"
};

void main() {
    printf("%s", avcodec_configuration());

    system("pause");
}