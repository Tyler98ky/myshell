//
// Created by tug65404 on 10/16/2019.
//

#ifndef LINUX_SHELL_MYSHELL_H
#define LINUX_SHELL_MYSHELL_H

int getInput(char* input);
void clearTheScreen(void);
void executeCommands(char** inputParsed);
void printCurrentDirectory(void);
void executeCommandsPiped(char** inputParsed, char** parsedpipe);
int builtInCommands(char** inputParsed);
void formatDirectory(char* directory);
void printHelp(void);
void parseSpace(char* input, char** inputParsed);
int processString(char* input, char** inputParsed, char** inputParsedpipe);
int parsePipe(char* input, char** inputPiped);
void init_shell(void);
void printEnvironmentVariables(void);

#endif //LINUX_SHELL_MYSHELL_H
