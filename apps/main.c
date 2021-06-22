#include <stdio.h>
#include "hello.h"

int main(int argc, char** argv) {
    char* helloJim = generateHelloString("Jim");
    printf("%s\n", helloJim);

    free(helloJim);

    return 0;
}