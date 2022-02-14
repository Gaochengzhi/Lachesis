#include "./headline.h"
#include <stdio.h>
#define _COSTOMIZE "default"
void lec_print_headline()
{
    struct utsname uname_pointer;

    uname(&uname_pointer);

    printf("Lachesis 0.0.1(%s), ", _COSTOMIZE);
    printf("Press Ctr+c to Quit.\n\n");
    printf("System name - %s\n", uname_pointer.sysname);
    printf("Version     - %s\n", uname_pointer.release);
    printf("Machine     - %s\n", uname_pointer.machine);
    puts("\n");
    puts("This prototype is designed to show what is parsing.\n");
    puts("type something like '+ 12 13'\n");
}
