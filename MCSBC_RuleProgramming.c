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

void rule_programming_mc_sbc(void) {
    printf("Starting MC_SBC rule programming ...\n");

    // Get Wildcard Ratio
    get_wildcard_ratio();
    wildcard_ratio_threshold = (1 - threshold_factor_wr) * max_wildcard_ratio;
#ifdef DEBUG_MCSBC
    printf("Wildcard Ratio calculated: MAX=%lf THRESHOLD=%lf\n", max_wildcard_ratio, wildcard_ratio_threshold);
#endif

    // Get Diversity Index
    get_diversity_index();
    diversity_index_threshold = threshold_factor_di * max_diversity_index;
#ifdef DEBUG_MCSBC
    printf("Diversity index calculated: MAX=%lf THRESHOLD=%lf\n", max_diversity_index, diversity_index_threshold);
#endif

    // Get Independence Index
    get_independence_index();
    independence_index_threshold = (1 - threshold_factor_ii) * max_independence_index;
#ifdef DEBUG_MCSBC
    printf("independence_index calculated: MAX=%lf THRESHOLD=%lf\n", max_independence_index, independence_index_threshold);
#endif

    // Generate Selection Factor
    generate_selection_factor();
#ifdef DEBUG_MCSBC
    printf("selection factor generated.\n");
#endif
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
    evaluate(rules_str, &chrom, false);

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
                - ((pi_zero / (1 - pi_star)) * log10(pi_zero / (1 - pi_star)))
                - ((pi_one / (1 - pi_star)) * log10(pi_one / (1 - pi_star)));
        if (diversity_index[i] != diversity_index[i]) { // check for nan
            diversity_index[i] = 0;
        }
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
            if (i == j) {
                independence_index[i][j] = 10; // much greater than normal values
                continue;
            }
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
#ifdef DUMP_SELECTION_FACTOR
            printf("%d,%d\twr=%lf di=%lf ii=%lf sf=%lf\n",
                    i, j,
                    wildcard_ratio[i][j],
                    diversity_index[i],
                    independence_index[i][j],
                    selection_factor[i][j]);
#endif
        }
    }
}

void select_effective_bits(EBS_t *ebs, uint8_t nb_ebs) {
#ifdef DEBUG_MCSBC
    printf("Trying to select %d EBS ... \n", nb_ebs);
#endif
    int i, j;
    int bit_vector[RULE_LEN];
    int nb_unused_bits = RULE_LEN;

    // Mark all bits as unused;
    for (i = 0; i < RULE_LEN; i++) {
        bit_vector[i] = 0;
    }

    for (i = 0, j = 0; i < RULE_LEN; i++) {
        if (diversity_index[i] < diversity_index_threshold) {
            bit_vector[i] = 1;
            j++;
        }
    }
#ifdef DEBUG_MCSBC
    printf("%d bits marked as used\n", j);
#endif

    // while Unused bits are available do
    int ebs_index = 0;
    while (nb_unused_bits > 0 && ebs_index < nb_ebs) {
#ifdef DEBUG_MCSBC
        printf("EBS %d: nb_bits=%d\n", ebs_index, ebs[ebs_index].nb_bits);
#endif
        // for All unused bits in a rule set do
        for (; ebs[ebs_index].top < ebs[ebs_index].nb_bits && nb_unused_bits > 0;) {
            double max_selection_factor = 0;
            int bit0 = -1, bit1 = -1;
            for (i = 0; i < RULE_LEN; i++) {
                if (bit_vector[i] > 0)
                    continue;
                // find maximum selection factor
                for (j = 0; j < RULE_LEN; j++) {
                    if (bit_vector[j] > 0 || i == j)
                        continue;
                    if (max_selection_factor < selection_factor[i][j]) {
                        max_selection_factor = selection_factor[i][j];
                        bit0 = i;
                        bit1 = j;
                    }
                }
            }

            if (bit0 == -1 || bit1 == -1) {
                printf("MC-SBC could not find requested EBS\n");
                exit(EXIT_FAILURE);
            }
#ifdef DEBUG_MCSBC
            printf("%d,%d --> wr=%lf di=%lf ii=%lf\n",
                    bit0, bit1,
                    wildcard_ratio[bit0][bit1],
                    diversity_index[bit0],
                    independence_index[bit0][bit1]);
#endif
            if (wildcard_ratio[bit0][bit1] < wildcard_ratio_threshold
                    // && diversity_index[bit0] > diversity_index_threshold
                    // && diversity_index[bit1] > diversity_index_threshold
                    && independence_index[bit0][bit1] < independence_index_threshold) {
                // Append Ri , Rj to current EBS;
                ebs[ebs_index].bits[ebs[ebs_index].top++] = bit0;
                ebs[ebs_index].bits[ebs[ebs_index].top++] = bit1;
                // Mark Ri , Rj as used;
                bit_vector[bit0] = 1;
                bit_vector[bit1] = 1;
                nb_unused_bits -= 2;
                if (ebs[ebs_index].top >= ebs[ebs_index].nb_bits)
                    break;
            } else {
                bit_vector[bit0] = 1;
                bit_vector[bit1] = 1;
                nb_unused_bits -= 2;
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