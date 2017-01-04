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
    printf(")\n");
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

    // Calculate Binth
    int binth = 0;
    for (i = 0; i < nb_grp; i++) {
        if (binth < nb_next_level_rules[i])
            binth = nb_next_level_rules[i];
    }

    printf("------------------------------------------\n");
    n_dump_chrom(*chrom);
    printf("binth     = %d\n", binth);
    printf("nb_dup    = %d\n", dup);
    printf("dup_ratio = %f\n", dup_ratio);
    printf("variance  = %f\n", var);
    printf("score     = %f\n", chrom->score);
    printf("------------------------------------------\n");

    if (show_result) {
        printf("nb_rules = ");
        for (i = 0; i < nb_grp; i++) {
            printf("%d ", nb_next_level_rules[i]);
        }
    }
}

int * index_str_to_int(char *index_str, int *nb_id) {
    int i, j;
    int len = strlen(index_str);
    int num = 0;
    char **str_id;
    str_id = (char **) malloc(pow(3, len) * sizeof (char *));
    str_id[num++] = strdup(index_str);

    if (len == 0) {
        *nb_id = 1;
        int *id = (int *) malloc(sizeof (int));
        id[0] = 0;
        return id;
    }

    for (i = 0; i < num;) {
        char *sample = str_id[i];
        for (j = 0; j < len; j++) {
            if (j == len - 1)
                i++;
            if (sample[j] == '*') {
                char *new_sample = (char *) malloc(len * sizeof (char));
                memcpy(new_sample, sample, len);
                sample[j] = '0';
                new_sample[j] = '1';
                str_id[num++] = new_sample;
                break;
            }
        }
    }

    *nb_id = num;
    int * id = (int *) malloc(num * sizeof (int));

    for (i = 0; i < num; i++) {
        int index_int = 0;
        char *sample = str_id[i];
        for (j = 0; j < len; j++) {
            int bit_index = len - (j + 1);
            if (sample[j] == '1')
                index_int |= (1 << bit_index);
        }
        id[i] = index_int;
    }
    
    /*
     * free allocated memories
     */
    for (i = 0; i < num; i++)
        free(str_id[i]);
    free(str_id);

    return id;
}

void dump_ebs(EBS_t ebs, int ebs_id) {
    int i;
    printf("EBS %d: ( ", ebs_id);
    for (i = 0; i < ebs.nb_bits; i++) {
        printf("%-3d ", ebs.bits[i]);
    }
    printf(")\n");
}

void evaluate_new(rule_str_t *rules, EBS_t *ebs, int nb_ebs) {

    int i, j, k;
    int r;
    int dup = 0;
    int total_nb_rules = 0; // nb_rules + dup

    /*
     *  ebs[0] is used as subset index
     */
    int nb_subset = pow(2, ebs[0].nb_bits);
    subset_t *subsets;
    subsets = (subset_t *) malloc(nb_subset * sizeof (subset_t));

    /* 
     * now we assume that each subset has 1 lookup table 
     * and ebs[1] is used as its index
     */
    int nb_lookup_entries = pow(2, ebs[1].nb_bits);
    for (i = 0; i < nb_subset; i++) {
        subsets[i].nb_tables = 1; // this should be (nb_ebs - 1)
        subsets[i].lookup_tables =
                (lookup_table_t *) malloc(subsets[i].nb_tables * sizeof (lookup_table_t));
        for (j = 0; j < subsets[i].nb_tables; j++) {
            subsets[i].lookup_tables[j].nb_entries = nb_lookup_entries;
            subsets[i].lookup_tables[j].entries =
                    (lookup_table_entry_t *) malloc(nb_lookup_entries * sizeof (lookup_table_entry_t));
            for (k = 0; k < subsets[i].lookup_tables[j].nb_entries; k++) {
                subsets[i].lookup_tables[j].entries[k].nb_rules = 0;
            }
        }
    }

    /*
     * insert rules to lookup tables
     */
    char *subset_index = (char *) malloc((ebs[0].nb_bits + 1) * sizeof (char));
    char *table_index = (char *) malloc((ebs[1].nb_bits + 1) * sizeof (char));
    for (r = 0; r < nb_rules; r++) {
        // collect subset index from rule
        for (i = 0; i < ebs[0].nb_bits; i++) {
            subset_index[i] = rules[r].value[ebs[0].bits[i]];
        }
        subset_index[ebs[0].nb_bits] = '\0';

        // collect table index from rule
        for (i = 0; i < ebs[1].nb_bits; i++) {
            table_index[i] = rules[r].value[ebs[1].bits[i]];
        }
        table_index[ebs[1].nb_bits] = '\0';

        int *subset_id;
        int subset_num = 0;
        subset_id = index_str_to_int(subset_index, &subset_num);

        for (i = 0; i < subset_num; i++) {
            int *table_id;
            int table_num = 0;
            table_id = index_str_to_int(table_index, &table_num);
            for (j = 0; j < table_num; j++) {
                subsets[subset_id[i]].lookup_tables[0].entries[table_id[j]].nb_rules++;
                total_nb_rules++;
                /* 
                 * TODO:Add r to 
                 *  subsets[subset_id[i]].lookup_tables[0].entries[table_id[j]].rule_id
                 */
            }
            free(table_id);
        }
        free(subset_id);
    }

    dup = total_nb_rules - nb_rules;

    //    float var = variance(nb_next_level_rules, nb_grp);
    float dup_ratio = dup / (float) nb_rules;
    //    chrom->score = fitness(var, dup_ratio);

    // Calculate Binth
    int binth = 0;
    for (i = 0; i < nb_subset; i++) {
        for (j = 0; j < nb_lookup_entries; j++)
            if (binth < subsets[i].lookup_tables[0].entries[j].nb_rules)
                binth = subsets[i].lookup_tables[0].entries[j].nb_rules;
    }

    printf("------------------------------------------\n");
    printf("number of subsets=%-6d ", nb_subset);
    dump_ebs(ebs[0], 0);
    printf("lookup table size=%-6d ", nb_lookup_entries);
    dump_ebs(ebs[1], 1);
    printf("binth     = %d\n", binth);
    printf("nb_dup    = %d\n", dup);
    printf("dup_ratio = %f\n", dup_ratio);
    //    printf("variance  = %f\n", var);
    //    printf("score     = %f\n", chrom->score);
    printf("------------------------------------------\n");

    /*
     * free allocated memories
     */
    free(table_index);
    free(subset_index);
    for (i = 0; i < nb_subset; i++) {
        for (j = 0; j < subsets[i].nb_tables; j++) {
            free(subsets[i].lookup_tables[j].entries);
        }
        free(subsets[i].lookup_tables);
    }
    free(subsets);
}