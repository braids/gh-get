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

bool isDir(char * dir)
{
    struct stat statdir;
    stat(dir, &statdir);
    return S_ISDIR(statdir.st_mode) ? true : false;
}

int main(int argc, char **argv)
{
    /* Taking one or more args for now, yell if we get less. */
    if(argc < 2)
    {
        printf("Error: Less than one argument specified.\nUsage: gh-get <GitHub user>/<repository>\n");
        return 1; 
    }
    
    /* Initialize our git command string. */
    char cmdstr[255];
    
    /* Formalize pointers to the first two args; I am lazy and will make this more elaborate later. */
    char * input = argv[1];
    char * param = argv[2];
    
    /* Set commonrepo dir if -commonrepo specified*/
    if(strcmp(input,"-commonrepo") == 0)
    {
        /* Sanitize slashes */
        int i;
        for(i=0; i<strlen(param); i++)
            if(param[i] == '\\')
                param[i] = '/';
        
        /* Set our commonrepo path in Git. */
        sprintf(cmdstr, "git config --global --replace-all gh-get.commonrepo %s", param);
    }
    else
    {
        /* Find if commonrepo exists. */
        char commonrepo[255];
        FILE *fp;
        fp = popen("git config --global --get gh-get.commonrepo", "r");
        fgets(commonrepo, 255, fp);
        pclose(fp);
        
        /* Remove commonrepo newline. */
        char * newline = strchr(commonrepo,'\n');
        *newline = '\0';
        
        /* Init specified repo directory. If local repo detected, modify path. */
        char repodir[255];
        if(strlen(commonrepo) != 0) 
        {
            sprintf(repodir, "%s/", commonrepo);
        }
        strcat(repodir, input);
        
        /* If repodir exists, perform a pull. */
        if(isDir(repodir)) 
        {
            sprintf(cmdstr, "git -C %s pull", repodir);
        }
        /* If repodir does not exist, clone to repodir. */
        else
        {
            sprintf(cmdstr, "git -C %s clone https://github.com/%s.git", repodir, input);
        }
    }
    
    /* Run our git command and return whatever value we got. */
    return system(cmdstr);
}
