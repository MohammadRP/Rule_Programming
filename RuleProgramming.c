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

/*
 * Rule Programming via MC-SBC ---------------------------------------------------------------
 */
#define NB_BITS_EBS1  10
#define threshold_factor 0.9

double **wildcard_ratio;
double max_wildcard_ratio = 0;
double wildcard_ratio_threshold;

double *diversity_index;
double max_diversity_index = 0;
double diversity_index_threshold;

double **independence_index;
double max_independence_index = 0;
double independence_index_threshold;

double **selection_factor;

void get_wildcard_ratio(void);
void get_diversity_index(void);
void get_independence_index(void);
void generate_selection_factor(void);
void select_effective_bits(EBS_t *ebs, uint8_t nb_ebs);

void find_effective_bits(rule_t *rules, int nb_eb);
float get_variance(int *data, int num);
float fitness_function(float variance, float dup_ratio);
void m_evaluate(rule_str_t *rules, chrom_t *chrom, bool show_result);
void m_select(chrom_t *chroms, int *sel_chroms);
chrom_t * m_crossover_mutation(chrom_t *chroms, int *next_chrom);
void dump_chrom(chrom_t _chrom);

void rule_programming_mc_sbc(void) {
    printf("Starting MC_SBC rule programming ...\n");

    // Get Wildcard Ratio
    get_wildcard_ratio();
    wildcard_ratio_threshold = (1 - threshold_factor) * max_wildcard_ratio;
    printf("Wildcard Ratio calculated. th=%lf\n", wildcard_ratio_threshold);

    // Get Diversity Index
    get_diversity_index();
    diversity_index_threshold = threshold_factor * max_diversity_index;
    printf("Diversity index calculated. th=%lf\n", diversity_index_threshold);

    // Get Independence Index
    get_independence_index();
    independence_index_threshold = (1 - threshold_factor)
            * max_independence_index;
    printf("independence_index calculated. th=%lf\n", independence_index_threshold);

    // Generate Selection Factor
    generate_selection_factor();
    printf("selection factor generated.\n");

    EBS_t eff_bit_sets[1];
    // EBS 0
    eff_bit_sets[0].nb_bits = NB_BITS_EBS1;
    eff_bit_sets[0].bits = (int *) malloc(
            eff_bit_sets[0].nb_bits * sizeof (int));
    int i;
    for (i = 0; i < eff_bit_sets[0].nb_bits; i++)
        eff_bit_sets[0].bits[i] = 0;
    eff_bit_sets[0].top = 0;

#if 0
    // EBS 1
    eff_bit_sets[1].nb_bits = 10;
    eff_bit_sets[1].bits = (int *) malloc(
            eff_bit_sets[1].nb_bits * sizeof (int));
    for (i = 0; i < eff_bit_sets[1].nb_bits; i++)
        eff_bit_sets[1].bits[i] = 0;
    eff_bit_sets[1].top = 0;

    // Select Effective Bits
    select_effective_bits(eff_bit_sets, 2);
#else
    select_effective_bits(eff_bit_sets, 1);
#endif

    chrom_t chrom;
    chrom.id = 0;
    chrom.nb_eb = eff_bit_sets[0].nb_bits;
    chrom.position = (int *) malloc(chrom.nb_eb * sizeof (int));
    for (i = 0; i < chrom.nb_eb; i++)
        chrom.position[i] = eff_bit_sets[0].bits[i];
    m_evaluate(rules_str, &chrom, true);

    printf("Done.\n\n");
}

void get_wildcard_ratio(void) {
    int i, j, r;
    wildcard_ratio = (double **) malloc(RULE_LEN * sizeof (double *));
    for (i = 0; i < RULE_LEN; i++) {
        wildcard_ratio[i] = (double *) malloc(RULE_LEN * sizeof (double));
    }
    for (i = 0; i < RULE_LEN; i++) {
        for (j = i; j < RULE_LEN; j++) {
            if (i == j) {
                wildcard_ratio[i][j] = 1;
                continue;
            }
            int nstarstar = 0;
            for (r = 0; r < nb_rules; r++) {
                if (rules_str[r].value[i] == '*'
                        && rules_str[r].value[j] == '*')
                    nstarstar++;
            }
            wildcard_ratio[i][j] = ((double) nstarstar / ((double) nb_rules));
            wildcard_ratio[j][i] = wildcard_ratio[i][j];
            if (max_wildcard_ratio < wildcard_ratio[i][j])
                max_wildcard_ratio = wildcard_ratio[i][j];
        }
    }
#ifdef DUMP_WILDCARD_RATIO
    sleep(2);
    printf("\nCalculated Wildcard Ratio:\n");
    for (i = 0; i < RULE_LEN; i++) {
        for (j = 0; j < RULE_LEN; j++) {
            printf("bits (%-2d,%-2d) --> %-.4lf\n", i, j, wildcard_ratio[i][j]);
        }
    }
#endif
}

void get_diversity_index(void) {
    int i, r;
    diversity_index = (double *) malloc(RULE_LEN * sizeof (double));
    for (i = 0; i < RULE_LEN; i++) {
        int nb_zero = 0, nb_one = 0, nb_star = 0;
        double pi_zero = 0, pi_one = 0, pi_star = 0;
        for (r = 0; r < nb_rules; r++) {
            if (rules_str[r].value[i] == '0')
                nb_zero++;
            else if (rules_str[r].value[i] == '1')
                nb_one++;
            else
                nb_star++;
        }
        pi_one = (double) nb_one / (double) nb_rules;
        pi_zero = (double) nb_zero / (double) nb_rules;
        pi_star = (double) nb_star / (double) nb_rules;
        diversity_index[i] = 0
                - ((pi_zero / (1 - pi_star)) * log(pi_zero / (1 - pi_star)))
                - ((pi_one / (1 - pi_star)) * log(pi_one / (1 - pi_star)));
        if (max_diversity_index < diversity_index[i])
            max_diversity_index = diversity_index[i];
    }
#ifdef DUMP_DIVERSITY_INDEX
    printf("\nCalculated Diversity Index:\n");
    sleep(2);
    for (i = 0; i < RULE_LEN; i++) {
        printf("bit %d --> %.4lf\n", i, diversity_index[i]);
    }
#endif
}

void get_independence_index(void) {
    int i, j, x, y, r;
    char bit_value[3] = {'0', '1', '*'};

    independence_index = (double **) malloc(RULE_LEN * sizeof (double *));
    for (i = 0; i < RULE_LEN; i++) {
        independence_index[i] = (double *) malloc(RULE_LEN * sizeof (double));
    }

    for (i = 0; i < RULE_LEN; i++) {
        for (j = 0; j < RULE_LEN; j++) {
            // i & j
            independence_index[i][j] = 0;
            for (x = 0; x < 3; x++) {
                for (y = 0; y < 3; y++) {
                    double n_xy = 0, n_x = 0, n_y = 0;
                    double p_xy = 0, p_x = 0, p_y = 0;
                    for (r = 0; r < nb_rules; r++) {
                        if (rules_str[r].value[i] == bit_value[x]
                                && rules_str[r].value[j] == bit_value[y])
                            n_xy += 1;
                        if (rules_str[r].value[i] == bit_value[x])
                            n_x += 1;
                        if (rules_str[r].value[j] == bit_value[y])
                            n_y += 1;
                    }
                    p_xy = n_xy / (double) nb_rules;
                    p_x = n_x / (double) nb_rules;
                    p_y = n_y / (double) nb_rules;
                    double partial_independence_index = p_xy - (p_x * p_y);
                    if (partial_independence_index < 0) // abs
                        partial_independence_index =
                            -partial_independence_index;
                    independence_index[i][j] += partial_independence_index;
                }
            }
            if (max_independence_index < independence_index[i][j])
                max_independence_index = independence_index[i][j];
        }
    }
#ifdef DUMP_INDEPENDENCE_INDEX
    printf("\nCalculated Independence Index:\n");
    sleep(2);
    for (i = 0; i < RULE_LEN; i++) {
        for (j = 0; j < RULE_LEN; j++) {
            printf("bits (%-2d,%-2d) --> %lf\n", i, j,
                    independence_index[i][j]);
        }
    }
#endif
}

void generate_selection_factor(void) {
    int i, j;
    double alpha = 1, beta = 1, gamma = 1;
    selection_factor = (double **) malloc(RULE_LEN * sizeof (double *));
    for (i = 0; i < RULE_LEN; i++) {
        selection_factor[i] = (double *) malloc(RULE_LEN * sizeof (double));
    }
    for (i = 0; i < RULE_LEN; i++) {
        for (j = 0; j < RULE_LEN; j++) {
            selection_factor[i][j] = alpha * (1 - wildcard_ratio[i][j])
                    + beta * diversity_index[i]
                    + gamma * (1 - independence_index[i][j]);
        }
    }
#ifdef DUMP_SELECTION_FACTOR
    printf("\nCalculated Selection Factor:\n");
    sleep(2);
    for (i = 0; i < RULE_LEN; i++) {
        for (j = 0; j < RULE_LEN; j++) {
            printf("bits (%-2d,%-2d) --> %lf\n", i, j, selection_factor[i][j]);
        }
    }
#endif
}

void select_effective_bits(EBS_t *ebs, uint8_t nb_ebs) {

    printf("Trying to select %d EBS ... \n", nb_ebs);

    int i, j;
    int bit_vector[RULE_LEN];
    int nb_unused_bits = RULE_LEN;

    // Mark all bits as unused;
    for (i = 0; i < RULE_LEN; i++) {
        bit_vector[i] = 0;
    }

    // while Unused bits are available do
    int ebs_index = 0;
    while (nb_unused_bits > 0 && ebs_index < nb_ebs) {
        printf("EBS %d: nb_bits=%d\n", ebs_index, ebs[ebs_index].nb_bits);
        // for All unused bits in a rule set do
        for (i = 0; i < RULE_LEN; i++) {
            if (bit_vector[i] > 0)
                continue;
            double max_selection_factor = 0;
            int bit0 = i, bit1 = -1;
            // find maximum selection factor
            for (j = 0; j < RULE_LEN; j++) {
                if (bit_vector[j] > 0)
                    continue;
                if (max_selection_factor < selection_factor[i][j]) {
                    max_selection_factor = selection_factor[i][j];
                    bit1 = j;
                }
            }

            if (wildcard_ratio[bit0][bit1] < wildcard_ratio_threshold
                    && diversity_index[bit0] > diversity_index_threshold
                    && independence_index[bit0][bit1]
                    < independence_index_threshold) {
                // Append Ri , Rj to current EBS;
                ebs[ebs_index].bits[ebs[ebs_index].top++] = bit0;
                ebs[ebs_index].bits[ebs[ebs_index].top++] = bit1;
                // Mark Ri , Rj as used;
                bit_vector[bit0] = 1;
                bit_vector[bit1] = 1;
                nb_unused_bits -= 2;
                if (ebs[ebs_index].top >= ebs[ebs_index].nb_bits)
                    break;
            }
        }
#ifdef DUMP_SELECTED_EBS
        for (i = 0; i < ebs[ebs_index].nb_bits; i++) {
            printf("%d\t", ebs[ebs_index].bits[i]);
        }
        printf("\n");
#endif
        // Move to next EBS;
        ebs_index++;
    }

}

/*
 * Rule Programming via genetic algorithm ----------------------------------------------------
 */
void rule_programming_genetic(void) {
    find_effective_bits(rules, NB_BITS_EBS1);
}

void find_effective_bits(rule_t *rules, int nb_eb) {
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
            while (bit_vector[random] != 0)
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
    m_evaluate(rules_str, &best_chrom, true);
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
    int i, mut_i, mut_j;
    int nb_ebit = chroms[0].nb_eb;

    chrom_t * ret_chrom;
    ret_chrom = (chrom_t *) malloc(nb_chroms * sizeof (chrom_t));
    for (i = 0; i < nb_chroms; i++) {
        ret_chrom[i].nb_eb = chroms[0].nb_eb;
        ret_chrom[i].position = (int *) malloc(
                ret_chrom[i].nb_eb * sizeof (int));
    }

    int *bit_vector;
    bit_vector = (int *) malloc((nb_chroms / 2) * sizeof (int));
    memset(bit_vector, 0, (nb_chroms / 2) * sizeof (int));

    for (i = 0; i < (nb_chroms / 2) / 2; i++) {
        int sel1, sel2;
        while (1) {
            sel1 = rand() % (nb_chroms / 2);
            sel2 = rand() % (nb_chroms / 2);
            if (sel1 == sel2)
                continue;
            if (bit_vector[sel1] != 0 || bit_vector[sel2] != 0)
                continue;
            break;
        }
        bit_vector[sel1] = 1;
        bit_vector[sel2] = 1;

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

    for (i = 0; i < nb_mutation; i++) {
        mut_i = rand() % nb_chroms;
        mut_j = rand() % nb_ebit;

        ret_chrom[mut_i].position[mut_j] = rand() % RULE_LEN;
    }

    free(bit_vector);
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

