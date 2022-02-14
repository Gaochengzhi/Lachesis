#include "./headline.h"
#include <stdio.h>
#define _COSTOMIZE "default"
void lec_print_headline()
{
    struct utsname uname_pointer;

    uname(&uname_pointer);

    printf("Lachesis 0.0.1(%s)\n ", _COSTOMIZE);
    printf("Press Ctr+c to Quit\n");
    printf("System name - %s\n", uname_pointer.sysname);
    printf("Version     - %s\n", uname_pointer.release);
    printf("Machine     - %s\n", uname_pointer.machine);
}
