#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "morsetree.h"

#define FLAG_TEST(node, flag)\
	((node)->flags &  (1 << (flag)))
#define FLAG_SET(node, flag)\
	(node)->flags |=  (1 << (flag))
#define FLAG_CLR(node, flag)\
	(node)->flags &= ~(1 << (flag))

morse_node_t *morse_tree_root;
morse_node_t *morse_current_node;

static inline morse_node_t* morse_tree_inorder_first(morse_node_t*, morse_node_t*);
static inline morse_node_t* morse_tree_inorder_last(morse_node_t*, morse_node_t*);
static        morse_node_t* morse_tree_inorder_next(morse_node_t*, morse_node_t*);
static inline morse_node_t* morse_node_new();
static        void          morse_tree_buildlist_inorder(morse_node_t*, morse_node_t*, morse_sym_t*, uint8_t*);

static inline morse_node_t* morse_tree_inorder_first(morse_node_t* cur, morse_node_t *skip) {
	while (cur->dit != NULL && cur->dit != skip && !FLAG_TEST(cur, MORSE_FLAG_DIT_ISTHREAD)) {
		cur = cur->dit;
	}
	return cur;
}

static inline morse_node_t* morse_tree_inorder_last(morse_node_t* cur, morse_node_t *skip) {
	while (cur->dah != NULL && cur->dah != skip && !FLAG_TEST(cur, MORSE_FLAG_DAH_ISTHREAD)) {
		cur = cur->dah;
	}
	return cur;
}

static morse_node_t* morse_tree_inorder_next(morse_node_t* cur, morse_node_t *skip) {
	if (cur->dah == NULL) { return NULL; }
	if (FLAG_TEST(cur, MORSE_FLAG_DAH_ISTHREAD)) { return cur->dah; }
	if (cur->dah == skip) {
		return morse_tree_inorder_last(cur->dah, NULL)->dah;
	} else {
		return morse_tree_inorder_first(cur->dah, skip);
	}
}

static void morse_tree_buildlist_inorder(morse_node_t *root, morse_node_t *skip, morse_sym_t *symlist, uint8_t *n) {
	morse_node_t *cur = morse_tree_inorder_first(root, skip);

	*n = 0;
	for (;;) {
		if (cur->sym != MORSE_SYM_NULL) {
			symlist[(*n)++] = cur->sym;
		}
		cur = morse_tree_inorder_next(cur, skip);
		if (cur == NULL || cur->depth < root->depth) { break; } /* don't leave this subtree */
	}
}

static inline morse_node_t *morse_node_new() {
	morse_node_t *node = malloc(sizeof(morse_node_t));
	node->dit   = NULL;
	node->dah   = NULL;
	node->depth = 0;
	node->flags = 0;
	node->sym   = MORSE_SYM_NULL;
	return node;
}

void morse_tree_insert(morse_sym_t sym, char* code) {
	morse_node_t* cur   = morse_tree_root;
	morse_node_t* next  = NULL;
	morse_node_t* prev  = NULL;
	for (uint8_t i=0; code[i] != 0; i++) {
		switch (code[i]) {
			case '.':
				next = cur;
				if (cur->dit == NULL || FLAG_TEST(cur, MORSE_FLAG_DIT_ISTHREAD)) {
					FLAG_CLR(cur, MORSE_FLAG_DIT_ISTHREAD);
					cur->dit        = morse_node_new();
					FLAG_SET(cur->dit, MORSE_FLAG_DIT_ISTHREAD);
					FLAG_SET(cur->dit, MORSE_FLAG_DAH_ISTHREAD);
					cur->dit->depth = cur->depth + 1;
					cur->dit->dit   = prev;
					cur->dit->dah   = next;
				}
				cur = cur->dit;
				break;
			case '-':
				prev = cur;
				if (cur->dah == NULL || FLAG_TEST(cur, MORSE_FLAG_DAH_ISTHREAD)) {
					FLAG_CLR(cur, MORSE_FLAG_DAH_ISTHREAD);
					cur->dah        = morse_node_new();
					FLAG_SET(cur->dah, MORSE_FLAG_DIT_ISTHREAD);
					FLAG_SET(cur->dah, MORSE_FLAG_DAH_ISTHREAD);
					cur->dah->depth = cur->depth + 1;
					cur->dah->dit   = prev;
					cur->dah->dah   = next;
				}
				cur = cur->dah;
				break;
		}
	}
	cur->sym = sym;
}

void morse_tree_get_hidden(morse_sym_t *symlist, uint8_t *n) {
	morse_tree_buildlist_inorder(morse_tree_root, morse_current_node, symlist, n);
}

void morse_tree_get_dits(morse_sym_t *symlist, uint8_t *n) {
	if (morse_current_node->dit != NULL && !FLAG_TEST(morse_current_node, MORSE_FLAG_DIT_ISTHREAD)) {
		morse_tree_buildlist_inorder(morse_current_node->dit, NULL, symlist, n);
	} else {
		*n = 0;
	}
}

void morse_tree_get_dahs(morse_sym_t *symlist, uint8_t *n) {
	if (morse_current_node->dah != NULL && !FLAG_TEST(morse_current_node, MORSE_FLAG_DAH_ISTHREAD)) {
		morse_tree_buildlist_inorder(morse_current_node->dah, NULL, symlist, n);
	} else {
		*n = 0;
	}
}

morse_sym_t morse_tree_get_selected() {
	return morse_current_node->sym;
}

void morse_tree_enter_dit() {
	if (morse_current_node->dit != NULL && !FLAG_TEST(morse_current_node, MORSE_FLAG_DIT_ISTHREAD)) {
		morse_current_node = morse_current_node->dit;
	}
}

void morse_tree_enter_dah() {
	if (morse_current_node->dah != NULL && !FLAG_TEST(morse_current_node, MORSE_FLAG_DAH_ISTHREAD)) {
		morse_current_node = morse_current_node->dah;
	}
}

morse_sym_t morse_tree_end_character() {
	morse_sym_t sym    = morse_current_node->sym;
	morse_current_node = morse_tree_root;
	return sym;
}

int morse_tree_init() {
	morse_tree_root    = morse_node_new();
	morse_current_node = morse_tree_root;
	
	morse_tree_insert(MORSE_SYM_A, ".-");
	morse_tree_insert(MORSE_SYM_B, "-...");
	morse_tree_insert(MORSE_SYM_C, "-.-.");
	morse_tree_insert(MORSE_SYM_D, "-..");
	morse_tree_insert(MORSE_SYM_E, ".");
	morse_tree_insert(MORSE_SYM_F, "..-.");
	morse_tree_insert(MORSE_SYM_G, "--.");
	morse_tree_insert(MORSE_SYM_H, "....");
	morse_tree_insert(MORSE_SYM_I, "..");
	morse_tree_insert(MORSE_SYM_J, ".---");
	morse_tree_insert(MORSE_SYM_K, "-.-");
	morse_tree_insert(MORSE_SYM_L, ".-..");
	morse_tree_insert(MORSE_SYM_M, "--");
	morse_tree_insert(MORSE_SYM_N, "-.");
	morse_tree_insert(MORSE_SYM_O, "---");
	morse_tree_insert(MORSE_SYM_P, ".--.");
	morse_tree_insert(MORSE_SYM_Q, "--.-");
	morse_tree_insert(MORSE_SYM_R, ".-.");
	morse_tree_insert(MORSE_SYM_S, "...");
	morse_tree_insert(MORSE_SYM_T, "-");
	morse_tree_insert(MORSE_SYM_U, "..-");
	morse_tree_insert(MORSE_SYM_V, "...-");
	morse_tree_insert(MORSE_SYM_W, ".--");
	morse_tree_insert(MORSE_SYM_X, "-..-");
	morse_tree_insert(MORSE_SYM_Y, "-.--");
	morse_tree_insert(MORSE_SYM_Z, "--..");
	morse_tree_insert(MORSE_SYM_0, "-----");
	morse_tree_insert(MORSE_SYM_1, ".----");
	morse_tree_insert(MORSE_SYM_2, "..---");
	morse_tree_insert(MORSE_SYM_3, "...--");
	morse_tree_insert(MORSE_SYM_4, "....-");
	morse_tree_insert(MORSE_SYM_5, ".....");
	morse_tree_insert(MORSE_SYM_6, "-....");
	morse_tree_insert(MORSE_SYM_7, "--...");
	morse_tree_insert(MORSE_SYM_8, "---..");
	morse_tree_insert(MORSE_SYM_9, "----.");
	morse_tree_insert(MORSE_SYM_PERIOD,     ".-.-.-");
	morse_tree_insert(MORSE_SYM_COMMA,      "--..--");
	morse_tree_insert(MORSE_SYM_QMARK,      "..--..");
	morse_tree_insert(MORSE_SYM_APOSTROPHE, ".----.");
	morse_tree_insert(MORSE_SYM_EPOINT,     "-.-.--");
	morse_tree_insert(MORSE_SYM_SLASH,      "-..-.");
	morse_tree_insert(MORSE_SYM_LPAREN,     "-.--.");
	morse_tree_insert(MORSE_SYM_RPAREN,     "-.--.-");
	morse_tree_insert(MORSE_SYM_AMPERSAND,  ".-...");
	morse_tree_insert(MORSE_SYM_COLON,      "---...");
	morse_tree_insert(MORSE_SYM_SEMICOLON,  "-.-.-.");
	morse_tree_insert(MORSE_SYM_EQUAL,      "-...-");
	morse_tree_insert(MORSE_SYM_PLUS,       ".-.-.");
	morse_tree_insert(MORSE_SYM_MINUS,      "-....-");
	morse_tree_insert(MORSE_SYM_UNDERSCORE, "..--.-");
	morse_tree_insert(MORSE_SYM_DQUOTE,     ".-..-.");
	morse_tree_insert(MORSE_SYM_DOLLAR,     "...-..-");
	morse_tree_insert(MORSE_SYM_AT,         ".--.-.");

	return 0;
}

static char morse_sym_ascii_table[] = {
	 0,
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
	'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'.', ',', '?', '\'', '!', '/', '(', ')', '&', ':', ';', '=', '+',
	'-', '_', '"', '$', '@'
};

char morse_sym_to_char(morse_sym_t sym) {
	return morse_sym_ascii_table[sym];
}

static void morse_tree_graphviz_subtree(FILE *file, morse_node_t *root) {
	char label[3] = {0, 0, 0};
	label[0] = morse_sym_to_char(root->sym);
	if (label[0] == '"') { label[0] = '\\'; label[1] = '"'; }
	fprintf(file, "\"%p\" [ label = \"%s\" ]\n", (void*)root, label);
	if (root->dit != NULL) {
  		fprintf(file, "\"%p\" -> \"%p\" [ style = \"dotted\", color=\"#%s\"]\n", (void*)root, (void*)root->dit, FLAG_TEST(root, MORSE_FLAG_DIT_ISTHREAD)?"0000FF":"000000");
		if (!FLAG_TEST(root, MORSE_FLAG_DIT_ISTHREAD)) { morse_tree_graphviz_subtree(file, root->dit); }
	}
	if (root->dah != NULL) {
		fprintf(file, "\"%p\" -> \"%p\" [ style = \"dashed\", color=\"#%s\"]\n", (void*)root, (void*)root->dah, FLAG_TEST(root, MORSE_FLAG_DAH_ISTHREAD)?"0000FF":"000000");
		if (!FLAG_TEST(root, MORSE_FLAG_DAH_ISTHREAD)) { morse_tree_graphviz_subtree(file, root->dah); }
	}
}

void morse_tree_graphviz(FILE *file)
{
  fprintf(file, "digraph tree {\n\n");
  morse_tree_graphviz_subtree(file, morse_tree_root);
  fprintf(file, "\n}\n");
}
