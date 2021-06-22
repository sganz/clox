#include "hello.h"

const char* generateHelloString(const char* personName) 
{
    char* hello = NULL;
    int nameLength = strlen(personName);
    hello = malloc(9 + nameLength + 1);
    strcat(hello, "Hello ");
    strcat(hello, personName);
    return hello;
}