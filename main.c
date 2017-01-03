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

    int i;
    struct timespec start, end;
    double elapsed_time;

    parse_args(argc, argv);
    init();

    if (alg & ALG_MCSBC) {

        /*
         * Initialize Effective Bit Sets
         */
        EBS_t eff_bit_sets[2];
        init_ebs(eff_bit_sets, 2);

        /*
         * Get Start Time of Execution
         */
        clock_gettime(CLOCK_REALTIME, &start);

        /*
         * Start MC-SBC
         */
        rule_programming_mc_sbc(eff_bit_sets, 2);

        /*
         * Get End Time of Execution and Calculate Elapsed Time
         */
        clock_gettime(CLOCK_REALTIME, &end);
        elapsed_time = (end.tv_sec - start.tv_sec) * 1e3 +
                (end.tv_nsec - start.tv_nsec) / 1e6;
        printf("MC-SBC takes %.2lf ms\n", elapsed_time);

        /*
         * Evaluate Results
         */
        evaluate_new(rules_str, eff_bit_sets, 2);
    }

    if (alg & ALG_GENETIC) {
        /*
         * Initialize Effective Bit Sets
         */
        EBS_t eff_bit_sets[2];
        init_ebs(eff_bit_sets, 2);

        /*
         * Get Start Time of Execution
         */
        clock_gettime(CLOCK_REALTIME, &start);

        /*
         * Start Genetic
         */
        rule_programming_genetic(eff_bit_sets, 2);

        /*
         * Get End Time of Execution and Calculate Elapsed Time
         */
        clock_gettime(CLOCK_REALTIME, &end);
        elapsed_time = (end.tv_sec - start.tv_sec) * 1e3 +
                (end.tv_nsec - start.tv_nsec) / 1e6;
        printf("Genetic takes %.2lf ms\n", elapsed_time);

        /*
         * Evaluate Results
         */
        evaluate_new(rules_str, eff_bit_sets, 2);
    }

    return (EXIT_SUCCESS);
}

