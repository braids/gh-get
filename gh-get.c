/*******************************************************************************
*   
*   gh-get
*   
*   Command-line wrapper for GitHub clones and pulls.
*   
*   This is intended to simplify the cloning and pulling of repos on GitHub by
*   reducing the information needed down to the user and repo name. Imitates
*   apt-get by cloning the repo if one does not exist, and updating the repo
*   with a pull if it already exists.
*   
*   Usage: gh-get <GitHub user>/<repository>
*   
*******************************************************************************/
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* Obtain what our setting in Git is for the repo dir */
char * repoDir()
{
    char dir[255];
    FILE *fp;
    
    fp = popen("git config --global --get gh-get.dir", "r");
    fgets(dir, 255, fp);
    pclose(fp);
    
    if(sizeof(dir) > 0)
        return dir;
    
    return NULL;
}

int main(int argc, char **argv)
{
    int i;
    
    char * repo = repoDir();
    
    /* Only taking one arg for now, yell if we get more or less. */
    if(argc < 2)
    {
        printf("Error: Less than one argument specified.\nUsage: gh-get <GitHub user>/<repository>\n");
        return 1; 
    }
    else if (argc > 3)
    {
        printf("Error: More than one argument specified.\nUsage: gh-get <GitHub user>/<repository>\n");
        return 2;
    }
    
    /* Build our git request. */
    char cmdstr[255] = "git ";
        
    char * input = argv[1];
    char * param = argv[2];
    
        /* Set home repo dir if -dir specified*/
    if(strcmp(input,"-dir") == 0)
    {
        char newdir[] = "git config --global --replace-all gh-get.dir ";
        strcat(newdir, param);
        
        i=system(newdir);
        
        return i;
    }
    else
    {
        /* Find if repo exists */
        bool repoexists = false;
        struct stat s;
        if(repo != NULL)
        {
            char * statdir = repo;
            strcat(statdir, "/");
            strcat(statdir, input);
            printf("%s", statdir);
            stat(statdir, &s);
        }
        else
            stat(input, &s);
        if(S_ISDIR(s.st_mode))
            repoexists = true;
        
        if(repoexists) 
        {
            /* If repo exists, perform a pull. */
            strcat(cmdstr, "-C ");
            if(repo != NULL) 
            {
                strcat(cmdstr, repo);
                strcat(cmdstr, "/");
            }
            strcat(cmdstr, input);
            strcat(cmdstr, " pull");
        }
        else
        {
            /* If repo does not exist, clone the repo. */
            strcat(cmdstr, "clone https://github.com/");
            strcat(cmdstr, input);
            strcat(cmdstr, ".git ");
            if(repo != NULL) 
            {
                strcat(cmdstr, repo);
                strcat(cmdstr, "/");
                strcat(cmdstr, input);
            }
        }
    }
    
    printf("%s\n", cmdstr);
    
    /* Run our git command. */
    i=system(cmdstr);
    
    /* Return whatever value we got. */
    return i;
}
