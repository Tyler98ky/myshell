#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/wait.h>

#include "myshell.h"


extern char **environ;

int main()
{
    /*
    * while(true) {
    *   printCurrentDirectory();
    *   input = getInput();
    *
    *   Boolean shouldForkProcesses = processString(input);
    *
    *   if piped: executeCommandsPiped(input);
    *   else if not builtIn: executeCommands(input);
    * }
    */

    char inputString[1024];
    char *parsedArgs[128];
    char* parsedArgsPiped[128];
    int execFlag = 0;
    greetingMessage();

    while (1) {
        if (getInput(inputString))
            continue;
        execFlag = processString(inputString, parsedArgs, parsedArgsPiped);

        if (execFlag == 1)
            executeCommands(parsedArgs);

        else if (execFlag == 2)
            executeCommandsPiped(parsedArgs, parsedArgsPiped);
    }
}

void greetingMessage() {
    printf("Hello world, this is my shell\nGo ahead and enter in some commands!\n\n\n");

}

/**
 * Prompt for user input with myshell> and read the following characters as a string
 * @param input
 * @return 1 if input wasn't valid, 0 otherwise. If it was valid, also copy the contents of the input into @param input
 */
int getInput(char* input)
{
    /*
    * input = readline();
    * if (input != null) return 0
    * else return 1;
    *
    */
    char *buf;
    buf = malloc(1024);
    printf("myshell> ");
    fgets(buf, 1024, stdin);
    strtok(buf, "\n");  // remove trailing newline
    if (strlen(buf) != 0) {
        strcpy(input, buf);
        return 0;
    } else {
        return 1;
    }
}

void clearTheScreen() {
    /*
    * n = height of the window
    * print n blank lines
    */
    printf("\033[H\033[J");

}


void executeCommands(char** inputParsed)
{
    /*
    * child = fork();
    * if (child <= 0) {
    *   print("Creating child process failed");
    * } else {
    *   execvp(the new command inputinputParsed);
    *   exit(0);
    * }
    *
    * else {
    *   wait for child to finish;
    *   return;
    * }
    */

    pid_t pid = fork();

    if (pid == -1) {
        return;
    } else if (pid == 0) {
        if (execvp(inputParsed[0], inputParsed) < 0) {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));

        }
        exit(0);
    } else {
        wait(NULL);
        return;
    }
}

void printCurrentDirectory()
{
    /*
    * currentDir = getcwd();
    * print(currentDir);
    */
    char currentDir[1024];
    char *userName = getenv("USER");

    getcwd(currentDir, sizeof(currentDir));
//    formatDirectory(currentDir);
    printf("%s@%s", userName, currentDir);
}

void executeCommandsPiped(char** inputParsed, char** parsedpipe)
{
    /*
    * p1 = fork();
    * if (p1 == 0) {
    *   close the read end of the pipe;
    *   dup(the write end of the pipe);
    *   close the write end of the pipe;
    *
    *   execvp to change execution to read end of the pipe;
    * }
    * else {
    *   p2 = fork();
    *   if (p2 == 0) {
    *       close the read end of the pipe;
    *       dup(the write end of the pipe);
    *       close the write end of the pipe;
    *
    *       execvp to change execution to read end of the pipe;
    *   } else {
    *       wait for processes to execute;
    *   }
    * }
    */

    int pipefd[2];
    pid_t p1, p2;

    if (pipe(pipefd) < 0) {
        return;  // pipe failed
    }
    p1 = fork();
    if (p1 < 0) {
        return;  // fork failed
    }

    if (p1 == 0) {  // child process
        close(1);
        dup2(pipefd[1], 1);
        close(pipefd[0]);

        if (execvp(inputParsed[0], inputParsed) < 0) {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(0);
        }
    } else {  // parent process
        p2 = fork();

        if (p2 < 0) {
            return;  // failed to fork
        }

        if (p2 == 0) {  // second child process
            close(0);
            dup2(pipefd[0], 0);
            close(pipefd[1]);
            if (execvp(parsedpipe[0], parsedpipe) < 0) {
                char error_message[30] = "An error has occurred\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(0);
            }
        } else {  // parent
            wait(NULL);
        }
    }

//    int pipefd[2];
//    int pid;
//    char *ls_args[] = {"ls", NULL};
//    char *wc_args[] = {"wc", NULL};
//
//    pipe(pipefd);
//    pid = fork();
//    if (pid == 0) {
//        dup2(pipefd[0], 0);
//        close(pipefd[1]);
//        execvp(parsedpipe[0], parsedpipe);
//    } else {
//        dup2(pipefd[1], 1);
//        close(pipefd[0]);
//        execvp(inputParsed[0], inputParsed);
//    }
}

void formatDirectory(char* directory) {
    char* homeDirectory = getenv("HOME");
    size_t directoryLength = sizeof(directory) / sizeof(directory[0]);
    size_t homeLength = sizeof(homeDirectory) / sizeof(homeDirectory[0]);
    char returnValue[2048];


    if (strstr(directory, homeDirectory) != NULL) {
        returnValue[0] = '~';
        for (int i = 0; i < (directoryLength-homeLength); i++) {
            returnValue[i+1] = directory[homeLength+i];
        }

        strcpy(directory, returnValue);
    }
}

/**
 *
 * @param inputParsed the parsed command to check if its a built in command
 * @return 1 if it was a built in command, 0 otherwise
 */
int builtInCommands(char** inputParsed)
{
    /*
    * commandList = {dir, clr, help, ls, etc..}
    * for every command in commandList {
    *   if strcmp(inputParsed, command): execute command;
    * }
    * */
    char* builtInCommands[] = {"cd", "clr", "", "environ", "", "help", "pause", "quit"};
    size_t builtInCommandListLength = sizeof(builtInCommands) / sizeof(builtInCommands[0]);
    int commandNumber = -1;

    for (size_t i = 0; i < builtInCommandListLength; i++) {
        if (strcmp(inputParsed[0], builtInCommands[i]) == 0) {
            commandNumber = i;
            break;
        }
    }

    switch (commandNumber) {
        case 0:
            chdir(inputParsed[1]);
            return 1;
        case 1:
            clearTheScreen();
            return 1;
        case 2:
            return 1;
        case 3:
            printEnvironmentVariables();
            return 1;
        case 4:
            return 1;
        case 5:
            printHelp();
            return 1;
        case 6:
            return 1;
        case 7:
            exit(0);
        default:
            break;
    }
    return 0;
}

void printEnvironmentVariables() {
    for (char **env = environ; *env; ++env)
        printf("%s\n", *env);
}

void printHelp()
{
    /*
    * pipe the user manual into built in command more
    * executeCommandsPiped("HelpText", "more");
    * */

    char helpText[] = "Built-in commands:\n\t{\"cd\", \"clr\", \"dir\", \"environ\", \"echo\", \"help\", \"pause\", \"quit\"}\n";
    printf("%s\n", helpText);
}


void parseSpace(char* input, char** inputParsed)
{
    /*
    * parsedString = split(input, " ")
    * return parsedString;
    * */

    for (int i = 0; i < 128; i++) {
        inputParsed[i] = strsep(&input, " ");

        if (inputParsed[i] == NULL)
            break;
        if (strlen(inputParsed[i]) == 0)
            i--;
    }
}

int processString(char* input, char** inputParsed, char** inputParsedpipe)
{

    /*
    * if (input is piped) {
    *   parsePipe(input, inputParsedInput);
    * } else {
    *   parseSpace(input, inputParsedInput);
    * }
    *
    * builtInCommand(input)
    *
    * return
    *   0 if built in
    *   1 if piped
    *   2 if regular commands
    *
    * */

    char* strpiped[2];
    int piped = 0;

    piped = parsePipe(input, strpiped);

    if (piped) {
        parseSpace(strpiped[0], inputParsed);
        parseSpace(strpiped[1], inputParsedpipe);

    } else {

        parseSpace(input, inputParsed);
    }

    if (builtInCommands(inputParsed))
        return 0;
    else
        return 1 + piped;
}

int parsePipe(char* input, char** inputPiped)
{
    /*
    * parsedString = split(input, "|")
    * return parsedString;
    * */
    for (int i = 0; i < 2; i++) {
        inputPiped[i] = strsep(&input, "|");
        if (inputPiped[i] == NULL)
            break;
    }

    if (inputPiped[1] == NULL)
        return 0;  // no pipe found
    else {
        return 1;
    }
}