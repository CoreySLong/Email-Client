/////////////////////////////////////////////////////////////////////////////////////////
// File: shellex.c
// Description: A simple UNIX shell as project #3 for cmsc 257
// Author: Professor Sonmez and Corey Long
// Start Date: 11/28/21
// Notes:
/////////////////////////////////////////////////////////////////////////////////////////

/* $begin shellmain */
#include "csapp.h"
#define MAXARGS   128

/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);

void fpid(){
    printf("%d\n", getpid());     // Should print the pid when user inputs "pid"
    fflush(stdout);
}

void fppid(){
    printf("%d\n", getppid());    // Should print the ppid when user inputs "ppid"
    fflush(stdout);
}

void fexit(){
    raise(SIGTERM);               // Raises the terminate signal to kill the process
}

void fcd(){
    char cwd[100];
    printf("%s\n", getcwd(cwd, sizeof(cwd)));
    fflush(stdout);
}

void fcwd(char * string){
    chdir(string);
}

void fhelp(){
    printf("**********************************************************************\n"	
    "A Custom Shell for CMSC 257\n"
    "Corey Long\n"
    "Usage: \n"
    "	- You can change the prompt by running the program with -p \"prompt name\"\n"
    "**********************************************************************\n"	

    "BUILTIN COMMANDS:\n"
    "	- quit: exits the command prompt\n"
    "	- exit: exits the command prompt by terminating the process\n"
    "	- pid:  prints the ID of the current process running\n"
    "	- ppid: prints the ID of the parent of the current process running\n"
    "	- cd:   prints the current working directory\n"
    "	    - cd \"directory name\": changes the directory to inputted directory name\n"
    "	    - cd ..: changes the directory to the parent directory\n"

    "SYSTEM COMMANDS:\n"
    "	- Refer to the man pages for these commands by using the command \"man\"\n");
    fflush(stdout);
}

void handler(){}   // Method used to make CTRL + C do nothing

int main(int argc, char *argv[]){
    char cmdline[MAXLINE]; /* Command line */

    signal(SIGINT, handler);    // signal handler for CTRL + C
    
    while (1) {
        /* Read */
        if(argc > 1) {    // Checks to see if there is input from the user    
        if(strcmp(argv[1], "-p") == 0)
                printf("%s> ", argv[2]);     // Should be printing out what the user wants for the prompt
		fflush(stdout);
	}
        else{
            printf("sh257> ");
	    fflush(stdout);
	}
        Fgets(cmdline, MAXLINE, stdin);
        if (feof(stdin))
            exit(0);

        /* Evaluate */
        eval(cmdline);
    }
}
/* $end shellmain */

/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline){
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */

    int child_status;

    strcpy(buf, cmdline);
    bg = parseline(buf, argv);
    if (argv[0] == NULL)
        return;   /* Ignore empty lines */

    if (!builtin_command(argv)) {
        if ((pid = Fork()) == 0) {   /* Child runs user job */
            if (execvp(argv[0], argv) < 0) {
                printf("Execution failed (in fork)\n");
		fflush(stdout);
		printf("%s: Command not found.\n", argv[0]);
		fflush(stdout);
		exit(1);
            }
        }

        /* Parent waits for foreground job to terminate */
        if (!bg) {
            int status;
            if (waitpid(pid, &status, 0) < 0)
                unix_error("waitfg: waitpid error");
	    
	    //pid_t wpid = wait(&child_status);
	    if(WIFEXITED(child_status)){
		printf("Process exited with status code %d\n", WEXITSTATUS(status));
		fflush(stdout);
	    }
        }
        else{
            printf("%d %s", pid, cmdline);
	    fflush(stdout);
	}
    }
    return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv){
    if (!strcmp(argv[0], "quit")) /* quit command */
        exit(0);
    if (!strcmp(argv[0], "&"))    /* Ignore singleton & */
        return 1;

    // My if statements
    if(!strcmp(argv[0], "pid")){
        fpid();
        return 1;
    }
    if(!strcmp(argv[0], "ppid")){
        fppid();
        return 1;
    }
    if(!strcmp(argv[0], "exit")){
        fexit();
        return 1;
    }
    if(!strcmp(argv[0], "cd")){
	if(argv[1] != NULL)
	    fcwd(argv[1]);
	else
	    fcd();
	return 1;
    }
    if(!strcmp(argv[0], "help")){
	fhelp();
	return 1;
    }
    return 0;                     /* Not a builtin command */
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv){
    char *delim;         /* Points to first space delimiter */
    int argc;            /* Number of args */
    int bg;              /* Background job? */

    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
        buf++;

    /* Build the argv list */
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;
        while (*buf && (*buf == ' ')) /* Ignore spaces */
            buf++;
    }
    argv[argc] = NULL;

    if (argc == 0)  /* Ignore blank line */
        return 1;

    /* Should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0)
        argv[--argc] = NULL;

    return bg;
}
/* $end parseline */

