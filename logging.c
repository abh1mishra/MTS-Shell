/* Do Not Modify This File */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "logging.h"

#define BUFSIZE 255

#define textproc_log(s) printf("\033[1;31m%s%s\033[0m",log_head, s); fflush(stdout)
#define textproc_unmarked_log(s) printf("\033[1;31m%s\033[0m", s); fflush(stdout)

#define textproc_write(s) char output[BUFSIZE] = {0}; snprintf(output,BUFSIZE-1,"\033[1;31m%s%s\033[0m", log_head, s); write(STDOUT_FILENO, output, strlen(output));

const char *log_head = "[AAHLOG] ";
/* Outputs the Help: All the Built-in Commands */
void log_help() { 
  textproc_log("Welcome to the AAH Text Processing System!\n");
  textproc_log("Instructions: help, quit, list\n");
  textproc_log("    open FILE, write [BUFID] FILE,\n");
  textproc_log("    close [BUFID], print [BUFID], active BUFID,\n");
  textproc_log("    new [COMMAND [ARGS ...]],\n");
  textproc_log("    exec [BUFID] COMMAND [ARGS ...],\n");
  textproc_log("    pause [BUFID], resume [BUFID], cancel [BUFID]\n");
  textproc_log("\n");
  textproc_log("Brackets denote optional arguments\n");
}

/* Outputs the message after running quit */
void log_quit(){
  textproc_log("Thanks for using AAH! Good-bye!\n");
}

/* Outputs the prompt */
void log_prompt() {
  printf("AAH>> ");
  fflush(stdout);
}

/* Outputs a notification of a buffer activation */
void log_activate(int buf_id) {
  char buffer[BUFSIZE] = {0};
  sprintf(buffer, "Activating Buffer ID #%d\n", buf_id);
  textproc_write(buffer);
}

/* Outputs a notification of a buffer activation */
void log_show_active(int buf_id) {
  char buffer[BUFSIZE] = {0};
  if (buf_id == 0) {
      sprintf(buffer, "No active buffers\n");
  } else {
      sprintf(buffer, "Buffer ID #%d is currently active\n", buf_id);
  }
  textproc_log(buffer);
}

/* Outputs a notification of a buffer print */
void log_print(int buf_id, const char *data) {
  char buffer[BUFSIZE] = {0};
  sprintf(buffer, "Printing Buffer ID #%d\n", buf_id);
  textproc_log(buffer);
  textproc_log("----------------------------------\n");
  textproc_unmarked_log(data);
  textproc_log("----------------------------------\n");
}

/* Outputs a notification of a read from file */
void log_read(int buf_id, const char *file) {
  char buffer[BUFSIZE] = {0};
  sprintf(buffer, "Read from file %s into Buffer ID #%d\n", file, buf_id);
  textproc_log(buffer);
}

/* Outputs a notification of a write to file */
void log_write(int buf_id, const char *file) {
  char buffer[BUFSIZE] = {0};
  sprintf(buffer, "Write from Buffer ID #%d into file %s\n", buf_id, file);
  textproc_log(buffer);
}

/* Outputs a notification of a buffer open */
void log_open(int buf_id) {
  char buffer[BUFSIZE] = {0};
  sprintf(buffer, "Opening Buffer ID #%d\n", buf_id);
  textproc_log(buffer);
}

/* Outputs a notification of a buffer close */
void log_close(int buf_id) {
  char buffer[BUFSIZE] = {0};
  sprintf(buffer, "Closing Buffer ID #%d\n", buf_id);
  textproc_log(buffer);
}

/* Outputs a notification of a buffer close error */
void log_close_error(int buf_id) {
  char buffer[BUFSIZE] = {0};
  sprintf(buffer, "Error Closing Buffer ID #%d due to active process\n", buf_id);
  textproc_log(buffer);
}

/* Outputs a notification of an file error */
void log_file_error(int error_type, const char *file) {
  static char *file_errs[] = {"opening for read", "opening for write"};
  if (error_type < 0 || error_type >= 2) {
	  textproc_write("Invalid input to log_file_error\n");
	  return;
  }
  char buffer[BUFSIZE] = {0};
  sprintf(buffer, "Error %s using file %s\n", file_errs[error_type], file);
  textproc_log(buffer);
}

/* Output when the command is not found
 * eg. User typed in ls instead of /bin/ls and exec returns an error
 */ 
void log_command_error(const char *line) {
  char buffer[BUFSIZE] = {0};
  sprintf(buffer, "Error: %s: Command Cannot Load\n", line);
  textproc_log(buffer);
}

/* Output when starting an active or background process */
void log_start(int buf_id, int pid, int type, const char *cmd) {
  char buffer[BUFSIZE] = {0};
  const char* type_str = (type==LOG_ACTIVE) ? "Active" : "Background";
  sprintf(buffer, "Buffer %d, %s Process %d: %s Started\n", buf_id, type_str, pid, cmd);
  textproc_write(buffer);
} 

/* Output when the given buffer id is not found */
void log_buf_id_error(int buf_id) {
  char buffer[BUFSIZE] = {0};
  sprintf(buffer, "Error: Buffer ID #%d Not Found in Buffer List\n", buf_id);
  textproc_write(buffer);
}

/* Output when ctrl-c is received */
void log_ctrl_c() {
  textproc_write("Keyboard Combination control-c Received\n");
}

/* Output when ctrl-z is received */
void log_ctrl_z() {
  textproc_write("Keyboard Combination control-z Received\n");
}

/* Output when a signal is sent to a buffer's process */
void log_cmd_signal(int signal_type, int buf_id) {
  char buffer[BUFSIZE] = {0};
  static const char* sigs[] = {"Pause", "Resume", "Cancel"};
  if (signal_type < 0 || signal_type >= 3) {
	  textproc_write("Invalid input to log_cmd_signal\n");
	  return;
  }
  sprintf(buffer,"%s message sent to Buffer ID #%d\n", sigs[signal_type], buf_id);
  textproc_log(buffer);
}

/* Used when attempting to take action on a buffer which is in the wrong state */
void log_cmd_state_conflict(int buf_id, int state) {
  static const char *buffer_state[] = { "Ready", "Working", "Paused", NULL };
  char buffer[BUFSIZE] = {0};
  if (state < 0 || state >= 3) {
	  textproc_write("Invalid input to log_cmd_state_conflict\n");
	  return;
  }
  sprintf(buffer, "Buffer %d: cannoct act on buffer in state %s\n", buf_id, buffer_state[state]);
  textproc_log(buffer);
}

/* Output when a job changes state.
 * (Signal Handler Safe Outputting)
 */
void log_cmd_state(int pid, int type, const char *cmd, int transition) {
  char buffer[BUFSIZE] = {0};
  static const char* msgs[] = {"Terminated Normally", "Terminated by Signal", "Continued", "Stopped"};
  if (transition < 0 || transition >= 4) {
	  textproc_write("Invalid input to log_buf_state\n");
	  return;
  }
  sprintf(buffer,"%s Process %d: %s %s\n",(type==LOG_ACTIVE)?"Active":"Background", pid, cmd, msgs[transition]);
  textproc_write(buffer);
}

/* Output to list the buffer counts */
void log_buf_count(int num_bufs){
  char buffer[BUFSIZE] = {0};
  sprintf(buffer, "%d Buffer(s)\n", num_bufs);
  textproc_log(buffer);
}

/* Output to detail a single buffer */
void log_buf_details(int buf_id, int state, int pid, const char *cmd){
  static const char *buffer_state[] = { "Ready", "Working", "Paused", NULL };
  char buffer[BUFSIZE] = {0};
  if (state < 0 || state >= 3) {
	  textproc_write("Invalid input to log_buf_details\n");
	  return;
  }
  if (pid && cmd) sprintf(buffer, "Buffer %d: %s (Process %d; %s)\n", buf_id, buffer_state[state], pid, cmd);
  else sprintf(buffer, "Buffer %d: %s\n", buf_id, buffer_state[state]);
  textproc_log(buffer);
}
