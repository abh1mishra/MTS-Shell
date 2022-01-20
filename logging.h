/* Do not modify this file */
#ifndef LOGGING_H
#define LOGGING_H

#define LOG_ACTIVE     0
#define LOG_BACKGROUND 1

#define LOG_STATE_READY   0
#define LOG_STATE_WORKING 1
#define LOG_STATE_PAUSED  2

#define LOG_CMD_PAUSE  0
#define LOG_CMD_RESUME 1
#define LOG_CMD_CANCEL 2

#define LOG_FILE_OPEN_READ  0
#define LOG_FILE_OPEN_WRITE 1

#define LOG_CANCEL     0
#define LOG_CANCEL_SIG 1
#define LOG_RESUME     2
#define LOG_PAUSE      3

void log_prompt();
void log_help();
void log_quit();
void log_activate(int buf_id);
void log_show_active(int buf_id);
void log_print(int buf_id, const char *data);
void log_read(int buf_id, const char *file);
void log_write(int buf_id, const char *file);
void log_open(int buf_id);
void log_close(int buf_id);
void log_close_error(int buf_id);
void log_file_error(int error_type, const char *file);
void log_command_error(const char *line);
void log_start(int buf_id, int pid, int type, const char *cmd);
void log_ctrl_c();
void log_ctrl_z();
void log_buf_id_error(int buf_id);
void log_cmd_signal(int signal_type, int buf_id);
void log_cmd_state(int pid, int type, const char *cmd, int transition);
void log_cmd_state_conflict(int buf_id, int state);
void log_buf_count(int num_bufs);
void log_buf_details(int buf_id, int state, int pid, const char *cmd);

#endif /*LOGGING_H*/
