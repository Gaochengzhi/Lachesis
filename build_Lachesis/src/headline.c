/**
 * File              : headline.c
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 16.02.2022
 * Last Modified Date: 27.04.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */
#include "headline.h"
#include "lachesis_type.h"
#include <stdio.h>
#include <stdlib.h>
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

int check_commandline_argument(int *argc, char **argv, lenv *e)
{
    int len = *argc;
    int return_number = 0;
    for (int i = 1; i < len; i++)
    {
        if (argv[i][0] == '-')
        {

            *argc = *argc - 1;
            if (argv[i][1] == 'g')
            {
                _debug_mode = true;
                save_runtime_info_to_log();
                return_number = 1;
                goto new;
            }
            if (strstr(argv[i], "-std"))
            {
                LObject *args = lobj_add(lobj_sexpr(), lobj_string("stdlib"));
                LObject *x = built_in_import(e, args);
                if (x->type == LOBJ_ERR)
                {
                    lobj_print_line(x);
                }
                lobj_delete(x);
                goto new;
            }
            if (strstr(argv[i], "-h"))
            {
                print_help();
                exit(0);
            }
            else
            {
                printf("Lachesis: Invalid argument %s\n", argv[i]);
                puts(
                    "usage: lac [-gstdhelp] [-h -help] [-g] [-std] [filename]");
                exit(0);
            }
            new : for (int arg_index = i; arg_index < len - 1; ++arg_index)
            {
                argv[arg_index] = argv[arg_index + 1];
            }
        }
    }
    return return_number;
}
void print_help()
{
    puts("------ commandline argument ------");
    puts("-std \t\t import standard libarary to imteractive prompt.");
    puts("-h \t\t print help info to the stdout.");
    puts("-g \t\t save runtime info to log.txt.");
    puts("------ language syntax -----");
    puts("|----------------------------------------- \t| "
         "--------------------------------------------------\t|");
    puts("|interface                                 \t| example              "
         "                             \t|");
    puts("|----------------------------------------- \t| "
         "--------------------------------------------------\t|");
    puts("|[+-*/==!=><≥≤] <sexpr>*                   \t| * 1 5 => 5           "
         "                             \t|");
    puts("|head <qexpr> => <qexpr>                   \t| head {1 2 3} => {1}  "
         "                             \t|");
    puts("|tail <qexpr> => <qexpr>                   \t| tail {1 2 3} => {2 "
         "3}                             \t|");
    puts("|list <sexpr> => <qexpr>                   \t| list [+ 1 2] => {+ 1 "
         "2}                           \t|");
    puts("|join <qexpr> => <qexpr>                   \t| join {+} {1 2} => {+ "
         "1 2}                         \t|");
    puts("|if <bool> <qexpr> <qexpr> => <sexpr>      \t| if [== x y] {+ x y} "
         "{- x y}                       \t|");
    puts("|print <string>* => <string>               \t| print \"hello "
         "world\" => hello world              \t|");
    puts("|fprint <file_name> <string>* => <string>  \t| fprint \"new.txt\" "
         "\"hello world\" => new:hello world\t|");
    puts("|import <string>*                          \t| import \"stdlib\"    "
         "                             \t|");
    puts("|----------------------------------------- \t| "
         "--------------------------------------------------\t|\n");
}
