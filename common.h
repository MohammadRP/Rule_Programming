/* 
 * File:   common.h
 * Author: mrp
 *
 * Created on January 2, 2017, 11:18 PM
 */

#ifndef COMMON_H
#define COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

    /* Debug Options ******************************************************** */
    //#define DUMP_RULES
    //#define DUMP_RULES_STR

    //#define DUMP_WILDCARD_RATIO
    //#define DUMP_DIVERSITY_INDEX
    //#define DUMP_INDEPENDENCE_INDEX
    //#define DUMP_SELECTION_FACTOR
    //#define DUMP_SELECTED_EBS

    //#define DUMP_EVALUATE
    //#define DUMP_SELECT
    //#define DUMP_CHROMS
    //#define DEBUG_GENETIC



    /* MC-SBC Parameters **************************************************** */
#define threshold_factor_wr 0.9
#define threshold_factor_di 0.9
#define threshold_factor_ii 0.9

    /* Genetic Parameters *************************************************** */
#define NB_CHROMS               (4 * (int)((RULE_LEN / (nb_eb * 8)) + 1))
#define NB_ITERATIONS           200
#define NB_MUTATION             max((0.1 * nb_ebit * nb_chroms), 1)
#define NORM_VAR
#define GEN_ALPHA               1
#define GEN_BETA                1
#define NB_UNCHANGED_ITERATION  20


#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

    /* Rule Set Parameters ************************************************** */
#ifdef DUMP_RULES_STR
#define RULE_LEN   (356+14)
#else
#define RULE_LEN   356
#endif
#define INGRESS_PORT_LEN    32
#define METADATA_LEN        64
#define ETH_SRC_LEN         48
#define ETH_DST_LEN         48
#define ETHER_TYPE_LEN      16
#define VID_LEN             12
#define VPRTY_LEN           3
#define TOS_LEN             6
#define MPLS_LBL_LEN        20
#define MPLS_TFC_LEN        3
#define IP_SRC_LEN          32
#define IP_DST_LEN          32
#define PROTO_LEN           8
#define PORT_SRC_LEN        16
#define PORT_DST_LEN        16

    typedef struct ip {
        uint8_t bytes[4];
    } ip_t;

    typedef struct mac {
        uint8_t bytes[6];
    } mac_t;

    typedef struct port {
        uint16_t lower_bound;
        uint16_t upper_bound;
    } port_t;

    typedef struct rule_value {
        uint32_t ingress_port;
        uint64_t metadata;
        mac_t eth_src;
        mac_t eth_dst;
        uint16_t ether_type;
        uint16_t vid;
        uint8_t vprty;
        uint8_t tos;
        uint32_t mpls_lbl;
        uint8_t mpls_tfc;
        uint32_t ip_src;
        uint32_t ip_dst;
        uint8_t proto;
        port_t port_src;
        port_t port_dst;
    } value_t;

    typedef struct rule_mask {
        uint8_t ingress_ports_mask;
        uint8_t metadata_mask;
        uint8_t eth_src_mask;
        uint8_t eth_dst_mask;
        uint8_t ether_type_mask;
        uint8_t vid_mask;
        uint8_t vprty_mask;
        uint8_t tos_mask;
        uint8_t mpls_lbl_mask;
        uint8_t mpls_tfc_mask;
        uint32_t ip_src_mask;
        uint32_t ip_dst_mask;
        uint8_t proto_mask;
        uint16_t port_src_mask;
        uint16_t port_dst_mask;
    } mask_t;

    typedef struct rule {
        int id;
        value_t value;
        mask_t mask;
        uint32_t pri;
        void *action;
    } rule_t;

    typedef struct rule_str {
        int id;
        char value[RULE_LEN + 1];
    } rule_str_t;

    typedef struct EBS {
        int *bits;
        int nb_bits;
        int top;
    } EBS_t;

    typedef struct chrom {
        int id;
        float score;
        int *position;
        int nb_eb;
    } chrom_t;

    extern int nb_rules;
    extern rule_t *rules;
    extern rule_str_t *rules_str;

    typedef struct lookup_table_entry {
        int *rule_id;
        int nb_rules;
    } lookup_table_entry_t;

    typedef struct lookup_table {
        lookup_table_entry_t * entries;
        int nb_entries;
    } lookup_table_t;

    typedef struct subset {
        lookup_table_t *lookup_tables;
        int nb_tables;
    } subset_t;

#ifdef __cplusplus
}
#endif

#endif /* COMMON_H */

