#include <stdio.h>
#include "a.h"
#include "../b/b.h"

void a()
{
    printf("calling a()\n");
    b();
}

