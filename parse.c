/* DO NOT modify this file.
 * This is the provided parsing facility. 
 * You can call parse() to divide the user command line into useful pieces. */

#include <stdio.h>
#include <stdarg.h>

#include "parse.h"
#include "textproc.h"

/* Helper Functions */
static char *string_copy(const char *str);
static int contains(const char *needle, char *haystack[]);
static int parse_id_token(const char *p_tok, const char *instruct, int *id); 
static int parse_file_token(const char *p_tok, const char *instruct, char **file); 
static void dprintf(const char* fmt, ...); 

/* Reference Data */

// instructions which may use an ID argument
static char *instructs_with_id[] = {"write", "close", "exec", "active", "pause", "resume", "cancel", "print", NULL};

// instructions which may use a filename argument
static char *instructs_with_file[] = {"open", "write", NULL};

/*********
 * Command Parsing Functions
 *********/

void parse(const char *cmd_line, Instruction *inst, char *argv[]) {
  /* Step 0: ensure a valid input, and quit gracefully if there isn't one. */
    if (!cmd_line || !inst || !argv) return;

  /* Step 1: Only work on a copy of the original command */ 
    char *p_tok = NULL;
    char buffer[MAXLINE] = {0};
    strncpy(buffer, cmd_line, MAXLINE);

  /* Step 2: Tokenize the inputs (space delim) and parse */
    p_tok = strtok(buffer, " ");
    if (p_tok == NULL) { return; }

    /* Step 2a: Parse the instruction */
    inst->instruct = string_copy(p_tok);
    p_tok = strtok(NULL, " ");
    if (p_tok == NULL) { return; }

    /* Step 2b: Parse the buffer ID */
    if (parse_id_token(p_tok, inst->instruct, &inst->id )) {
        p_tok = strtok(NULL, " ");
        if (p_tok == NULL) { return; }
    }

    /* Step 2c: Parse the file name */
    if (parse_file_token(p_tok, inst->instruct, &inst->file)) {
        p_tok = strtok(NULL, " ");
        if (p_tok == NULL) { return; }
    }

    int index = 0;

    while(p_tok != NULL) {
        argv[index] = string_copy(p_tok);
	index++;
        p_tok = strtok(NULL, " ");
	if (index >= MAXARGS) p_tok = NULL;
    }
}



/* Parse the buffer ID from the current token.  If the input is a valid number token, and it corresponds to an 
 * appropriate instruction, then return true, else return false.  The id argument is populated with the ID
 * taken from p_tok.
 */
static int parse_id_token(const char *p_tok, const char *instruct, int *id) {
    if (!p_tok || !instruct || !id) { return 0; }

    if (!contains(instruct, instructs_with_id)) { return 0; }

    char *end = NULL;
    *id = (int) strtol(p_tok, &end, 10);

    // check if we successfully read a valid ID number
    if (!end || (*end) || end == p_tok) {
        *id = 0;
	return 0;
    }

    return 1;
}

/* Parse the file name from the current token.  If the input is a valid file token, and it corresponds to an 
 * appropriate instruction, then return true, else return false.  The file argument is populated with the 
 * file name taken from p_tok.
 */
static int parse_file_token(const char *p_tok, const char *instruct, char **file) {
    if (!p_tok || !instruct || !file) { return 0; }

    if (!contains(instruct, instructs_with_file)) { return 0; }

    *file = string_copy(p_tok);

    return 1;
}

/* Allocate and copy the contents of a string, or return NULL if the input is NULL. */
static char *string_copy(const char *str) {
    if (!str) { return NULL; }  // ignore NULL strings

    size_t size = strlen(str) + 1;
    char *copy = malloc(size);   // allocate space for the new string
    snprintf(copy, size, "%s", str);  // copy the string
    return copy;
}

/* Returns true of false depending on whether the haystack contains the needle. */
static int contains(const char *needle, char *haystack[]) {
    if (!needle || !haystack) return 0;

    for (int i = 0;  haystack[i];  i++) {
        if (strncmp(needle, haystack[i], strlen(haystack[i]) + 1) == 0) { return 1; }
    }
    return 0;
}

/*********
 * String Processing Helpers
 *********/

/* Returns 1 if string is all whitespace, else 0 */
int is_whitespace(const char *str) {
    if (!str) return 1;
    while (isspace(*str)) { str++; }  // skip past every whitespace character
    return *str == '\0';  // decide based on whether the first non-whitespace is the end-of-string
}

/*********
 * Initialization Functions
 *********/

int initialize_instruction(Instruction *inst) {
    if (!inst) return 0;

    inst->instruct = NULL;
    inst->id = 0;
    inst->file = NULL;

    return 1;
}

int initialize_argv(char *argv[]) {
    if (!argv) return 0;

    for(int i = 0; i < MAXARGS; i++) {
        argv[i] = NULL;
    }
    return 1;
}

int initialize_command(Instruction *inst, char *argv[]) {
    if (! initialize_instruction(inst)) return 0;
    if (! initialize_argv(argv)) return 0;
    return 1;
}


void free_instruction(Instruction *inst) {

    if (inst) {
        free(inst->instruct);
	inst->instruct = NULL;
	free(inst->file);
	inst->file = NULL;
    }

}

void free_argv(char *argv[]) {

    for(int i = 0; argv[i]; i++) {
        free(argv[i]);
        argv[i] = NULL;
    }

}

void free_command(Instruction *inst, char *argv[]) {
    free_instruction(inst);
    free_argv(argv);
}

/*********
 * Debug Functions
 *********/
static void dprintf(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	char buf[256] = {0};
	snprintf(buf, 256, "\033[1;33m[DEBUG] %s\033[0m", fmt);
	vfprintf(stderr, buf, args);
	va_end(args);
}

void debug_print_parse(char *cmdline, Instruction *inst, char *argv[], char *loc) {
    int i = 0;
    fprintf(stderr, "\n");
    dprintf("-------------\n");
    if (loc) { dprintf("- %s\n", loc); }
    if (loc) { dprintf("-------------\n"); }

    if(cmdline) { dprintf("cmdline     = %s\n", cmdline); }
  
    if(inst) {
        dprintf("instruction = %s\n", inst->instruct);
        if (inst->id) { dprintf("buffer ID  = %d\n", inst->id); }
        else { dprintf("buffer ID   = (default)\n"); }
        if (inst->file) { dprintf("file        = %s\n", inst->file); }
    }

    if(argv) {
        for(i = 0; argv[i]; i++) {
            dprintf("argv[%d] == %s\n", i, argv[i]);
	}
    }
  
    dprintf("-------------\n");
}
