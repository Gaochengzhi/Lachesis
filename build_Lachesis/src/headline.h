/**
 * File              : headline.h
 * License           : The MIT License (MIT)
 * Author            : Gao Chengzhi <2673730435@qq.com>
 * Date              : 16.02.2022
 * Last Modified Date: 01.04.2022
 * Last Modified By  : Gao Chengzhi <2673730435@qq.com>
 */
#pragma once
#ifndef _HeadLine_H
#define _HeadLine_H
#include "lachesis_builtin.h"
#include "lachesis_debug.h"
#include "lachesis_object.h"
#include "lachesis_type.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <time.h>
#include <unistd.h>
void lec_print_headline();
void print_help(char* input);
void save_runtime_info_to_log();
void check_commandline_argument(int* argc, char** argv, lenv* e);
#endif
