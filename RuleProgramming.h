/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RuleProgramming.h
 * Author: mrp
 *
 * Created on January 2, 2017, 11:34 PM
 */

#ifndef RULEPROGRAMMING_H
#define RULEPROGRAMMING_H

#ifdef __cplusplus
extern "C" {
#endif
    
#include <stdbool.h>

    //#define DUMP_WILDCARD_RATIO
    //#define DUMP_DIVERSITY_INDEX
    //#define DUMP_INDEPENDENCE_INDEX
    //#define DUMP_SELECTION_FACTOR
#define DUMP_SELECTED_EBS

    //#define DUMP_EVALUATE
    //#define DUMP_SELECT
    //#define DUMP_CHROMS
    //#define DEBUG_GENETIC

#define nb_chroms     16
#define nb_iterations 25
#define nb_mutation   max((0.1 * nb_ebit * nb_chroms), 1)

    void evaluate(rule_str_t *rules, chrom_t *chrom, bool show_result);
    void rule_programming_mc_sbc(void);
    void rule_programming_genetic(void);


#ifdef __cplusplus
}
#endif

#endif /* RULEPROGRAMMING_H */

