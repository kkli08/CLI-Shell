#include "header.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/signal.h>
#include <signal.h>
#include <sys/time.h>
// Command : jobs
// Display the status of all running processes spawned by shell379. See
// the print format below in the example.
void JOBS_Handler(int *Process_num, struct Process ProcessTable[], char UserInputLINE[], int *Acitve_Process_num)
{
    // print process table information
    printf("\nRunning processes:\n");
    int sort = 0;
    int temp_num = *Process_num;
    printf("#      PID S SEC COMMAND\n");
    for (int i = 0; i < temp_num; i++)
    {
        // kill() to find terminate process
        if (kill(ProcessTable[i].PID, 0) != 0)
        {
            ProcessTable[i].S = 'T'; // T stands for terminated.
        }
        else if (ProcessTable[i].S == 'R' || ProcessTable[i].S == 'S')
        {
            // ---------------------------------------------------
            int SEC = 0;
            char ps_command[] = "ps -p ";
            int int_pid = ProcessTable[i].PID;
            char str_pid[10];
            // get string pid
            snprintf(str_pid, 10, "%d", int_pid);

            // get string ps -p <pid>
            int com_len, ps_len;
            ps_len = strlen(ps_command);
            com_len = strlen(ps_command) + strlen(str_pid);
            char *pid_command = (char *)malloc(com_len);
            strncat(pid_command, ps_command, ps_len);
            strncat(pid_command, str_pid, com_len);

            // printf("popen command: %s\n", pid_command);
            // popen
            char buf[1024];
            FILE *p_file = NULL;
            char *line = NULL;
            size_t len = 0;
            ssize_t read;

            p_file = popen(pid_command, "r");

            // free memory allocation
            free(pid_command);

            if (!p_file)
            {
                fprintf(stderr, "error to popen");
            }

            // read the "ps -p <pid>" output
            // cited:
            // https://linux.die.net/man/3/getline
            // ------------------------------------
            int line_num = 0;
            char lines[3][100];
            while ((read = getline(&line, &len, p_file)) != -1)
            {
                strcpy(lines[line_num++], line);
            }
            SEC = SEC_calculation(lines[1]);

            // ------------------------------------
            pclose(p_file);
            // ---------------------------------------------------

            printf("%d: %d %c   %d %s\n", sort, ProcessTable[i].PID, ProcessTable[i].S, SEC, ProcessTable[i].COMMAND);
            sort++;
        }
    }
    printf("Processes =      %d active\n", sort);

    //----------------------------------------------------------

    // handle usertime and system time
    long int USER_TIME, SYS_TIME;
    int ret;
    struct rusage usage;

    ret = getrusage(RUSAGE_CHILDREN, &usage);
    if (ret != 0)
    {
        printf("getrusage failed.\n");
        exit(4);
    }
    USER_TIME = usage.ru_utime.tv_sec;
    SYS_TIME = usage.ru_stime.tv_sec;

    printf("Completed processes:\n");
    printf("User time =        %ld seconds\n", USER_TIME);
    printf("Sys  time =        %ld seconds\n\n", SYS_TIME);
}

// Command: : exit
// End the execution of shell379. Wait until all processes initiated by the
// shell are complete. Print out the total user and system time for all
// processes run by the shell.
void EXIT_Handler(int *Process_num, struct Process ProcessTable[], char UserInputLINE[], int *Acitve_Process_num)
{
    // handle usertime and system time
    long int USER_TIME, SYS_TIME;
    int ret;
    struct rusage usage;

    // wait all the children process to be complete
    int temp_num = *Process_num;
    for (int i = 0; i < temp_num; i++)
    {
        // kill() to find running process
        if (kill(ProcessTable[i].PID, 0) == 0 && ProcessTable[i].S == 'R')
        {
            waitpid(ProcessTable[i].PID, NULL, 0);
        }
    }

    // get rusage times.
    ret = getrusage(RUSAGE_CHILDREN, &usage);
    if (ret != 0)
    {
        printf("getrusage failed.\n");
        exit(4);
    }
    USER_TIME = usage.ru_utime.tv_sec;
    SYS_TIME = usage.ru_stime.tv_sec;

    printf("\nResources used:\n");
    printf("User time =        %ld seconds\n", USER_TIME);
    printf("Sys  time =        %ld seconds\n\n", SYS_TIME);
}

// Command : kill <int>
// Kill process <int>.
void KILL_Handler(int argc, char **ARGS, int *Process_num, struct Process ProcessTable[], char UserInputLINE[], int *Acitve_Process_num)
{
    if (argc == 2)
    {
        if (atoi(ARGS[1]) <= 0)
        {
            printf("bad kill command input\n");
            printf("should use kill <pid>\n");
        }
        else
        {
            int pid = atoi(ARGS[1]);
            // kill() check the input pid
            if (kill(pid, 0) != 0)
            {
                printf("error in process with input :(\n");
                printf("please check the pid again.\n");
            }
            else
            {
                // 1 for kill
                // 3 for stop
                // 4 for cont
                kill(pid, SIGKILL);
                int temp_num = *Process_num;
                for (int i = 0; i < temp_num; i++)
                {
                    if (ProcessTable[i].PID == pid)
                    {
                        ProcessTable[i].S = 'T'; // change the status in pt
                    }
                }
            }
        }
    }
    else
    {
        printf("bad kill command input\n");
        printf("should use kill <pid>\n");
    }
}

// Command : resume <int>
// Resume the execution of process <int>. This undoes a suspend.
void RESUME_Handler(int argc, char **ARGS, int *Process_num, struct Process ProcessTable[], char UserInputLINE[], int *Acitve_Process_num)
{
    if (argc == 2)
    {
        if (atoi(ARGS[1]) <= 0)
        {
            printf("bad resume command input\n");
            printf("should use kill <pid>\n");
        }
        else
        {
            int pid = atoi(ARGS[1]);
            // kill() check the input pid
            if (kill(pid, 0) != 0)
            {
                printf("error in process with input :(\n");
                printf("please check the pid again.\n");
            }
            else
            {
                // 1 for kill
                // 3 for stop
                // 4 for cont
                kill(pid, SIGCONT);
                int temp_num = *Process_num;
                for (int i = 0; i < temp_num; i++)
                {
                    if (ProcessTable[i].PID == pid)
                    {
                        ProcessTable[i].S = 'R'; // change the status in pt
                    }
                }
            }
        }
    }
    else
    {
        printf("bad resume command input\n");
        printf("should use resume <pid>\n");
    }
}

// Command : sleep <int>
// Sleep for <int> seconds.
void SLEEP_Handler(int argc, char **ARGS, int *Process_num, struct Process ProcessTable[], char UserInputLINE[], int *Acitve_Process_num)
{
    if (argc == 2)
    {
        if (atoi(ARGS[1]) <= 0)
        {
            printf("Bad integer input!\n");
        }
        else
        {
            int sec = atoi(ARGS[1]);
            sleep(sec);
        }
    }
    else
    {
        printf("bad sleep command input\n");
        printf("should use sleep <int>\n");
    }
}

// Command : suspend <int>
// Suspend execution of process <int>. A resume will reawaken it.
void SUSPEND_Handler(int argc, char **ARGS, int *Process_num, struct Process ProcessTable[], char UserInputLINE[], int *Acitve_Process_num)
{
    if (argc == 2)
    {
        if (atoi(ARGS[1]) <= 0)
        {
            printf("bad suspend command input\n");
            printf("should use kill <pid>\n");
        }
        else
        {
            int pid = atoi(ARGS[1]);
            // kill() check the input pid
            if (kill(pid, 0) != 0)
            {
                printf("error in process with input :(\n");
                printf("please check the pid again.\n");
            }
            else
            {
                // 1 for kill
                // 3 for stop
                // 4 for cont
                kill(pid, SIGSTOP);
                int temp_num = *Process_num;
                for (int i = 0; i < temp_num; i++)
                {
                    if (ProcessTable[i].PID == pid)
                    {
                        ProcessTable[i].S = 'S'; // change the status in pt
                    }
                }
            }
        }
    }
    else
    {
        printf("bad suspend command input\n");
        printf("should use suspend <pid>\n");
    }
}

// Command : wait <pid>
// Wait until process <int> has completed execution.
void WAIT_Handler(int argc, char **ARGS, int *Process_num, struct Process ProcessTable[], char UserInputLINE[], int *Acitve_Process_num)
{
    if (argc == 2)
    {
        if (atoi(ARGS[1]) <= 0)
        {
            printf("bad wait command input\n");
            printf("should use kill <pid>\n");
        }
        else
        {
            int pid = atoi(ARGS[1]);
            // kill() check the input pid
            if (kill(pid, 0) != 0)
            {
                printf("error in process with input :(\n");
                printf("please check the pid again.\n");
            }
            else
            {
                // WUNTRACED 如果子进程进入暂停状态，则马上返回。
                waitpid(pid, NULL, WUNTRACED);
                int temp_num = *Process_num;
                for (int i = 0; i < temp_num; i++)
                {
                    if (ProcessTable[i].PID == pid)
                    {
                        ProcessTable[i].S = 'T'; // change the status in pt
                    }
                }
            }
        }
    }
    else
    {
        printf("bad wait command input\n");
        printf("should use wait <pid>\n");
    }
}

// Command : <cmd> <arg>*
// Spawn a process to execute command <cmd> with 0 or more arguments <arg>.
// <cmd> and <arg> are each one or more sequences of nonblank characters.
// & -->> If used, this must be the last argument and indicates that the command is to be executed in the background.
// input: <fname -->> This argument is the “<” character followed by a string of characters, a filename to be used for program input.
// output: >fname -->> This argument is the “>” character followed by a string of characters, a filename to be used for program output.
void OtherCMD(int argc, char **ARGS, int *Process_num, struct Process ProcessTable[], char UserInputLINE[], int *Acitve_Process_num)
{
    // printf("Handle OTHER command...\n");

    char Background[] = "&";

    char INPUT_SIGNAL = '<';
    char OUTPUT_SIGNAL = '>';

    int BOOLEAN_OUTPUT_FILE = 0;
    char *OUTPUT_FILE_NAME;

    int BOOLEAN_INPUT_FILE = 0;
    char *INPUT_FILE_NAME;

    int NEW_argc = argc;
    char **NEW_ARGS = malloc(argc * sizeof(*NEW_ARGS));
    int NEW_ARGS_i = 0; // record the current position of the new args.

    for (int i = 0; i < argc; i++)
    {
        // handle input filename condition
        // NO BUGS IN INPUTFILE BLOCK [DONE]
        if (ARGS[i][0] == INPUT_SIGNAL)
        {
            // latest version :0
            // take off the sign in front of the filename
            char *NEW_FILENAME = ARGS[i] + 1;
            // This line is cited/inspired by:
            // https://reactgo.com/c-remove-first-character/

            // find the # of chars in the output filename
            int n = 0;
            for (int i = 0; NEW_FILENAME[i] != '\0'; ++i)
            {
                n++;
            };
            // store the filename to OUTPUT_FILE_NAME
            INPUT_FILE_NAME = (char *)malloc(n);
            strncpy(INPUT_FILE_NAME, NEW_FILENAME, strlen(NEW_FILENAME));

            // change boolean value
            BOOLEAN_INPUT_FILE = 1;

            // realloc the NEW_ARGS and related variable
            NEW_argc--;
            NEW_ARGS = realloc(NEW_ARGS, NEW_argc * sizeof(*NEW_ARGS));
        }

        // handle output file condition
        // NO BUGS [DONE]
        else if (ARGS[i][0] == OUTPUT_SIGNAL)
        {
            // printf("hello this is OUTPUT block\n");

            // take off the sign in front of the filename
            char *NEW_FILENAME = ARGS[i] + 1;
            // This line is cited/inspired by:
            // https://reactgo.com/c-remove-first-character/

            // find the # of chars in the output filename
            int n = 0;
            for (int i = 0; NEW_FILENAME[i] != '\0'; ++i)
            {
                n++;
            };
            // store the filename to OUTPUT_FILE_NAME
            OUTPUT_FILE_NAME = (char *)malloc(n);
            strncpy(OUTPUT_FILE_NAME, NEW_FILENAME, strlen(NEW_FILENAME));

            // change boolean value
            BOOLEAN_OUTPUT_FILE = 1;

            // realloc the NEW_ARGS and related variable
            NEW_argc--;
            NEW_ARGS = realloc(NEW_ARGS, NEW_argc * sizeof(*NEW_ARGS));
        }

        // handle &
        else if (ARGS[i][0] == '&')
        {
            // printf("hello this is & block\n");
            // realloc the NEW_ARGS and related variable
            NEW_argc--;
            NEW_ARGS = realloc(NEW_ARGS, NEW_argc * sizeof(*NEW_ARGS));
        }

        // NO BUGS [DONE]
        else
        {
            // printf("hello this is normal args block\n");
            NEW_ARGS[NEW_ARGS_i] = ARGS[i];
            // // printf testing purpose
            // printf("ARGS in orgin:%s\n", NEW_ARGS[NEW_ARGS_i]);
            NEW_ARGS_i++;
        }
    }
    NEW_ARGS[NEW_ARGS_i] = NULL;

    // initialize pipe
    int pid_fd[2], usertime_fd[2], systime_fd[2];
    if (pipe(pid_fd) == -1 || pipe(usertime_fd) == -1 || pipe(systime_fd) == -1)
    {
        printf("An error ocurred with opening the pipe\n");
        _exit(1);
    }

    // execute command
    int pid = fork();
    if (pid < 0)
    {
        perror("fork");
        _exit(1);
    }
    else if (pid == 0)
    {
        close(pid_fd[0]);

        int data_pid = (int)getpid();
        // printf("Entering child process...\nwith pid:%d\n", (int)getpid());

        if (write(pid_fd[1], &data_pid, sizeof(int)) == -1)
        {
            printf("error ocurred with pipe\n");
            _exit(2);
        }
        close(pid_fd[1]);

        fflush(stdout);

        // test
        if (BOOLEAN_OUTPUT_FILE == 1)
        {
            // handle output file
            // get the output in the file

            // This line of code is lifesaving method
            // cited from:
            // https://blog.csdn.net/weixin_30695935/article/details/117056115?spm=1001.2101.3001.6661.1&utm_medium=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-1-117056115-blog-117062660.pc_relevant_multi_platform_featuressortv2dupreplace&depth_1-utm_source=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-1-117056115-blog-117062660.pc_relevant_multi_platform_featuressortv2dupreplace&utm_relevant_index=1
            freopen(OUTPUT_FILE_NAME, "w", stdout);

            // input classfication
            if (BOOLEAN_INPUT_FILE == 1)
            {
                freopen(INPUT_FILE_NAME, "r", stdin);
                if (INPUT_FILE_NAME)
                    free(INPUT_FILE_NAME);
            }

            // free memory allocation
            if (OUTPUT_FILE_NAME)
                free(OUTPUT_FILE_NAME);

            if (execvp(NEW_ARGS[0], NEW_ARGS) == -1)
            {
                perror("execvp");
                printf("Something wrong, this shouldn't print out\n");
                exit(1);
            }
        }
        else
        {
            // input classfication
            if (BOOLEAN_INPUT_FILE == 1)
            {
                freopen(INPUT_FILE_NAME, "r", stdin);
                // free memory:)
                if (INPUT_FILE_NAME)
                    free(INPUT_FILE_NAME);
            }

            if (execvp(NEW_ARGS[0], NEW_ARGS) == -1)
            {
                perror("execvp");
                printf("Something wrong, this shouldn't print out\n");
                exit(1);
            }
        }
    }
    else
    {
        close(pid_fd[1]);
        // intialize current child process pid.
        int data_pid;
        if (read(pid_fd[0], &data_pid, sizeof(int)) == -1)
        {
            printf("error ocurred with pipe\n");
            _exit(3);
        }
        close(pid_fd[0]);

        // debugging purpose
        // printf("User command is in cmd handler: %s\n", UserInputLINE);

        //-Process-Table-----------------
        struct Process process;
        process.number = *Process_num;
        process.PID = data_pid;
        process.S = 'R';
        strcpy(process.COMMAND, UserInputLINE);

        ProcessTable[*Process_num] = process;

        int temp = *Process_num;
        temp++;
        *Process_num = temp;

        // active process record
        int act_temp = *Acitve_Process_num;
        act_temp++;
        *Acitve_Process_num = act_temp;

        //-------------------------------

        if (ARGS[argc - 1][0] != '&')
        {
            waitpid(data_pid, NULL, 0);
            // wait(NULL);
        }
    }
    free(NEW_ARGS);
}

int SEC_calculation(char info_line[])
{
    // printf("input line is:\n%s\n", info_line);
    char *ARGS[20];
    char *TIMEARGS[4];
    // Extracting
    char *Tokens = strtok(info_line, " ");
    // The code which store each argument into an array
    // is cite from: https://stackoverflow.com/questions/15472299/split-string-into-tokens-and-save-them-in-an-array
    int i = 0;
    while (Tokens != NULL)
    {
        ARGS[i++] = Tokens;
        Tokens = strtok(NULL, " ");
    }

    i = 0;
    char *Time_Token = strtok(ARGS[2], ":.");
    while (Time_Token != NULL)
    {
        TIMEARGS[i++] = Time_Token;
        Time_Token = strtok(NULL, ":.");
    }

    if (atoi(TIMEARGS[0]) < 0 || atoi(TIMEARGS[1]) < 0 || atoi(TIMEARGS[2]) < 0)
    {
        printf("error message: this shouldn't print out for the second caculator:(\n");
        return 1;
    }
    else
    {
        int hour = atoi(TIMEARGS[0]);
        int min = atoi(TIMEARGS[1]);
        int sec = atoi(TIMEARGS[2]);
        return hour * 360 + min * 60 + sec;
    }
    return 0;
}

// It is really fun to have this kind of practical programming experience:)
// Author : Ke Li
// CCID : kli1