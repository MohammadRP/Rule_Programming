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

#include "init.h"
#include "common.h"
#include "RuleProgramming.h"

#include "common.h"
#include "init.h"

/*
 * 
 */
int main(int argc, char** argv) {

    parse_args(argc, argv);
    init();
    rule_programming_mc_sbc();
    rule_programming_genetic();

    return (EXIT_SUCCESS);
}

