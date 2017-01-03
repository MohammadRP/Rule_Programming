/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: mrp
 *
 * Created on January 2, 2017, 11:14 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>

#include "init.h"
#include "common.h"
#include "RuleProgramming.h"

#include "common.h"
#include "init.h"

/*
 * 
 */
int main(int argc, char** argv) {

    struct timespec start, end;
    double diff;

    parse_args(argc, argv);
    init();

    clock_gettime(CLOCK_REALTIME, &start);
    rule_programming_mc_sbc();
    clock_gettime(CLOCK_REALTIME, &end);
    diff = (end.tv_sec - start.tv_sec) * 1e3 +
            (end.tv_nsec - start.tv_nsec) / 1e6;
    printf("MC-SBC takes %.2lf ms\n", diff);

    clock_gettime(CLOCK_REALTIME, &start);
    rule_programming_genetic();
    clock_gettime(CLOCK_REALTIME, &end);
    diff = (end.tv_sec - start.tv_sec) * 1e3 +
            (end.tv_nsec - start.tv_nsec) / 1e6;
    printf("Genetic takes %.2lf ms\n", diff);

    return (EXIT_SUCCESS);
}

