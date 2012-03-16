#include <stdio.h>
#include "b.h"
#include "../c/c.h"

void b()
{
    printf("calling b()\n");
}

void b1()
{
    printf("calling b1()\n");
    c();
}
