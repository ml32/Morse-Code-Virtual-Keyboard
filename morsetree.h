#ifndef MORSETREE_H
#define MORSETREE_H

#include <stdint.h>
#include <stdio.h>

typedef int morse_sym_t;

typedef struct morse_node {
	struct morse_node *dit, *dah;
	uint8_t            flags;
	uint8_t            depth;
	morse_sym_t        sym;
} morse_node_t;

#define MORSE_FLAG_DIT_ISTHREAD 0
#define MORSE_FLAG_DAH_ISTHREAD 1

extern morse_node_t *morse_tree_root;
extern morse_node_t *morse_current_node;

/* Must be called to construct the morse code tree */
int  morse_tree_init();

/* Used internally to insert symbols -- can be used to add custom symbols */
void morse_tree_insert(morse_sym_t sym, char* code);

/* Fill symlist with symbols which are not part of the selected node's subtree */
/* "symlist" must be at least as large as MORSE_SYMLIST_SIZE, plus custom symbols added */
void morse_tree_get_hidden(morse_sym_t *symlist, uint8_t *n);
/* Fill symlist with symbols which can be reached by entering a "dit" */
void morse_tree_get_dits(morse_sym_t *symlist, uint8_t *n);
/* Fill symlist with symbols which can be reached by entering a "dah" */
void morse_tree_get_dahs(morse_sym_t *symlist, uint8_t *n);
/* Return the symbol of the currently selected node */
morse_sym_t morse_tree_get_selected();

/* Enter a "dit", update morse_current_node */
void morse_tree_enter_dit();
/* Enter a "dah", update morse_current_node */
void morse_tree_enter_dah();
/* Return the current symbol and reset morse_current node to morse_tree_root */
morse_sym_t morse_tree_end_character();

#define MORSE_SYM_NULL         0
#define MORSE_SYM_A            1
#define MORSE_SYM_B            2
#define MORSE_SYM_C            3
#define MORSE_SYM_D            4
#define MORSE_SYM_E            5
#define MORSE_SYM_F            6
#define MORSE_SYM_G            7
#define MORSE_SYM_H            8
#define MORSE_SYM_I            9
#define MORSE_SYM_J           10
#define MORSE_SYM_K           11
#define MORSE_SYM_L           12
#define MORSE_SYM_M           13
#define MORSE_SYM_N           14
#define MORSE_SYM_O           15
#define MORSE_SYM_P           16
#define MORSE_SYM_Q           17
#define MORSE_SYM_R           18
#define MORSE_SYM_S           19
#define MORSE_SYM_T           20
#define MORSE_SYM_U           21
#define MORSE_SYM_V           22
#define MORSE_SYM_W           23
#define MORSE_SYM_X           24
#define MORSE_SYM_Y           25
#define MORSE_SYM_Z           26
#define MORSE_SYM_0           27
#define MORSE_SYM_1           28
#define MORSE_SYM_2           29
#define MORSE_SYM_3           30
#define MORSE_SYM_4           31
#define MORSE_SYM_5           32
#define MORSE_SYM_6           33
#define MORSE_SYM_7           34
#define MORSE_SYM_8           35
#define MORSE_SYM_9           36
#define MORSE_SYM_PERIOD      37
#define MORSE_SYM_COMMA       38
#define MORSE_SYM_QMARK       39
#define MORSE_SYM_APOSTROPHE  40
#define MORSE_SYM_EPOINT      41
#define MORSE_SYM_SLASH       42
#define MORSE_SYM_LPAREN      43
#define MORSE_SYM_RPAREN      44
#define MORSE_SYM_AMPERSAND   45
#define MORSE_SYM_COLON       46
#define MORSE_SYM_SEMICOLON   47
#define MORSE_SYM_EQUAL       48
#define MORSE_SYM_PLUS        49
#define MORSE_SYM_MINUS       50
#define MORSE_SYM_UNDERSCORE  51
#define MORSE_SYM_DQUOTE      52
#define MORSE_SYM_DOLLAR      53
#define MORSE_SYM_AT          54

#define MORSE_SYM_COUNT       55
#define MORSE_SYMLIST_SIZE    MORSE_SYM_COUNT

char morse_sym_to_char(morse_sym_t sym);
void morse_tree_graphviz(FILE *file);

#endif
