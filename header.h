#include <sys/resource.h>
// Define all the constant
#define LINE_LENGTH 100   // Max # of characters in an input line
#define MAX_ARGS 7        // Max number of arguments to a command
#define MAX_LENGTH 20     // Max # of characters in an argument
#define MAX_PT_ENTRIES 32 // Max entries in the Process Table

struct Process
{
    int number;                // start from 0
    int PID;                   // (int) getpid()
    char S;                    // status 'R' for running  or 'S' for suspended
    char COMMAND[LINE_LENGTH]; // userinput line
};

// Function declarations
void child_exit();

void CommandHandler(char UserInputLINE[], int *Process_num, struct Process ProcessTable[], int *Acitve_Process_num, int *QUIT);
void JOBS_Handler(int *Process_num, struct Process ProcessTable[], char UserInputLINE[], int *Acitve_Process_num);
void EXIT_Handler(int *Process_num, struct Process ProcessTable[], char UserInputLINE[], int *Acitve_Process_num);
void KILL_Handler(int argc, char **ARGS, int *Process_num, struct Process ProcessTable[], char UserInputLINE[], int *Acitve_Process_num);
void RESUME_Handler(int argc, char **ARGS, int *Process_num, struct Process ProcessTable[], char UserInputLINE[], int *Acitve_Process_num);
void SLEEP_Handler(int argc, char **ARGS, int *Process_num, struct Process ProcessTable[], char UserInputLINE[], int *Acitve_Process_num);
void SUSPEND_Handler(int argc, char **ARGS, int *Process_num, struct Process ProcessTable[], char UserInputLINE[], int *Acitve_Process_num);
void WAIT_Handler(int argc, char **ARGS, int *Process_num, struct Process ProcessTable[], char UserInputLINE[], int *Acitve_Process_num);
void OtherCMD(int argc, char **ARGS, int *Process_num, struct Process ProcessTable[], char UserInputLINE[], int *Acitve_Process_num);
int SEC_calculation(char info_line[]);

// It is really fun to have this kind of practical programming experience:)
// Author : Ke Li
// CCID : kli1