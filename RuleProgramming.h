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
    
    void evaluate(rule_str_t *rules, chrom_t *chrom, bool show_result);
    void evaluate_new(rule_str_t *rules, EBS_t *ebs, int nb_ebs, float *ret_rep_ratio, int *ret_binth);
    void rule_programming_mc_sbc(EBS_t *ebss, int nb_ebs);
    void rule_programming_genetic(EBS_t *ebs, int nb_ebs);


#ifdef __cplusplus
}
#endif

#endif /* RULEPROGRAMMING_H */

