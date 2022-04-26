/**
 * File              : headline.c
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 16.02.2022
 * Last Modified Date: 15.04.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */
#include "headline.h"
#include "lachesis_type.h"
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
    puts("      Type h <name> or h help to help\n");
    puts("      add -g or --g to debug");
    puts("      add--std to include std library\n\n");
}
void save_runtime_info_to_log()
{

    time_t current_time;
    FILE *file;

    current_time = time(NULL);

    file = fopen("output.log", "a+");

    fprintf(file,
            "\n==============================================\nThis is output log "
            "in %s\n==============================================\n",
            ctime(&current_time));
    freopen("output.log", "a+", stderr);
    fclose(file);
}

void check_commandline_argument(int *argc, char **argv, lenv *e)
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
            else if (strstr(argv[i], "--std"))
            {
                LObject *args = lobj_add(lobj_sexpr(), lobj_string("stdlib"));
                LObject *x = built_in_import(e, args);
                if (x->type == LOBJ_ERR)
                {
                    lobj_print_line(x);
                }
                lobj_del(x);
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
    if (strstr(input, "basic") != NULL)
    {
        puts("basic: \
            \n - [* 12 14][+ 15 19] \
            ");
        putchar('\n');
    }
    if (strstr(input, "define") != NULL)
    {
        puts("define: \
            \n def {x} 100 \
            \n +x 1 \
            \n \
            \n def {a b} 5 6 \
            \n + ab \
            ");
        putchar('\n');
    }
    if (strstr(input, "head") != NULL)
    {
        puts("head: \
            \n argument must a Q-expression \
            \n head {12 13 14} => return {12 }");
        putchar('\n');
    }
    if (strstr(input, "tail") != NULL)
    {
        puts("tail: \
            \n argument must a Q-expression \
            \n tail {12 13 14} => return {12 14}");
        putchar('\n');
    }
    if (strstr(input, "func") != NULL)
    {
        puts("func: \
            \n [\\ {x y} {+ x y}] 10 20 \
            \n def {add} [\\ {x y} {+ x y}] \
            \n add 10 20\
            ");
        putchar('\n');
    }
    if (strstr(input, "if") != NULL)
    {
        puts("define: \
            \n def {x y} 100 200\
            \n if [ == x y ] {+ x y} {- x y} => -100\
            ");
        putchar('\n');
    }
    if (strstr(input, "help") != NULL)
    {
        puts("Check:\
                \n  basic \
                \n  define\
                \n  head\
                \n  tail\
                \n  func\
                \n  if\
                \n  ");
    }
    else
    {
        puts("No help find, check 'h check'");
    }
}
