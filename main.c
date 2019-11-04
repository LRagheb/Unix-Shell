#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>


void sigHandler(int n){  //signal handler to detect termination of ALL CHILDREN
pid_t pid;
//pid=wait(NULL);
FILE *fp; // Pointer for file
//fopen either creates or opens an existing file
    fp=fopen("logFile.log","a"); //Whenever a sigchild is detected it appends to the file
    if (fp==NULL)
    {
    printf("\n Error! log file not created");
    }
    fprintf(fp,"CHILD PROCESS WAS TERMINATED\n");
    fclose(fp);
}

int main()
{
                        signal(SIGCHLD,sigHandler);

    int backB,x; //boolean for background acts
    char prevd[1024];
    char temp[1024];
    char initd[1024];


    //get initial directory (To enable user to return to it if command "cd" is used
    //previous directly is saved when cd is used to enable user to return to previous directory upon typing "cd -"
    getcwd(prevd,sizeof(prevd));
    printf("PATH %s\n",prevd);
    strcpy(initd,prevd);
    printf("PATH %s\n",initd);


    char inputcmd[1000]; //Command line taken from user
    char copycmd[1000];
    char *parsedcmd[100]; //Parsed Command (Array of Strings)

    char *token;
    int size,i,j;
    char emptys[100]= {'\0'};


    while (1)
    {
        i=0; //reset used variables
        j=0;

//FIRST: INPUT COMMAND
        printf("\nShell> ");
        fflush(stdout);
        fgets(inputcmd,1000,stdin);
        size=strlen(inputcmd);
        inputcmd[size-1]='\0';
        if (inputcmd[size-2]=='&')
        {
            inputcmd[size-2]='\0';
            backB=1; //background command
        }
        else
            backB=0;
        strcpy(copycmd,inputcmd); //keep original command until it is executed since tokenising destroys strings


//THEN: TOKENIZE COMMAND

        token=strtok(inputcmd," ");  //first token
        while (token!=NULL)
        {
            parsedcmd[i]=token;
            token=strtok(NULL," ");
            i++;
        }
        parsedcmd[i]='\0';


//CHECK FOR EXIT COMMAND
        if (strcmp(parsedcmd[0],"exit")==0)
            exit(0);

//CHECK FOR CD COMMAND
        else if (strcmp(parsedcmd[0],"cd")==0)
        {
            if (strlen(copycmd)==2)

            {
                parsedcmd[1]=initd;
            }

            else if (strcmp(parsedcmd[1],"-")==0)
            {
                parsedcmd[1]=prevd;
            }

            getcwd(temp,sizeof(temp));
            if (chdir(parsedcmd[1])<0)
                printf("Error! Couldn't change directory \n");

            else
                strcpy(prevd,temp);


        }

//COMMANDS WITH EXECVP
        else

        {

            pid_t child=fork();

            int status;

            {


                if (child==0) //fork successful
                   { if (execvp(parsedcmd[0],parsedcmd)<0)
                        printf("ERROR! Perhaps the command was wrong?");
                        }

                else if (child>0)
                {
                        if (backB==0) //not a background
                            waitpid(child,&status,WUNTRACED); //parent waits


                }
                else
                    printf("Error! Unsuccessful execution of %s",parsedcmd[0]);
            }



        }
//RESET STRING ARRAYS
        inputcmd[0]='\0';
        for (j; j<i; j++)
            parsedcmd[j]="";

        backB=0;




    }
}
