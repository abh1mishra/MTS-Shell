

/* This is the only file you should update and submit. */

/* Fill in your Name and GNumber in the following two comment fields
 * Name:
 * GNumber:
 */

#include <sys/wait.h>
#include <sys/resource.h>
#include "textproc.h"
#include "parse.h"
#include "util.h"
#include "signal.h"

/* Constants */
#define DEBUG 1

static struct buffList* head;
static const char *textproc_path[] = { "./", "/usr/bin/", NULL };
static const char *instructions[] = { "quit", "help", "list", "new", "open", "write", "close", "print", "active", "pause", "resume", "cancel", "exec", NULL};
char currPath[]="./";
char binPath[]="/bin/";


struct buffList{
int id;
char *text;
int pid;
// const char* cmd;
int state;
int outPipe;
//states are 0,1,2 as following Ready Working,Paused
struct buffList* next;
};


/* The entry of your text processor program */
int createBuffers(struct buffList**,struct buffList**,struct buffList**,int*,char*);
void deleteBuffers(struct buffList**, struct buffList**,struct buffList**,int,int*);
void printList(struct buffList*,char*);
void Activate(struct buffList*,struct buffList**,int);
void Write_AAH(struct buffList*,int,int);
void AAHExec(struct buffList*,char*[],int,int,char*);
void cancel_AAH(struct buffList*,int);



// void sigchild_handler(int signum){
// int status;
// printf("stopped\n");
// 	struct buffList* n=head;
// 	while (n != NULL) {
// 			if(n->state>0){
// 				printf("pid is %d\n",n->pid );
// 				pid_t pid=waitpid(n->pid,&status,WUNTRACED|WNOHANG|WCONTINUED);
// 				if(pid>0){
// 				if(WIFSTOPPED(status) !=0){
// 					n->state=2;
// 					printf("paused it\n");
// 					goto endhere;
// 				}
// 				if(WIFCONTINUED(status) !=0){
// 					n->state=1;
// 					printf("started status is %d\n",WIFCONTINUED(status));
// 					goto endhere;
// 				}
// 					if(WIFEXITED(status)!=0){
// 						printf("ended good\n");
// 						n->pid=-1;
// 						n->state=0;
// 					}
// 	if(WIFEXITED(status)==0){
// 		printf("didnot terminate properly %d %d\n",pid,n->id );
// 		if(pid>0){
// 			printf("terminated\n");
// 			n->pid=-1;
// 			n->state=0;
// 		}
// 	}}
// 			}
// 			endhere:
// 			n = n->next;
// 	}
// }
void sigchild_handler(int signum){
int status;
pid_t pid;
// printf("stopped\n"); get handle of head of buffer list
	struct buffList* n=head;
	//loop through processes to reap when signal reaches
	while (n != NULL) {
		//checking only processes other than ready
			if(n->state>0){
				// printf("pid is %d\n",n->pid ); options to check resume, and stopped and wont wait till any processes changes
				pid=waitpid(n->pid,&status,WUNTRACED|WNOHANG|WCONTINUED);
				//if pid 0 means no change in status
				if(pid>0){
					//if stop signalled
				if(WIFSTOPPED(status) !=0){
					n->state=2;
					printf("paused it\n");
				}
									//if resume signalled
				else if(WIFCONTINUED(status) !=0){
					n->state=1;
					printf("started status is %d\n",WIFCONTINUED(status));
				}
									//if exited normally signalled
					else if(WIFEXITED(status)!=0){
						//get input from reaped process in buffer
						printf("ended good\n");
						n->text=fd_to_text(n->outPipe);
						close(n->outPipe);
						n->pid=-1;
						n->state=0;
					}
										//if exited abruptly
					else if(WIFEXITED(status)==0){
							printf("terminated\n");
							n->pid=-1;
							n->state=0;
					}
}
			}
			n = n->next;
	}
}


void printList(struct buffList* n,char* cmd)
{
    while (n != NULL) {
        log_buf_details(n->id,n->state,n->pid,cmd);
        n = n->next;
    }
}
// cancel, resume and stop have same structure, loop till finding required id and then signal with KILL CONT or STOP
void cancel_AAH(struct buffList* n,int bufId)
{
    int i=0;
    while (n != NULL) {
        if(n->id==bufId){
          i=1;
          if((n->state)==0){log_cmd_state_conflict(n->state,bufId);break;}
          log_cmd_signal(LOG_CMD_CANCEL,bufId);
          kill(n->pid,SIGKILL);
          break;
        }
        n = n->next;
    }
    if(i==0)
      log_buf_id_error(bufId);
}

void pause_AAH(struct buffList* n,int bufId)
{
    int i=0;
    while (n != NULL) {
        if(n->id==bufId){
          i=1;
          if((n->state)==0){log_cmd_state_conflict(n->state,bufId);break;}
          log_cmd_signal(LOG_CMD_PAUSE,bufId);
          kill(n->pid,SIGSTOP);
          break;
        }
        n = n->next;
    }
    if(i==0)
      log_buf_id_error(bufId);
}
void resume_AAH(struct buffList* n,int bufId)
{
    int i=0;
    while (n != NULL) {
        if(n->id==bufId){
          i=1;
          if((n->state)==0){log_cmd_state_conflict(n->state,bufId);break;}
          log_cmd_signal(LOG_CMD_RESUME,bufId);
          kill(n->pid,SIGCONT);
          break;
        }
        n = n->next;
    }
    if(i==0)
      log_buf_id_error(bufId);
}


//crucial function


void AAHExec(struct buffList* n,char* argv[],  int active_check,int bufId,char* cmd){
  int i=0;
	//loop till getting required id
  while (n != NULL) {
      if(n->id==bufId){
				//check if ready
        (n->state==0)?({i=1;}):(i=2);
        break;
      }
      n = n->next;
  }
	//if ready
  if(i==1){
              char path[100];
              int j=0;
							//search in bin flder
              snprintf(path,sizeof(argv[0])+sizeof(binPath),"%s%s",binPath,argv[0]);
              if( access(path, F_OK ) == 0 ) {
                j=1;
                goto here;
            }
						//search in current folder
              snprintf(path,sizeof(argv[0])+sizeof(currPath),"%s%s",currPath,argv[0]);
              if(access(path, F_OK ) == 0){
                j=1;
               }
               here:
               if(j==1){
								 // printf("kolo\n");
//create pipes
                 int inPipe[2];
                 int outPipe[2];
                 pipe(inPipe);
                 pipe(outPipe);
                 int pid;
                 //fork a child
								 // signal hander for SIGCHLD
								 struct sigaction new;
								 struct sigaction old;
								 memset(&new,0,sizeof(new));
								 new.sa_handler=sigchild_handler;
								 sigaction(SIGCHLD,&new,&old);
								 // block SIGCHLD
								 sigset_t x;
								 sigemptyset (&x);
								 sigaddset(&x, SIGCHLD);
								 sigprocmask(SIG_BLOCK, &x, NULL);
                 if((pid=fork())>0){
									 //parent process
									 n->state=1;
                   n->pid=pid;
									 // printf("pid is %d\n",pid );
                   log_start(bufId,pid,LOG_ACTIVE,cmd);
                   close(inPipe[0]);
                   close(outPipe[1]);
									 //write to child process
                   text_to_fd(n->text,inPipe[1]);
                   close(inPipe[1]);
									 //store n outpipe in buffer
                   n->outPipe=outPipe[0];
                   //for active process
                   if(active_check==1){
                     n->text=fd_to_text(outPipe[0]);
                     close(outPipe[0]);
                     int status;
										 //reap active process
                     wait(&status);
                     n->pid=0;
                     n->outPipe=-1;
										 n->state=0;

										 // printf("hello\n");

                     if(status<0){
                       log_command_error(cmd);
                     }
                   }
									 //unblock here
									 sigprocmask(SIG_UNBLOCK, &x, NULL);

                 }

                 else{
									 //child process
									 setpgid(0,0);
                   close(inPipe[1]);
                   close(outPipe[0]);
									 //replace stander input and output with custom pipes
                   dup2(inPipe[0],STDIN_FILENO);
                   dup2(outPipe[1],STDOUT_FILENO);
									 //run program here
                   execv(path,argv);
                   close(inPipe[0]);
                   close(outPipe[1]);
                   exit(5);
                 }

               }
               else{
                 // printf("Non\n" );
               }
             }
             else if(i==2){
               //busy
               log_cmd_state_conflict(bufId,n->state);
             }
             else{
               log_buf_id_error(bufId);
             }
}







void Activate(struct buffList* n,struct buffList** active,int bufId)
{
    int i=0;
		//loop till you find id
    while (n != NULL) {
        if(n->id==bufId){
          i=1;
          if((n->state)!=0){
            //if not ready reap and than activate
            n->text=fd_to_text(n->outPipe);
            close(n->outPipe);
            int status;
            waitpid(n->pid,&status,0);
            n->pid=0;
            n->outPipe=-1;
            if(status<0){
              log_command_error("exec");
            }
            n->state=0;
            *active=n;
          }
          log_activate(bufId);
					*active=n;
          break;
        }
        n = n->next;
    }
    if(i==0)
      log_buf_id_error(bufId);
}

//both write and print have same functionality, loop till you get id and then use given functions to do rest or log
void Write_AAH(struct buffList* n,int fd,int bufId)
{
    int i=0;
    while (n != NULL) {
        if(n->id==bufId){
          i=1;
          text_to_fd(n->text,fd);
          break;
        }
        n = n->next;
    }
    if(i==0)
      log_buf_id_error(bufId);
}
void print_AAH(struct buffList* n,int bufId)
{
    int i=0;
    while (n != NULL) {
        if(n->id==bufId){
          i=1;
          log_print(bufId,n->text);
          break;
        }
        n = n->next;
    }
    if(i==0)
      log_buf_id_error(bufId);
}

int createBuffers(struct buffList** head,struct buffList** tail,struct buffList** active,int* noOfBuffers,char* str){
struct buffList* now=(struct buffList*)malloc(sizeof(struct buffList));
int bufId;
if(*tail==NULL){
bufId=1;
*head=now;
}
else{
bufId=(((*tail)->id)+1);
(*tail)-> next=now;
}
*tail=now;
*active=now;
now->id=bufId;
now->state=0;
now->text="";
now->outPipe=-1;
// now->cmd=NULL;
if(str){
now->text=str;
}
now->pid=-1;//initially -1 . -1 means no process alotted
now-> next=NULL;
//update buffer length
(*noOfBuffers)++;
log_open(bufId);
log_activate(bufId);
return(bufId);
}

void deleteBuffers(struct buffList** head_ref, struct buffList** tail,struct buffList** active,int bufId,int* noOfBuffers)
{
  if(bufId<0){
    log_buf_id_error(bufId);
    return;
  }
	// Store head node
	struct buffList *temp = *head_ref, *prev;

	// If head node itself holds the key to be deleted
	if (temp != NULL && temp->id == bufId) {
    if(temp->state!=0){
      log_close_error(bufId);
      return;
    }
		*head_ref = temp->next; // Changed head
    (*noOfBuffers)--;
    //if tail and head coincides
    if(temp->next==NULL){
      *tail=NULL;
    }
    //if head is the active one
    if((temp->id)==((*active)->id)){
      *active=*tail;
    }
    log_close(bufId);
    free(temp); // free old head
		return;
	}

	// Search for the key to be deleted, keep track of the
	// previous node as we need to change 'prev->next'
	while (temp != NULL && temp->id != bufId) {
		prev = temp;
		temp = temp->next;
	}

	// If key was not present in linked list
	if (temp == NULL)
  {
    log_buf_id_error(bufId);
		return;
  }

	// Unlink the node from linked list
  if(temp->state!=0){
    log_close_error(bufId);
    return;
  }
  if(temp->next==NULL){
    *tail=prev;
  }
  if((temp->id)==((*active)->id)){
    *active=*tail;
  }
  (*noOfBuffers)--;
	prev->next = temp->next;
  log_close(bufId);

	free(temp); // Free memory
}

void sigint_handler(){
	signal(SIGINT,sigint_handler);

	struct buffList* n=head;
	while (n != NULL) {
			if(n->state>0){
				kill(n->pid,SIGKILL);
			}
			n = n->next;
			// printf("see %s\n",n->text);
	}
}
void sigtstp_handler(){
	signal(SIGTSTP,sigtstp_handler);
	struct buffList* n=head;
	while (n != NULL) {
			if(n->state>0){
				kill(n->pid,SIGTSTP);
			}
			n = n->next;
			// printf("see %s\n",n->text);
	}
}
int main() {
    char cmdline[MAXLINE];        /* Command line */
    char *cmd = NULL;
		//CTRL-C ,CTRL-Z handlers
		signal(SIGINT,sigint_handler);
		signal(SIGTSTP,sigtstp_handler);

    head=NULL;
    struct buffList* active=NULL;
    struct buffList* tail=NULL;
    int noOfBuffers=0;

    /* Intial Prompt and Welcome */
    log_help();

    /* Shell looping here to accept user command and execute */
    while (1) {
        char *argv[MAXARGS];        /* Argument list */
        Instruction inst;           /* Instruction structure: check parse.h */

        /* Print prompt */
        log_prompt();

        /* Read a line */
        // note: fgets will keep the ending '\n'
        if (fgets(cmdline, MAXLINE, stdin) == NULL) {
            if (errno == EINTR) {
                continue;
            }
            exit(-1);
        }

        if (feof(stdin)) {  /* ctrl-d will exit text processor */
          // exit(0);
        }

        /* Parse command line */
        if (strlen(cmdline)==1)   /* empty cmd line will be ignored */
          continue;

        cmdline[strlen(cmdline) - 1] = '\0';        /* remove trailing '\n' */

        cmd = malloc(strlen(cmdline) + 1);
        snprintf(cmd, strlen(cmdline) + 1, "%s", cmdline);

        /* Bail if command is only whitespace */
        if(!is_whitespace(cmd)) {
            initialize_command(&inst, argv);    /* initialize arg lists and instruction */
            parse(cmd, &inst, argv);            /* call provided parse() */

            if (DEBUG) {  /* display parse result, redefine DEBUG to turn it off */
                debug_print_parse(cmd, &inst, argv, "main (after parse)");
	    }

            /* After parsing: your code to continue from here */
            /*================================================*/
						//catch commands

						//help command
            if(strcmp(inst.instruct,instructions[1])==0)
              log_help();
            if(strcmp(inst.instruct,instructions[0])==0){
              log_quit();
              exit(0);
            }

//list command
            if(strcmp(inst.instruct,instructions[2])==0){
              if(noOfBuffers==0){
                log_show_active(0);
              }
              else{
              log_buf_count(noOfBuffers);
              printList(head,cmd);
              printf("Active buffer is %d\n",active->id );
            }
            }

						//activate command

            if(strcmp(inst.instruct,instructions[8])==0){
              if(noOfBuffers==0){
                log_show_active(0);
              }
              else{
                int bufId=active->id;
                if(inst.id){
                  bufId=inst.id;
                }
                Activate(head,&active,bufId);
                printf("Active buffer is %d\n",active->id );
            }
            }

//delete buffer command

            if(strcmp(inst.instruct,instructions[6])==0)
            {
              int bufId=active->id;
              if(inst.id){
                bufId=inst.id;
              }
              deleteBuffers(&head,&tail,&active,bufId,&noOfBuffers);
            }

// open file command
            if(strcmp(inst.instruct,instructions[4])==0)
              {
                if(inst.file){
                int fd=open(inst.file,O_RDONLY,0600);
                if(fd<0){
                  log_file_error(LOG_FILE_OPEN_READ,inst.file);
                  break;
                }
                char *str=fd_to_text(fd);
                close(fd);
                log_read(createBuffers(&head,&tail,&active,&noOfBuffers,str),inst.file);

              }
              }

//write to file from buffer command

              if(strcmp(inst.instruct,instructions[5])==0){
                if(inst.file){
                int fd=open(inst.file,O_WRONLY | O_CREAT | O_TRUNC,0600);
                if(fd<0){
                  log_file_error(LOG_FILE_OPEN_READ,inst.file);
                  break;
                }
                int bufId=active->id;
                if(inst.id){
                  bufId=inst.id;
                }
                Write_AAH(head,fd,bufId);
                close(fd);
              }
            }

//print buffer command

            if(strcmp(inst.instruct,instructions[7])==0){
              int bufId=active->id;
              if(inst.id){
                bufId=inst.id;
              }
              print_AAH(head,bufId);

          }

//cancel a process command

          if(strcmp(inst.instruct,instructions[11])==0){
            int bufId=active->id;
            if(inst.id){
              bufId=inst.id;
            }
            cancel_AAH(head,bufId);
        }

//pause process command

				if(strcmp(inst.instruct,instructions[9])==0){
					int bufId=active->id;
					if(inst.id){
						bufId=inst.id;
					}
					pause_AAH(head,bufId);

			}

//resume process command

			if(strcmp(inst.instruct,instructions[10])==0){
				int bufId=active->id;
				if(inst.id){
					bufId=inst.id;
				}
				resume_AAH(head,bufId);

		}
//exec shellcall called
          if(strcmp(inst.instruct,instructions[12])==0){
            int bufId=active->id;
            if(inst.id){
              bufId=inst.id;
            }
            int active_check;
            (bufId==active->id)?(active_check=1):(active_check=0);
            AAHExec(head,argv,active_check,bufId,cmd);
        }

//new and new* command

            if(strcmp(inst.instruct,instructions[3])==0){
              createBuffers(&head,&tail,&active,&noOfBuffers,NULL);
              if(*argv){
                char** p=argv;
                // p++;
                int bufId=active->id;
                AAHExec(head,p,1,bufId,cmd);
            }
            }
        }

        free_command(&inst, argv);
    }
    return 0;
}

*****************************************************************************************************************************************




// /* This is the only file you should update and submit. */
//
// /* Fill in your Name and GNumber in the following two comment fields
//  * Name:
//  * GNumber:
//  */
//
// #include <sys/wait.h>
// #include <sys/types.h>
// #include "textproc.h"
// #include "parse.h"
// #include "util.h"
// #include <signal.h>
// #include <limits.h>
//
//
//
//
//
//
// /* Constants */
// #define DEBUG 1
// static struct Stack* stack;
//
// static const char *textproc_path[] = { "./", "/usr/bin/", NULL };
// static const char *instructions[] = { "quit", "help", "list", "new", "open", "write", "close", "print", "active", "pause", "resume", "cancel", "exec", NULL};
// char currPath[]="./";
// char binPath[]="/bin/";
//
// struct buffList{
// int id;
// char *text;
// int pid;
// // const char* cmd;
// int state;
// int outPipe;
// //states are 0,1,2 as following Ready Working,Paused
// struct buffList* next;
// };
//
//
//
//
// struct Stack {
// 	int top;
// 	unsigned capacity;
// 	struct buffList *array[100];
// };
//
// // function to create a stack of given capacity. It initializes size of
// // stack as 0
// struct Stack* createStack()
// {
// 	struct Stack* stack = (struct Stack*)malloc(sizeof(struct Stack));
// 	stack->capacity = 100;
// 	stack->top = -1;
// 	return stack;
// }
//
// // Stack is full when top is equal to the last index
// int isFull(struct Stack* stack)
// {
// 	return stack->top == stack->capacity - 1;
// }
//
// // Stack is empty when top is equal to -1
// int isEmpty(struct Stack* stack)
// {
// 	return stack->top == -1;
// }
//
// // Function to add an item to stack. It increases top by 1
// void push(struct Stack* stack, struct buffList* item)
// {
// 	if (isFull(stack))
// 		return;
// 	stack->array[++stack->top] = item;
// 	printf("%d\n",item->id );
// }
//
// // Function to remove an item from stack. It decreases top by 1
// struct buffList* pop(struct Stack* stack)
// {
// 	if (isEmpty(stack))
// 		return NULL;
// 	return stack->array[stack->top--];
// }
//
// // Function to return the top from stack without removing it
// struct buffList* peek(struct Stack* stack)
// {
// 	if (isEmpty(stack))
// 		return NULL;
// 	return stack->array[stack->top];
// }
//
//
//
//
//
//
//
//
//
//
//
//
//
// void sigint_handler(int signum) { //Handler for SIGINT
//    //Reset handler to catch SIGINT next time.
//    signal(SIGINT, sigint_handler);
//    printf("Cannopusht be stopped using Ctrl+C \n");
//    fflush(stdout);
// }
// void sigchild_handler(int signum) { //Handler for SIGINT
//    //Reset handler to catch SIGINT next time.
//    // signal(SIGCHILD, sigchild_handler);
// 	 struct buffList* k=pop(stack);
// 	 printf("%d\n", k->id);
//    fflush(stdout);
// }
//
//
// void sigcontchild_handler(int signum) { //Handler for SIGINT
//    //Reset handler to catch SIGINT next time.
//    signal(SIGCONT, sigint_handler);
//    log_cmd_state(1, LOG_BACKGROUND,"will see", LOG_RESUME);
//    fflush(stdout);
// }
// void sigtstpchild_handler(int signum) { //Handler for SIGINT
//    //Reset handler to catch SIGINT next time.
//    signal(SIGTSTP, sigtstpchild_handler);
//    printf("Cannot be stopped using Ctrl+C \n");
//    fflush(stdout);
// }
// void sigkillchild_handler(int signum) { //Handler for SIGINT
//    //Reset handler to catch SIGINT next time.
//    signal(SIGKILL, sigkillchild_handler);
//    printf("Cannot be stopped using Ctrl+C \n");
//    fflush(stdout);
// }
//
//
//
//
// /* The entry of your text processor program */
// int createBuffers(struct buffList**,struct buffList**,struct buffList**,int*,char*);
// void deleteBuffers(struct buffList**, struct buffList**,struct buffList**,int,int*);
// void printList(struct buffList*,char*);
// void Activate(struct buffList*,struct buffList**,int);
// void Write_AAH(struct buffList*,int,int);
// void AAHExec(struct buffList*,char*[],struct Stack*,int,int,char*);
// void cancel_AAH(struct buffList*,int);
// void pause_AAH(struct buffList*,int);
// void resume_AAH(struct buffList*,int);
//
//
// void printList(struct buffList* n,char* cmd)
// {
//     while (n != NULL) {
//         log_buf_details(n->id,n->state,n->pid,cmd);
//         n = n->next;
//         // printf("see %s\n",n->text);
//     }
// }
//
//
//
//
//
//
//
//
//
//
//
//
// void AAHExec(struct buffList* n,char* argv[],  struct Stack* stack,int active_check,int bufId,char* cmd){
//   int i=0;
//   while (n != NULL) {
//       if(n->id==bufId){
//         (n->state==0)?({i=1;n->state=1;}):(i=2);
//         break;
//       }
//       n = n->next;
//   }
//   if(i==1){
//               char path[100];
//               int j=0;
//               snprintf(path,sizeof(argv[0])+sizeof(binPath),"%s%s",binPath,argv[0]);
//               if( access(path, F_OK ) == 0 ) {
//                 j=1;
//                 goto here;
//             }
//               snprintf(path,sizeof(argv[0])+sizeof(currPath),"%s%s",currPath,argv[0]);
//               if(access(path, F_OK ) == 0){
//                 j=1;
//                }
//                here:
//                if(j==1){
//                  int inPipe[2];
//                  int outPipe[2];
//                  pipe(inPipe);
//                  pipe(outPipe);
//                  int pid,ppid;
//                  ppid=getpid();
//                  //fork a child
//                  if((pid=fork())>0){
//                    signal(SIGCHLD,sigchild_handler);
//                    n->pid=pid;
//                    log_start(bufId,pid,LOG_ACTIVE,cmd);
//                    close(inPipe[0]);
//                    close(outPipe[1]);
//                    text_to_fd(n->text,inPipe[1]);
//                    close(inPipe[1]);
//                    n->outPipe=outPipe[0];
//                    //for active process
//                    if(active_check==1){
//                      n->text=fd_to_text(outPipe[0]);
//                      close(outPipe[0]);
//                      int status;
//                      wait(&status);
//                      n->pid=0;
//                      n->outPipe=-1;
//                      if(status<0){
//                        log_command_error(cmd);
//                      }
//                      (WIFSIGNALED(status))?(printf("kol")):(printf("lol" ));
//                      n->state=0;
//                    }
//                    printf("hello\n");
//                  }
//                  else{
//                    // signal(SIGCONT,sigcontchild_handler);
//                    // signal(SIGTSTP,sigtstpchild_handler);
//                    // signal(SIGKILL,sigkillchild_handler);
//                    close(inPipe[1]);
//                    close(outPipe[0]);
//                    dup2(inPipe[0],STDIN_FILENO);
//                    dup2(outPipe[1],STDOUT_FILENO);
// 									 printf("lolo\n");
//
//                    execv(path,argv);
//                    close(inPipe[0]);
//                    close(outPipe[1]);
//                    printf("code\n" );
//                    push(stack,n);
//                    kill(ppid,SIGCHLD);
//                    exit(0);
//                  }
//                }
//                else{
//                  printf("Non\n" );
//                }
//              }
//              else if(i==2){
//                //busy
//                log_cmd_state_conflict(bufId,n->state);
//              }
//              else{
//                log_buf_id_error(bufId);
//              }
// }
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
// void Activate(struct buffList* n,struct buffList** active,int bufId)
// {
//     int i=0;
//     while (n != NULL) {
//         if(n->id==bufId){
//           i=1;
//           *active=n;
//           if((n->state)==1){
//             n->text=fd_to_text(n->outPipe);
//             close(n->outPipe);
//             int status;
//             waitpid(n->pid,&status,0);
//             n->pid=0;
//             n->outPipe=-1;
//             if(status<0){
//               log_command_error("exec");
//             }
//             n->state=0;
//           }
//           log_activate(bufId);
//           break;
//         }
//         n = n->next;
//     }
//     if(i==0)
//       log_buf_id_error(bufId);
// }
// void cancel_AAH(struct buffList* n,int bufId)
// {
//     int i=0;
//     while (n != NULL) {
//         if(n->id==bufId){
//           i=1;
//           if((n->state)==0){log_cmd_state_conflict(n->state,bufId);}
//           log_cmd_signal(LOG_CMD_CANCEL,bufId);
//           kill(n->pid,SIGKILL);
//           n->pid=0;
//           break;
//         }
//         n = n->next;
//     }
//     if(i==0)
//       log_buf_id_error(bufId);
// }
// void pause_AAH(struct buffList* n,int bufId)
// {
//     int i=0;
//     while (n != NULL) {
//         if(n->id==bufId){
//           i=1;
//           if((n->state)==0){log_cmd_state_conflict(n->state,bufId);}
//           log_cmd_signal(LOG_CMD_PAUSE,bufId);
//           kill(n->pid,SIGTSTP);
//           n->state=2;
//           break;
//         }
//         n = n->next;
//     }
//     if(i==0)
//       log_buf_id_error(bufId);
// }
// void resume_AAH(struct buffList* n,int bufId)
// {
//     int i=0;
//     while (n != NULL) {
//         if(n->id==bufId){
//           i=1;
//           if((n->state)==0){log_cmd_state_conflict(n->state,bufId);}
//           // if(((n->state)==1) &((*active)->id==n->id)){
//           //   n->text=fd_to_text(n->outPipe);
//           //   close(n->outPipe);
//           //   int status;
//           //   waitpid(n->pid,&status,0);
//           //   n->pid=0;
//           //   n->outPipe=-1;
//           //   if(status<0){
//           //     log_command_error("exec");
//           //   }
//           //   n->state=0;
//           // }
//           log_cmd_signal(LOG_CMD_RESUME,bufId);
//           kill(n->pid,SIGCONT);
//           n->state=1;
//           break;
//         }
//         n = n->next;
//     }
//     if(i==0)
//       log_buf_id_error(bufId);
// }
// void Write_AAH(struct buffList* n,int fd,int bufId)
// {
//     int i=0;
//     while (n != NULL) {
//         if(n->id==bufId){
//           i=1;
//           text_to_fd(n->text,fd);
//           break;
//         }
//         n = n->next;
//     }
//     if(i==0)
//       log_buf_id_error(bufId);
// }
// void print_AAH(struct buffList* n,int bufId)
// {
//     int i=0;
//     while (n != NULL) {
//         if(n->id==bufId){
//           i=1;
//           log_print(bufId,n->text);
//           break;
//         }
//         n = n->next;
//     }
//     if(i==0)
//       log_buf_id_error(bufId);
// }
//
// int createBuffers(struct buffList** head,struct buffList** tail,struct buffList** active,int* noOfBuffers,char* str){
// struct buffList* now=(struct buffList*)malloc(sizeof(struct buffList));
// int bufId;
// if(*tail==NULL){
// bufId=1;
// *head=now;
// }
// else{
// bufId=(((*tail)->id)+1);
// (*tail)-> next=now;
// }
// *tail=now;
// *active=now;
// now->id=bufId;
// now->state=0;
// now->text="";
// now->outPipe=-1;
// // now->cmd=NULL;
// if(str){
// now->text=str;
// }
// now->pid=-1;//initially -1 . -1 means no process alotted
// now-> next=NULL;
// (*noOfBuffers)++;
// log_open(bufId);
// log_activate(bufId);
// return(bufId);
// }
//
// void deleteBuffers(struct buffList** head_ref, struct buffList** tail,struct buffList** active,int bufId,int* noOfBuffers)
// {
//   if(bufId<0){
//     log_buf_id_error(bufId);
//     return;
//   }
// 	// Store head node
// 	struct buffList *temp = *head_ref, *prev;
//
// 	// If head node itself holds the key to be deleted
// 	if (temp != NULL && temp->id == bufId) {
//     if(temp->state!=0){
//       log_close_error(bufId);
//       return;
//     }
// 		*head_ref = temp->next; // Changed head
//     (*noOfBuffers)--;
//     //if tail and head coincides
//     if(temp->next==NULL){
//       *tail=NULL;
//     }
//     //if head is the active one
//     if((temp->id)==((*active)->id)){
//       *active=*tail;
//     }
//     log_close(bufId);
//     free(temp); // free old head
// 		return;
// 	}
//
// 	// Search for the key to be deleted, keep track of the
// 	// previous node as we need to change 'prev->next'
// 	while (temp != NULL && temp->id != bufId) {
// 		prev = temp;
// 		temp = temp->next;
// 	}
//
// 	// If key was not present in linked list
// 	if (temp == NULL)
//   {
//     log_buf_id_error(bufId);
// 		return;
//   }
//
// 	// Unlink the node from linked list
//   if(temp->state!=0){
//     log_close_error(bufId);
//     return;
//   }
//   if(temp->next==NULL){
//     *tail=prev;
//   }
//   if((temp->id)==((*active)->id)){
//     *active=*tail;
//   }
//   (*noOfBuffers)--;
// 	prev->next = temp->next;
//   log_close(bufId);
//
// 	free(temp); // Free memory
// }
//
//
//
// int main() {
//     char cmdline[MAXLINE];        /* Command line */
//     char *cmd = NULL;
//     struct buffList* head=NULL;
//     struct buffList* active=NULL;
//     struct buffList* tail=NULL;
//     stack = createStack(100);
//     int noOfBuffers=0;
//     signal(SIGINT,sigint_handler);
//     /* Intial Prompt and Welcome */
//     log_help();
//
//     /* Shell looping here to accept user command and execute */
//     while (1) {
//         char *argv[MAXARGS];        /* Argument list */
//         Instruction inst;           /* Instruction structure: check parse.h */
//
//         /* Print prompt */
//         log_prompt();
//
//         /* Read a line */
//         // note: fgets will keep the ending '\n'
//         if (fgets(cmdline, MAXLINE, stdin) == NULL) {
//             if (errno == EINTR) {
//                 continue;
//             }
//             exit(-1);
//         }
//
//         if (feof(stdin)) {  /* ctrl-d will exit text processor */
//           printf("kolo\n");
//           exit(0);
//         }
//
//         /* Parse command line */
//         if (strlen(cmdline)==1)   /* empty cmd line will be ignored */
//           continue;
//
//         cmdline[strlen(cmdline) - 1] = '\0';        /* remove trailing '\n' */
//
//         cmd = malloc(strlen(cmdline) + 1);
//         snprintf(cmd, strlen(cmdline) + 1, "%s", cmdline);
//
//         /* Bail if command is only whitespace */
//         if(!is_whitespace(cmd)) {
//             initialize_command(&inst, argv);    /* initialize arg lists and instruction */
//             parse(cmd, &inst, argv);            /* call provided parse() */
//
//             if (DEBUG) {  /* display parse result, redefine DEBUG to turn it off */
//                 debug_print_parse(cmd, &inst, argv, "main (after parse)");
// 	    }
//
//             /* After parsing: your code to continue from here */
//             /*================================================*/
//             if(strcmp(inst.instruct,instructions[1])==0)
//               log_help();
//             if(strcmp(inst.instruct,instructions[0])==0){
//               log_quit();
//               exit(0);
//             }
//             if(strcmp(inst.instruct,instructions[2])==0){
//               if(noOfBuffers==0){
//                 log_show_active(0);
//               }
//               else{
//               log_buf_count(noOfBuffers);
//               printList(head,cmd);
//               printf("Active buffer is %d\n",active->id );
//             }
//             }
//             if(strcmp(inst.instruct,instructions[8])==0){
//               if(noOfBuffers==0){
//                 log_show_active(0);
//               }
//               else{
//                 int bufId=active->id;
//                 if(inst.id){
//                   bufId=inst.id;
//                 }
//                 Activate(head,&active,bufId);
//                 printf("Active buffer is %d\n",active->id );
//             }
//             }
//             if(strcmp(inst.instruct,instructions[6])==0)
//             {
//               int bufId=active->id;
//               if(inst.id){
//                 bufId=inst.id;
//               }
//               deleteBuffers(&head,&tail,&active,bufId,&noOfBuffers);
//             }
//             if(strcmp(inst.instruct,instructions[4])==0)
//               {
//                 if(inst.file){
//                 int fd=open(inst.file,O_RDONLY,0600);
//                 if(fd<0){
//                   log_file_error(LOG_FILE_OPEN_READ,inst.file);
//                   break;
//                 }
//                 char *str=fd_to_text(fd);
//                 close(fd);
//                 log_read(createBuffers(&head,&tail,&active,&noOfBuffers,str),inst.file);
//
//               }
//
//               }
//               if(strcmp(inst.instruct,instructions[5])==0){
//                 if(inst.file){
//                 int fd=open(inst.file,O_WRONLY | O_CREAT | O_TRUNC,0600);
//                 if(fd<0){
//                   log_file_error(LOG_FILE_OPEN_READ,inst.file);
//                   break;
//                 }
//                 int bufId=active->id;
//                 if(inst.id){
//                   bufId=inst.id;
//                 }
//                 Write_AAH(head,fd,bufId);
//                 close(fd);
//               }
//             }
//             if(strcmp(inst.instruct,instructions[7])==0){
//               int bufId=active->id;
//               if(inst.id){
//                 bufId=inst.id;
//               }
//               print_AAH(head,bufId);
//           }
//           if(strcmp(inst.instruct,instructions[11])==0){
//             int bufId=active->id;
//             if(inst.id){
//               bufId=inst.id;
//             }
//             cancel_AAH(head,bufId);
//
//         }
// //exec shellcall called
//           if(strcmp(inst.instruct,instructions[12])==0){
//             int bufId=active->id;
//             if(inst.id){
//               bufId=inst.id;
//             }
//             int active_check;
//             (bufId==active->id)?(active_check=1):(active_check=0);
//             AAHExec(head,argv,stack,active_check,bufId,cmd);
//         }
//             if(strcmp(inst.instruct,instructions[3])==0){
//               createBuffers(&head,&tail,&active,&noOfBuffers,NULL);
//               if(*argv){
//                 char** p=argv;
//                 // p++;
//                 int bufId=active->id;
//                 AAHExec(head,p,stack,1,bufId,cmd);
//             }
//             }
//         }
//
//         free_command(&inst, argv);
//     }
//     return 0;
// }
