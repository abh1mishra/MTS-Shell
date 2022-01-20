/* Do not modify this file */

#ifndef PARSE_H
#define PARSE_H

#include <ctype.h> /* isspace */

/* Types: Instruction.
 *
 * This is a record which keeps information about which instruction we're execting.
 * 
 * instruct field: This holds the name of the instruction which we're exectuing 
 *          (e.g. "new", "open", "exec", "list", "help", "quit", ...). 
 * id field: This holds the ID of the buffer which the command is being applied to. 
 *          If we're using the current default buffer, or if the command doesn't use a buffer
 *          (e.g. "help", "list"), then the id value will be 0.
 * file field: If the command has an associated filename ("open" and "write"), the filename 
 *          goes here.  If there is no associated file, then this field will be NULL.
 *
 * If the instruction includes a command to be executed ("new" and "exec"), then the command and
 * its arguments will be stored in a separate argv[] list.
 */
typedef struct instruction_struct{
	char *instruct;   // the instruction we're running
	int id;           // the buffer ID associated with the command, or 0 if default
	char *file;       // the filename associated with the command
} Instruction;

/* Command Parsing Functions: parse(). 
 *
 * This command will take a provided command line string and parse it into an Instruction structure and,
 * if necessary, the associated command and its arguments will be loaded into argv[].  It is necessary to
 * initialize the Instruction and argv before the call, and to free them afterwards (see the Constructors 
 * and Destructors section).
 *
 * Inputs:
 * cmd_line - The text of the command line, as entered by the user.
 * inst - A pre-initialized Instruction structure, which will be populated on return with the details of 
 *         the instruction which the user entered.  The caller is responsible for releasing the resources
 *         used by this structure - using free_instruction() or free_command() - once they are done with it.
 * argv - A pre-initialized list of strings, which if needed will contain the command to be executed and all
 *         of its arguments.  If used, argv[0] will be the name of the command to run, while the remainder
 *         of argv[] should be the list of arguments used to run the command.  The caller is responsible 
 *         for releasing the resources used by this list - using free_argv() or free_command() - once they 
 *         are done with it.
*/
void parse(const char* cmd_line, Instruction *inst, char *argv[]);

/* String Processing Functions: is_whitespace(). 
 *
 * Returns true if str contains only whitespace, or is NULL. Otherwise, it returns false.
 */
int is_whitespace(const char *str);

/* Constructors and Descrutors: initialze/free the resources associated with Instruction and/or argv.
 *
 * The initialize_* instructions each return true on success, false on failure.
 *
 * The *.command() variations of each function will initialize/free both structures. 
 * The free_instruction() and free_command() call do not deallocate the Instruction structure itself, only its
 *         associated resources.  Likewise, initialize_instruction() and initialize_command() will not malloc
 *         a new Instruction if the input is NULL. 
 */
int initialize_instruction(Instruction *inst);
int initialize_argv(char *argv[]);
int initialize_command(Instruction *inst, char *argv[]);  
void free_instruction(Instruction *inst);
void free_argv(char *argv[]);
void free_command(Instruction *inst, char *argv[]);  

/* Debug Functions: debug_print_parse().
 *
 * A debug function which we can use to show the command line which was entered and the parsed structures
 * which were used.  We can optionally provide a freeform string in the loc argument, to print out info about
 * the context. */
void debug_print_parse(char *cmdline, Instruction *inst, char *argv[], char *loc);

#endif /*PARSE_H*/
