/**
 * File              : headline.c
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 16.02.2022
 * Last Modified Date: 24.03.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */
#include "headline.h"
#include <stdio.h>
#include <string.h>
#define _COSTOMIZE "default"
void lec_print_headline()
{
    struct utsname uname_pointer;

    uname(&uname_pointer);

    puts(
        " █████                          █████                        ███      "
        "      \n░░███                          ░░███                        "
        "░░░             \n ░███         ██████    ██████  ░███████    ██████  "
        " █████  ████   █████    \n ░███        ░░░░░███  ███░░███ ░███░░███  "
        "███░░███ ███░░  ░░███  ███░░     \n ░███         ███████ ░███ ░░░  "
        "░███ ░███ ░███████ ░░█████  ░███ ░░█████    \n ░███      █ ███░░███ "
        "░███  ███ ░███ ░███ ░███░░░   ░░░░███ ░███  ░░░░███   \n "
        "███████████░░████████░░██████  ███ █████░░██████  ██████  █████ "
        "██████    \n░░░░░░░░░░░  ░░░░░░░░  ░░░░░░  ░░░░ ░░░░░  ░░░░░░  ░░░░░░ "
        " ░░░░░ ░░░░░░     \n                                                  "
        "                          \n                                          "
        "                                  \n                                  "
        "                                          \n");
    printf("Lachesis 0.0.1(%s) \n", _COSTOMIZE);
    puts("if you want to quit, type 'q'\n");
    printf("System name: %s, ", uname_pointer.sysname);
    printf("Version: %s, ", uname_pointer.release);
    printf("Machine: %s. ", uname_pointer.machine);
    puts("\n");
    puts("      Type h or help to help");
    puts("      Type -g or --g to debug\n\n");
}
void save_runtime_info_to_log()
{

    time_t current_time;
    FILE *file;

    current_time = time(NULL);

    file = fopen("output.log", "a+");

    fprintf(file, "\n==============================================\nThis is output log in %s\n==============================================\n", ctime(&current_time));
    freopen("output.log", "a+", stderr);
    fclose(file);
}
void check_commandline_argument(int *argc, char **argv)
{
    for (int i = 1; i < *argc; i++)
    {
        if (argv[i][0] == '-')
        {
            *argc = *argc - 1;
            if (argv[i][1] == 'g')
            {
                _debug_mode = true;
                save_runtime_info_to_log();
            }
            else
            {
                puts("Invalid option.");
            }
            for (int arg_index = i; arg_index <= *argc; ++arg_index)
            {
                argv[arg_index] = argv[arg_index + 1];
            }
        }
    }
}
void print_help(char *input)
{
    input += 2;
    if (strstr(input, "function") != NULL)
    {
        puts("Function: \
            \n def {x} 100 \
            \n +x 1 \
            \n \
            \n def {a b} 5 6 \
            \n + ab \
            ");
    }
    else
    {
        printf("no help for: %s\n\n", input);
    }
}
