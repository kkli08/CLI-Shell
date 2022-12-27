#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/signal.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/resource.h>

int main()
{
    // define all the variables
    char UserInputLINE[LINE_LENGTH];
    int USER_TIME = 0;
    int SYS_TIME = 0;
    int Process_num = 0;
    int Acitve_Process_num = 0;
    struct Process ProcessTable[100];
    int QUIT = 0; // Quit shell program condition.

    signal(SIGCHLD, child_exit);
    do
    {
        // Command line prompt
        printf("SHELL379: ");
        fgets(UserInputLINE, LINE_LENGTH, stdin);

        // Identify the command
        if (UserInputLINE[0] != '\n')
        {
            UserInputLINE[strlen(UserInputLINE) - 1] = '\0';
            CommandHandler(UserInputLINE, &Process_num, ProcessTable, &Acitve_Process_num, &QUIT);
        }

        for (int i = 0; i < Process_num; i++)
        {
            // kill() to find terminate process
            // ProcessTable[i].S = 'T'; ->> T stands for terminated.
            if (kill(ProcessTable[i].PID, 0) != 0)
            {
                Acitve_Process_num--;
            }
        }
    } while (QUIT == 0 && Acitve_Process_num < MAX_PT_ENTRIES);

    // printf("End of the program...\n");
    exit(0);
    return 0;
}

void CommandHandler(char UserInputLINE[], int *Process_num, struct Process ProcessTable[], int *Acitve_Process_num, int *QUIT)
{
    // struct sigaction sa;
    // Variable declarations
    char *ARGS[MAX_ARGS];
    char Jobs[] = "jobs";       // [DONE]
    char Exit[] = "exit";       // [DONE]
    char Kill[] = "kill";       // [DONE]
    char Resume[] = "resume";   // [DONE]
    char Sleep[] = "sleep";     // [DONE]
    char Suspend[] = "suspend"; // [DONE]
    char Wait[] = "wait";       // [DONE]
                                // CMD [DONE]
    int length;
    length = strlen(UserInputLINE);
    char *InputLineForHandler = (char *)malloc(length * sizeof(char));

    // use strncpy to avoid pointer change
    strncpy(InputLineForHandler, UserInputLINE, length);

    // Extracting
    char *Tokens = strtok(UserInputLINE, " ");
    // The code which store each argument into an array
    // is cite from: https://stackoverflow.com/questions/15472299/split-string-into-tokens-and-save-them-in-an-array
    int i = 0;
    while (Tokens != NULL)
    {
        ARGS[i++] = Tokens;
        Tokens = strtok(NULL, " ");
    }
    ARGS[i] = NULL;
    // i will be the argc -> the # of the arguments.
    // including the last NULL args.

    // Main commands
    if (strcmp(Jobs, UserInputLINE) == 0)
    {
        JOBS_Handler(Process_num, ProcessTable, InputLineForHandler, Acitve_Process_num);
    }
    else if (strcmp(Exit, UserInputLINE) == 0)
    {
        EXIT_Handler(Process_num, ProcessTable, InputLineForHandler, Acitve_Process_num);
        int temp = *QUIT;
        temp++;
        *QUIT = temp;
    }
    else if (strcmp(Kill, ARGS[0]) == 0)
    {
        // -SIGTERM/-SIGKILL PID
        KILL_Handler(i, ARGS, Process_num, ProcessTable, InputLineForHandler, Acitve_Process_num);
    }
    else if (strcmp(Resume, ARGS[0]) == 0)
    {
        // -SIGCONT jobs_ID
        RESUME_Handler(i, ARGS, Process_num, ProcessTable, InputLineForHandler, Acitve_Process_num);
    }
    else if (strcmp(Sleep, ARGS[0]) == 0)
    {
        SLEEP_Handler(i, ARGS, Process_num, ProcessTable, InputLineForHandler, Acitve_Process_num);
    }
    else if (strcmp(Suspend, ARGS[0]) == 0)
    {
        // -SIGTSTP/-SIGSTOP PID
        SUSPEND_Handler(i, ARGS, Process_num, ProcessTable, InputLineForHandler, Acitve_Process_num);
    }
    else if (strcmp(Wait, ARGS[0]) == 0)
    {
        WAIT_Handler(i, ARGS, Process_num, ProcessTable, InputLineForHandler, Acitve_Process_num);
    }
    else
    {
        OtherCMD(i, ARGS, Process_num, ProcessTable, InputLineForHandler, Acitve_Process_num);
    }

    // // free memory allocation
    free(InputLineForHandler);
}

// void child_exit() is inspired by/cited:
// https://docs.oracle.com/cd/E19455-01/806-4750/signals-7/index.html
// recieve exit code for the children processes:)
void child_exit()
{
    int wstat;
    pid_t pid;

    while ((pid = wait3(&wstat, WNOHANG, (struct rusage *)NULL)) > 0)
        ;
}

// It is really fun to have this kind of practical programming experience:)
// Author : Ke Li
// CCID : kli1