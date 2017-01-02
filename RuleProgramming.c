/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <memory.h>
#include <stdbool.h>

#include "common.h"
#include "RuleProgramming.h"

void n_dump_chrom(chrom_t _chrom) {
    int i;
    printf("chrom %-2d: bits:( ", _chrom.id);
    for (i = 0; i < _chrom.nb_eb; i++) {
        printf("%-3d ", _chrom.position[i]);
    }
    printf(")\tscore:%f\n", _chrom.score);
}

float variance(int *data, int num) {
    int i;
    float sum = 0;
    float avg = 0;
    float var_sum = 0;
    float var = 0;
    for (i = 0; i < num; i++)
        sum += data[i];
    avg = sum / num;
    for (i = 0; i < num; i++)
        var_sum += (data[i] - avg) * (data[i] - avg);
    var = var_sum / num;
    return var;
}

float fitness(float variance, float dup_ratio) {
    return (100 / (dup_ratio + (variance / nb_rules)));
}

void evaluate(rule_str_t *rules, chrom_t *chrom, bool show_result) {
    int i, j, k;

    int nb_ebit = chrom->nb_eb;
    int nb_grp = pow(2, nb_ebit);

    int *bit_pos;
    bit_pos = (int *) malloc(nb_ebit * sizeof (int));
    for (i = 0; i < nb_ebit; i++)
        bit_pos[i] = chrom->position[i];

    int *nb_next_level_rules;
    nb_next_level_rules = (int *) malloc(nb_grp * sizeof (int));
    memset(nb_next_level_rules, 0, nb_grp * sizeof (int));
    int dup = 0;

    for (i = 0; i < nb_rules; i++) {
        char **next_level_grp_id;
        int size = 0;
        next_level_grp_id = (char **) malloc(pow(3, nb_ebit) * sizeof (char *));
        char *value = (char *) malloc(nb_ebit + 1);
        for (j = 0; j < nb_ebit; j++)
            value[j] = rules[i].value[bit_pos[j]];
        value[nb_ebit] = '\0';
        next_level_grp_id[size++] = value;
        for (j = 0; j < size;) {
            char *sample = next_level_grp_id[j];
            for (k = 0; k < nb_ebit; k++) {
                if (sample[k] == '*') {
                    char *new_sample = (char *) malloc(nb_ebit + 1);
                    memcpy(new_sample, sample, nb_ebit + 1);
                    sample[k] = '0';
                    new_sample[k] = '1';
                    next_level_grp_id[size++] = new_sample;
                    dup++;
                    if (k == (nb_ebit - 1))
                        j++;
                    break;
                }
                if (k == (nb_ebit - 1))
                    j++;
            }
        }
        for (j = 0; j < size; j++) {
            unsigned int index = 0;
            char *value = next_level_grp_id[j];
            for (k = 0; k < nb_ebit; k++) {
                int bit_index = nb_ebit - (k + 1);
                if (value[k] == '1')
                    index |= (1 << bit_index);
            }
            nb_next_level_rules[index]++;
        }
        for (j = 0; j < size; j++) {
            free(next_level_grp_id[j]);
        }
        free(next_level_grp_id);

    } // end for(i=0; i<nb_rules; i++)

    float var = variance(nb_next_level_rules, nb_grp);
    float dup_ratio = dup / (float) nb_rules;
    chrom->score = fitness(var, dup_ratio);

    if (show_result) {
        n_dump_chrom(*chrom);
        printf("nb_rules = ");
        for (i = 0; i < nb_grp; i++) {
            printf("%d ", nb_next_level_rules[i]);
        }
        printf("\tnb_dup = %d\t", dup);
        printf("var = %f\tdup_ratio = %f\n\n\n", var, dup_ratio);
    } else {
#ifdef DUMP_EVALUATE
        n_dump_chrom(*chrom);
        printf("nb_rules = ");
        for (i = 0; i < nb_grp; i++) {
            printf("%d ", nb_next_level_rules[i]);
        }
        printf("\tnb_dup = %d\t", dup);
        printf("var = %f\tdup_ratio = %f\n\n\n", var, dup_ratio);
#endif
    }
}
