/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   init.h
 * Author: mrp
 *
 * Created on January 2, 2017, 11:17 PM
 */

#ifndef INIT_H
#define INIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

    char *input_file = NULL;
    int nb_rules;
    rule_t *rules;
    rule_str_t *rules_str;

    void usage(char *filename) {
        printf("Usage: %s -f filename [-h]\n", filename);
        exit(EXIT_FAILURE);
    }

    void parse_args(int argc, char **argv) {
        int c;
        opterr = 0;
        while ((c = getopt(argc, argv, "f:h")) != -1) {
            switch (c) {
                case 'f':
                    input_file = strdup(optarg);
                    break;
                case '?':
                case 'h':
                    usage(argv[0]);
                    break;
                default:
                    break;
            }
        }
        if (input_file == NULL) {
            printf("please specify rules file name\n");
            usage(argv[0]);
        }
        if (access(input_file, F_OK) != 0) {
            printf("%s does not exist\n", input_file);
            exit(EXIT_FAILURE);
        }
        if (access(input_file, R_OK) != 0) {
            printf("can not read from %s\n", input_file);
            exit(EXIT_FAILURE);
        }
    }

    void dump_rule(rule_t rule) {
        char *format = "rule%d:\n"
                "0x%x/%d\t" // ingress port
                "0x%lx/%d\t"// meta data
                "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx/%d\t"// src mac
                "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx/%d\t"// dst mac
                "0x%hx/%d\t"// ether type
                "0x%hx/%d\t"// vid
                "0x%hhx/%d\t"// vprty
                "0x%hhx/%d\t"// tos
                "0x%x/%d\t"// mpls lbl
                "0x%hhx/%d\t"// mpls tfc
                "0x%x/%x\t"// src ip
                "0x%x/%x\t"// dst ip
                "%hhx/%d\t"// proto
                "%hu:%hu\t"// src port
                "%hu:%hu\n"; // dst port

        printf(format,
                rule.id, //
                rule.value.ingress_port, rule.mask.ingress_ports_mask,
                rule.value.metadata, rule.mask.metadata_mask, /***/
                rule.value.eth_src.bytes[0], rule.value.eth_src.bytes[1],
                rule.value.eth_src.bytes[2], rule.value.eth_src.bytes[3],
                rule.value.eth_src.bytes[4], rule.value.eth_src.bytes[5],
                rule.mask.eth_src_mask, /***/
                rule.value.eth_dst.bytes[0], rule.value.eth_dst.bytes[1],
                rule.value.eth_dst.bytes[2], rule.value.eth_dst.bytes[3],
                rule.value.eth_dst.bytes[4], rule.value.eth_dst.bytes[5],
                rule.mask.eth_dst_mask, /***/
                rule.value.ether_type, rule.mask.ether_type_mask, /***/
                rule.value.vid, rule.mask.vid_mask, /***/
                rule.value.vprty, rule.mask.vprty_mask, /***/
                rule.value.tos, rule.mask.tos_mask, /***/
                rule.value.mpls_lbl, rule.mask.mpls_lbl_mask, /***/
                rule.value.mpls_tfc, rule.mask.mpls_tfc_mask, /***/
                rule.value.ip_src, rule.mask.ip_src_mask, /***/
                rule.value.ip_dst, rule.mask.ip_dst_mask, /***/
                rule.value.proto, rule.mask.proto_mask, /***/
                rule.value.port_src.lower_bound, rule.value.port_src.upper_bound, /***/
                rule.value.port_dst.lower_bound, rule.value.port_dst.upper_bound); /***/
        printf("------------------------------------------------------------\n");
    }

    void load_rule(rule_t *rule, char *input_rule) {
        char *format = ""
                "0x%08x/%d\t" // ingress port
                "0x%lx/%d\t"// meta data
                "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx/%d\t"// src mac
                "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx/%d\t"// dst mac
                "0x%hx/%d\t"// ether type
                "0x%hx/%d\t"// vid
                "0x%hhx/%d\t"// vprty
                "0x%hhx/%d\t"// tos
                "0x%x/%d\t"// mpls lbl
                "0x%hhx/%d\t"// mpls tfc
                "%hhu.%hhu.%hhu.%hhu/%x\t"// src ip
                "%hhu.%hhu.%hhu.%hhu/%x\t"// dst ip
                "%hhx/%d\t"// proto
                "%hu:%hu\t"// src port
                "%hu:%hu\n"; // dst port
        ip_t ip_src, ip_dst;
        sscanf(input_rule,
                format, //
                &rule->value.ingress_port, &rule->mask.ingress_ports_mask, /***/
                &rule->value.metadata, &rule->mask.metadata_mask, /***/
                &rule->value.eth_src.bytes[0], &rule->value.eth_src.bytes[1],
                &rule->value.eth_src.bytes[2], &rule->value.eth_src.bytes[3],
                &rule->value.eth_src.bytes[4], &rule->value.eth_src.bytes[5],
                &rule->mask.eth_src_mask, /***/
                &rule->value.eth_dst.bytes[0], &rule->value.eth_dst.bytes[1],
                &rule->value.eth_dst.bytes[2], &rule->value.eth_dst.bytes[3],
                &rule->value.eth_dst.bytes[4], &rule->value.eth_dst.bytes[5],
                &rule->mask.eth_dst_mask, /***/
                &rule->value.ether_type, &rule->mask.ether_type_mask, /***/
                &rule->value.vid, &rule->mask.vid_mask, /***/
                &rule->value.vprty, &rule->mask.vprty_mask, /***/
                &rule->value.tos, &rule->mask.tos_mask, /***/
                &rule->value.mpls_lbl, &rule->mask.mpls_lbl_mask, /***/
                &rule->value.mpls_tfc, &rule->mask.mpls_tfc_mask, /***/
                &ip_src.bytes[0], &ip_src.bytes[1], &ip_src.bytes[2],
                &ip_src.bytes[3], &rule->mask.ip_src_mask, /***/
                &ip_dst.bytes[0], &ip_dst.bytes[1], &ip_dst.bytes[2],
                &ip_dst.bytes[3], &rule->mask.ip_dst_mask, /***/
                &rule->value.proto, &rule->mask.proto_mask, /***/
                &rule->value.port_src.lower_bound,
                &rule->value.port_src.upper_bound, /***/
                &rule->value.port_dst.lower_bound,
                &rule->value.port_dst.upper_bound); /***/

        rule->value.ip_src = (ip_src.bytes[0] << 24) | (ip_src.bytes[1] << 16)
                | (ip_src.bytes[2] << 8) | ip_src.bytes[3];

        rule->value.ip_dst = (ip_dst.bytes[0] << 24) | (ip_dst.bytes[1] << 16)
                | (ip_dst.bytes[2] << 8) | ip_dst.bytes[3];
    }

    void dump_rule_str(rule_str_t rule_str) {
        printf("rule%d:\n%s\n", rule_str.id, rule_str.value);
    }

    void convert_rule_to_string_form(rule_t *rule, rule_str_t *rule_str) {
        int i, j;
        int offset = 0;

        //    rule id ---------------------------------------------------------------
        rule_str->id = rule->id;

        //    uint32_t ingress_port -------------------------------------------------
        if (rule->mask.ingress_ports_mask == 0) {
            for (i = 0; i < INGRESS_PORT_LEN; i++) {
                rule_str->value[i + offset] = '*';
            }
        } else {
            for (i = 0; i < INGRESS_PORT_LEN; i++) {
                if (rule->value.ingress_port & (1 << (INGRESS_PORT_LEN - i - 1)))
                    rule_str->value[i + offset] = '1';
                else
                    rule_str->value[i + offset] = '0';
            }
        }
        offset += INGRESS_PORT_LEN;
#ifdef DUMP_RULES_STR
        rule_str->value[offset++] = '\n';
#endif

        //    uint64_t metadata -----------------------------------------------------
        if (rule->mask.metadata_mask == 0) {
            for (i = 0; i < METADATA_LEN; i++) {
                rule_str->value[i + offset] = '*';
            }
        } else {
            for (i = 0; i < METADATA_LEN; i++) {
                if (rule->value.metadata & ((uint64_t) 1 << (METADATA_LEN - i - 1)))
                    rule_str->value[i + offset] = '1';
                else
                    rule_str->value[i + offset] = '0';
            }
        }
        offset += METADATA_LEN;
#ifdef DUMP_RULES_STR
        rule_str->value[offset++] = '\n';
#endif

        //    mac_t eth_src ---------------------------------------------------------
        if (rule->mask.eth_src_mask == 0) {
            for (i = 0; i < ETH_SRC_LEN; i++) {
                rule_str->value[i + offset] = '*';
            }
        } else {
            uint64_t eth_src = ((uint64_t) rule->value.eth_src.bytes[0] << 40)
                    | ((uint64_t) rule->value.eth_src.bytes[1] << 32)
                    | ((uint64_t) rule->value.eth_src.bytes[2] << 24)
                    | ((uint64_t) rule->value.eth_src.bytes[3] << 16)
                    | ((uint64_t) rule->value.eth_src.bytes[4] << 8)
                    | ((uint64_t) rule->value.eth_src.bytes[5]);
            for (i = 0; i < ETH_SRC_LEN; i++) {
                if (eth_src & ((uint64_t) 1 << (ETH_SRC_LEN - i - 1)))
                    rule_str->value[i + offset] = '1';
                else
                    rule_str->value[i + offset] = '0';
            }
        }
        offset += ETH_SRC_LEN;
#ifdef DUMP_RULES_STR
        rule_str->value[offset++] = '\n';
#endif

        //    mac_t eth_dst ----------------------------------------------------------
        if (rule->mask.eth_dst_mask == 0) {
            for (i = 0; i < ETH_DST_LEN; i++) {
                rule_str->value[i + offset] = '*';
            }
        } else {
            uint64_t eth_dst = ((uint64_t) rule->value.eth_dst.bytes[0] << 40)
                    | ((uint64_t) rule->value.eth_dst.bytes[1] << 32)
                    | ((uint64_t) rule->value.eth_dst.bytes[2] << 24)
                    | ((uint64_t) rule->value.eth_dst.bytes[3] << 16)
                    | ((uint64_t) rule->value.eth_dst.bytes[4] << 8)
                    | ((uint64_t) rule->value.eth_dst.bytes[5]);
            for (i = 0; i < ETH_DST_LEN; i++) {
                if (eth_dst & ((uint64_t) 1 << (ETH_DST_LEN - i - 1)))
                    rule_str->value[i + offset] = '1';
                else
                    rule_str->value[i + offset] = '0';
            }
        }
        offset += ETH_DST_LEN;
#ifdef DUMP_RULES_STR
        rule_str->value[offset++] = '\n';
#endif

        //    uint16_t ether_type -----------------------------------------------------
        if (rule->mask.ether_type_mask == 0) {
            for (i = 0; i < ETHER_TYPE_LEN; i++) {
                rule_str->value[i + offset] = '*';
            }
        } else {
            for (i = 0; i < ETHER_TYPE_LEN; i++) {
                if (rule->value.ether_type & (1 << (ETHER_TYPE_LEN - i - 1))) {
                    rule_str->value[i + offset] = '1';
                } else {
                    rule_str->value[i + offset] = '0';
                }
            }
        }
        offset += ETHER_TYPE_LEN;
#ifdef DUMP_RULES_STR
        rule_str->value[offset++] = '\n';
#endif

        //    uint12_t vid ------------------------------------------------------------
        if (rule->mask.vid_mask == 0) {
            for (i = 0; i < VID_LEN; i++) {
                rule_str->value[i + offset] = '*';
            }
        } else {
            for (i = 0; i < VID_LEN; i++) {
                if (rule->value.vid & (1 << (VID_LEN - i - 1))) {
                    rule_str->value[i + offset] = '1';
                } else {
                    rule_str->value[i + offset] = '0';
                }
            }
        }
        offset += VID_LEN;
#ifdef DUMP_RULES_STR
        rule_str->value[offset++] = '\n';
#endif

        //    uint3_t vprty -----------------------------------------------------------
        if (rule->mask.vprty_mask == 0) {
            for (i = 0; i < VPRTY_LEN; i++) {
                rule_str->value[i + offset] = '*';
            }
        } else {
            for (i = 0; i < VPRTY_LEN; i++) {
                if (rule->value.vprty & (1 << (VPRTY_LEN - i - 1))) {
                    rule_str->value[i + offset] = '1';
                } else {
                    rule_str->value[i + offset] = '0';
                }
            }
        }
        offset += VPRTY_LEN;
#ifdef DUMP_RULES_STR
        rule_str->value[offset++] = '\n';
#endif

        //    uint6_t tos -------------------------------------------------------------
        if (rule->mask.tos_mask == 0) {
            for (i = 0; i < TOS_LEN; i++) {
                rule_str->value[i + offset] = '*';
            }
        } else {
            for (i = 0; i < TOS_LEN; i++) {
                if (rule->value.tos & (1 << (TOS_LEN - i - 1))) {
                    rule_str->value[i + offset] = '1';
                } else {
                    rule_str->value[i + offset] = '0';
                }
            }
        }
        offset += TOS_LEN;
#ifdef DUMP_RULES_STR
        rule_str->value[offset++] = '\n';
#endif

        //    uint20_t mpls_lbl -------------------------------------------------------
        if (rule->mask.mpls_lbl_mask == 0) {
            for (i = 0; i < MPLS_LBL_LEN; i++) {
                rule_str->value[i + offset] = '*';
            }
        } else {
            for (i = 0; i < MPLS_LBL_LEN; i++) {
                if (rule->value.mpls_lbl & (1 << (MPLS_LBL_LEN - i - 1))) {
                    rule_str->value[i + offset] = '1';
                } else {
                    rule_str->value[i + offset] = '0';
                }
            }
        }
        offset += MPLS_LBL_LEN;
#ifdef DUMP_RULES_STR
        rule_str->value[offset++] = '\n';
#endif

        //    uint3_t mpls_tfc --------------------------------------------------------
        if (rule->mask.mpls_tfc_mask == 0) {
            for (i = 0; i < MPLS_TFC_LEN; i++) {
                rule_str->value[i + offset] = '*';
            }
        } else {
            for (i = 0; i < MPLS_TFC_LEN; i++) {
                if (rule->value.mpls_tfc & (1 << (MPLS_TFC_LEN - i - 1))) {
                    rule_str->value[i + offset] = '1';
                } else {
                    rule_str->value[i + offset] = '0';
                }
            }
        }
        offset += MPLS_TFC_LEN;
#ifdef DUMP_RULES_STR
        rule_str->value[offset++] = '\n';
#endif

        //    uint32_t ip_src ---------------------------------------------------------
        for (i = 0; i < IP_SRC_LEN; i++) {
            if (rule->mask.ip_src_mask & (1 << (IP_SRC_LEN - i - 1))) {
                if (rule->value.ip_src & (1 << (IP_SRC_LEN - i - 1))) {
                    rule_str->value[i + offset] = '1';
                } else {
                    rule_str->value[i + offset] = '0';
                }
            } else {
                rule_str->value[i + offset] = '*';
            }
        }
        offset += IP_SRC_LEN;
#ifdef DUMP_RULES_STR
        rule_str->value[offset++] = '\n';
#endif

        //    uint32_t ip_dst ---------------------------------------------------------
        for (i = 0; i < IP_DST_LEN; i++) {
            if (rule->mask.ip_dst_mask & (1 << (IP_DST_LEN - i - 1))) {
                if (rule->value.ip_dst & (1 << (IP_DST_LEN - i - 1))) {
                    rule_str->value[i + offset] = '1';
                } else {
                    rule_str->value[i + offset] = '0';
                }
            } else {
                rule_str->value[i + offset] = '*';
            }
        }
        offset += IP_DST_LEN;
#ifdef DUMP_RULES_STR
        rule_str->value[offset++] = '\n';
#endif

        //    uint8_t proto -----------------------------------------------------------
        if (rule->mask.proto_mask == 0) {
            for (i = 0; i < PROTO_LEN; i++) {
                rule_str->value[i + offset] = '*';
            }
        } else {
            for (i = 0; i < PROTO_LEN; i++) {
                if (rule->value.proto & (1 << (PROTO_LEN - i - 1))) {
                    rule_str->value[i + offset] = '1';
                } else {
                    rule_str->value[i + offset] = '0';
                }
            }
        }
        offset += PROTO_LEN;
#ifdef DUMP_RULES_STR
        rule_str->value[offset++] = '\n';
#endif

        //    port_t port_src ---------------------------------------------------------
        uint16_t start_value = rule->value.port_src.lower_bound;
        uint16_t end_value = rule->value.port_src.upper_bound;
        char port_prefix[PORT_SRC_LEN];
        for (i = 0; i < PORT_SRC_LEN; i++) {
            if (start_value & (1 << (PORT_SRC_LEN - i - 1))) {
                port_prefix[i] = '1';
            } else {
                port_prefix[i] = '0';
            }
        }
        for (i = start_value; i < end_value; i++) {
            uint16_t tmp = (uint16_t) i;
            for (j = 0; j < PORT_SRC_LEN; j++) {
                if (port_prefix[j] == '*')
                    continue;
                if (tmp & (1 << (PORT_SRC_LEN - j - 1))) {
                    if (port_prefix[j] == '0')
                        port_prefix[j] = '*';
                } else {
                    if (port_prefix[j] == '1')
                        port_prefix[j] = '*';
                }
            }
        }
        for (i = 0; i < PORT_SRC_LEN; i++) {
            rule_str->value[i + offset] = port_prefix[i];
        }
        offset += PORT_SRC_LEN;
#ifdef DUMP_RULES_STR
        rule_str->value[offset++] = '\n';
#endif

        //    port_t port_dst ---------------------------------------------------------
        start_value = rule->value.port_dst.lower_bound;
        end_value = rule->value.port_dst.upper_bound;
        for (i = 0; i < PORT_DST_LEN; i++) {
            if (start_value & (1 << (PORT_DST_LEN - i - 1))) {
                port_prefix[i] = '1';
            } else {
                port_prefix[i] = '0';
            }
        }
        for (i = start_value; i < end_value; i++) {
            uint16_t tmp = (uint16_t) i;
            for (j = 0; j < PORT_DST_LEN; j++) {
                if (port_prefix[j] == '*')
                    continue;
                if (tmp & (1 << (PORT_DST_LEN - j - 1))) {
                    if (port_prefix[j] == '0')
                        port_prefix[j] = '*';
                } else {
                    if (port_prefix[j] == '1')
                        port_prefix[j] = '*';
                }
            }
        }
        for (i = 0; i < PORT_DST_LEN; i++) {
            rule_str->value[i + offset] = port_prefix[i];
        }
        offset += PORT_DST_LEN;
        rule_str->value[RULE_LEN] = '\0';
    }

    void init(void) {

        FILE * fp;
        char * line = NULL;
        size_t len = 0;
        ssize_t read;
        char ch;
        int nb_lines = 0;
        int i = 0, j = 0;

        // get nb_rules -------------------------------
        fp = fopen(input_file, "r");
        nb_lines = 0;
        while (!feof(fp)) {
            ch = fgetc(fp);
            if (ch == '\n') {
                nb_lines++;
            }
        }
        nb_rules = nb_lines;
        fclose(fp);
        printf("NB_RULES : %d\n", nb_rules);

        // load rules ---------------------------------
        fp = fopen(input_file, "r");
        rules = (rule_t *) malloc(nb_rules * sizeof (rule_t));
        i = 0;
        while ((read = getline(&line, &len, fp)) != -1) {
            rules[i].id = i;
            load_rule(&rules[i], line);
#ifdef DUMP_RULES
            dump_rule(rules[i]);
#endif
            i++;
        }

        // convert rules to string --------------------
        rules_str = (rule_str_t *) malloc(nb_rules * sizeof (rule_str_t));
        for (i = 0; i < nb_rules; i++) {
            convert_rule_to_string_form(&rules[i], &rules_str[i]);
#ifdef DUMP_RULES_STR
            dump_rule_str(rules_str[i]);
            dump_rule(rules[i]);
#endif
        }

    }

#ifdef __cplusplus
}
#endif

#endif /* INIT_H */

