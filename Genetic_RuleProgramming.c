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

void find_effective_bits(EBS_t *cur_ebs);
float get_variance(int *data, int num);
float fitness_function(float variance, float dup_ratio);
void m_evaluate(rule_str_t *rules, chrom_t *chrom, bool show_result);
void m_select(chrom_t *chroms, int *sel_chroms);
chrom_t * m_crossover_mutation(chrom_t *chroms, int *next_chrom);
void dump_chrom(chrom_t _chrom);

int bit_mask[RULE_LEN];

/*
 * Rule Programming via genetic algorithm ----------------------------------------------------
 */
void rule_programming_genetic(EBS_t *ebs, int nb_ebs) {
    printf("\n\nStarting Genetic rule programming ...\n");

    /*
     * Initialize bit mask
     */
    int i;
    for (i = 0; i < RULE_LEN; i++) {
        bit_mask[i] = 0;
    }

    /*
     * find first ebs
     */
    if (ebs[0].nb_bits != 0)
        find_effective_bits(&ebs[0]);

    /*
     * mark ebs[0].bits as used in bit mask
     */
    for (i = 0; i < ebs[0].nb_bits; i++) {
        bit_mask[ebs[0].bits[i]] = 1;
    }

    /*
     * find second ebs
     */
    if (ebs[1].nb_bits != 0)
        find_effective_bits(&ebs[1]);

    printf("Done.\n");
}

void find_effective_bits(EBS_t *cur_ebs) {
    int nb_eb = cur_ebs->nb_bits;
    int i, j, iter;
    chrom_t *chroms;
    int next_chroms[nb_chroms / 2];
    chrom_t best_chrom;
    best_chrom.score = 0; // initail value

    // initialize chroms
    int *bit_vector;
    bit_vector = (int *) malloc(RULE_LEN * sizeof (int));
    memset(bit_vector, 0, RULE_LEN * sizeof (int));
    int random;
    srand(time(NULL));
    chroms = (chrom_t *) malloc(nb_chroms * sizeof (chrom_t));
    for (i = 0; i < nb_chroms; i++) {
        chroms[i].nb_eb = nb_eb;
        chroms[i].position = (int *) malloc(nb_eb * sizeof (int));
        chroms[i].id = i;
        chroms[i].score = 0;
        for (j = 0; j < nb_eb; j++) {
            random = rand() % RULE_LEN;
            while (bit_vector[random] != 0 || bit_mask[random] != 0)
                random = rand() % RULE_LEN;
            bit_vector[random] = 1;
            chroms[i].position[j] = random;
        }
#ifdef DUMP_CHROMS
        dump_chrom(chroms[i]);
#endif
    }

    iter = 0;
    while (iter++ < nb_iterations) {
#ifdef DEBUG_GENETIC
        printf("iteration %d ...\n\n", iter);
#endif

        // evaluation
        for (i = 0; i < nb_chroms; i++) {
            m_evaluate(rules_str, &chroms[i], false);
            if (chroms[i].score > best_chrom.score)
                best_chrom = chroms[i];
        }

        // selection
        m_select(chroms, next_chroms);

        // reproduction
        //m_reproduce(chroms);

        // crossover & mutation
        chroms = m_crossover_mutation(chroms, next_chroms);

#ifdef DUMP_CHROMS
        for (i = 0; i < nb_chroms; i++)
            dump_chrom(chroms[i]);
#endif

#ifdef DEBUG_GENETIC
        printf("Enter any key to go to the next iteration ... \n\n");
        getchar();
#endif
    }
#if 0
    evaluate(rules_str, &best_chrom, false);
#endif

    for (i = 0; i < cur_ebs->nb_bits; i++) {
        cur_ebs->bits[i] = best_chrom.position[i];
    }
}

float get_variance(int *data, int num) {
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

float fitness_function(float variance, float dup_ratio) {
    return (100 / (dup_ratio + (variance / nb_rules)));
}

void m_evaluate(rule_str_t *rules, chrom_t *chrom, bool show_result) {
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

    float var = get_variance(nb_next_level_rules, nb_grp);
    float dup_ratio = dup / (float) nb_rules;
    chrom->score = fitness_function(var, dup_ratio);

    if (show_result) {
        dump_chrom(*chrom);
        printf("nb_rules = ");
        for (i = 0; i < nb_grp; i++) {
            printf("%d ", nb_next_level_rules[i]);
        }
        printf("\tnb_dup = %d\t", dup);
        printf("var = %f\tdup_ratio = %f\n\n\n", var, dup_ratio);
    } else {
#ifdef DUMP_EVALUATE
        dump_chrom(*chrom);
        printf("nb_rules = ");
        for (i = 0; i < nb_grp; i++) {
            printf("%d ", nb_next_level_rules[i]);
        }
        printf("\tnb_dup = %d\t", dup);
        printf("var = %f\tdup_ratio = %f\n\n\n", var, dup_ratio);
#endif
    }

    free(nb_next_level_rules);
}

void m_select(chrom_t *chroms, int *sel_chroms) {
    int i, j;
    for (i = 0; i < nb_chroms / 2; i++) {
        float max = 0;
        int sel = -1;
        for (j = 0; j < nb_chroms; j++) {
            if (max < chroms[j].score) {
                max = chroms[j].score;
                sel = j;
            }
        }
        if (sel >= 0) {
            sel_chroms[i] = sel;
            chroms[sel].score = 0; // dont select in the next iteration
        }
    }
#ifdef DUMP_SELECT
    printf("selected chroms: ");
    for (i = 0; i < nb_chroms / 2; i++)
        printf("%d ", sel_chroms[i]);
    printf("\n\n\n");
#endif
}

void m_reproduce(chrom_t *chrom) {

}

chrom_t * m_crossover_mutation(chrom_t *chroms, int *next_chrom) {
    int i, j;
    int mut_i, mut_j, mut_rand;
    int nb_ebit = chroms[0].nb_eb;

    chrom_t * ret_chrom;
    ret_chrom = (chrom_t *) malloc(nb_chroms * sizeof (chrom_t));
    for (i = 0; i < nb_chroms; i++) {
        ret_chrom[i].nb_eb = chroms[0].nb_eb;
        ret_chrom[i].position = (int *) malloc(
                ret_chrom[i].nb_eb * sizeof (int));
    }

    int *bit_vector_chroms;
    bit_vector_chroms = (int *) malloc((nb_chroms / 2) * sizeof (int));
    memset(bit_vector_chroms, 0, (nb_chroms / 2) * sizeof (int));

    for (i = 0; i < (nb_chroms / 2) / 2; i++) {
        int sel1, sel2;
        while (1) {
            sel1 = rand() % (nb_chroms / 2);
            sel2 = rand() % (nb_chroms / 2);
            if (sel1 == sel2)
                continue;
            if (bit_vector_chroms[sel1] != 0 || bit_vector_chroms[sel2] != 0)
                continue;
            break;
        }
        bit_vector_chroms[sel1] = 1;
        bit_vector_chroms[sel2] = 1;

        sel1 = next_chrom[sel1];
        sel2 = next_chrom[sel2];

        int part1_len = (nb_ebit / 2);
        int part2_len = nb_ebit - part1_len;

        ret_chrom[4 * i].id = 4 * i;
        ret_chrom[4 * i].score = 0;
        memcpy(&ret_chrom[4 * i].position[0], &chroms[sel1].position[0],
                part1_len * sizeof (int));
        memcpy(&ret_chrom[4 * i].position[part1_len], &chroms[sel2].position[0],
                part2_len * sizeof (int));

        ret_chrom[4 * i + 1].id = 4 * i + 1;
        ret_chrom[4 * i + 1].score = 0;
        memcpy(&ret_chrom[4 * i + 1].position[0], &chroms[sel1].position[0],
                part1_len * sizeof (int));
        memcpy(&ret_chrom[4 * i + 1].position[part1_len],
                &chroms[sel2].position[part1_len], part2_len * sizeof (int));

        ret_chrom[4 * i + 2].id = 4 * i + 2;
        ret_chrom[4 * i + 2].score = 0;
        memcpy(&ret_chrom[4 * i + 2].position[0],
                &chroms[sel1].position[part1_len], part1_len * sizeof (int));
        memcpy(&ret_chrom[4 * i + 2].position[part1_len],
                &chroms[sel2].position[0], part2_len * sizeof (int));

        ret_chrom[4 * i + 3].id = 4 * i + 3;
        ret_chrom[4 * i + 3].score = 0;
        memcpy(&ret_chrom[4 * i + 3].position[0],
                &chroms[sel1].position[part1_len], part1_len * sizeof (int));
        memcpy(&ret_chrom[4 * i + 3].position[part1_len],
                &chroms[sel2].position[part1_len], part2_len * sizeof (int));
    }

    int *bit_vector = (int *) malloc(RULE_LEN * sizeof (int));
    memset(bit_vector, 0, RULE_LEN * sizeof (int));
    for (i = 0; i < nb_chroms; i++) {
        for (j = 0; j < ret_chrom[i].nb_eb; j++) {
            bit_vector[ret_chrom[i].position[j]] = 1;
        }
    }

    for (i = 0; i < nb_mutation; i++) {
        mut_i = rand() % nb_chroms;
        mut_j = rand() % nb_ebit;

        mut_rand = rand() % RULE_LEN;
        while (bit_mask[mut_rand] != 0 || bit_vector[mut_rand] != 0)
            mut_rand = rand() % RULE_LEN;

        ret_chrom[mut_i].position[mut_j] = mut_rand;
    }

    free(bit_vector);
    free(bit_vector_chroms);
    //free(chroms);
    return ret_chrom;
}

void dump_chrom(chrom_t _chrom) {
    int i;
    printf("chrom %-2d: bits:( ", _chrom.id);
    for (i = 0; i < _chrom.nb_eb; i++) {
        printf("%-3d ", _chrom.position[i]);
    }
    printf(")\tscore:%f\n", _chrom.score);
}