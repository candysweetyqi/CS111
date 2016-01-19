#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#define no_argument 0
#define required_argument 1 
#define optional_argument 2


int main(int argc, char * argv[])
{
  printf("Hello\n");

  const struct option longopts[] =
  {
    {"command",     required_argument,  0, 'c'},
    {"verbose", no_argument, 0, 'v'},
    {"rdonly", required_argument,0,'r'},
    {"wronly",required_argument,0,'w'},
    {0,0,0,0},
  };

  int index;
  int iarg=0;
  int fd;
  pid_t pid;
  //turn off getopt error message
  //opterr=0; 
  errno = 0;
  int verbose_shown=0;
  int logic_fd=0;
  //map logic fd to real fd
  int fd_vec[argc];
  while((iarg = getopt_long(argc, argv, "", longopts, &index)) != -1)
  {
    if(verbose_shown==1){
      printf ("%s ", longopts[index].name);
      int vidx = optind;
      //printf("optind: %d\n", optind);
      for( ;vidx < argc; ++vidx){
            //printf("optind: %d", optind);
        if(*argv[vidx] == '-' ){
          if (vidx<argc-1 && (*argv[vidx+1] == '-'))
            break;  //hit the next option
        }
        printf("%s ", argv[vidx]);       
      }
      printf ("\n");
    }
    switch (iarg)
    {
      case 'v':
        verbose_shown=1;
        break;

      case 'c':
        pid = fork();
        char* a[20];
        // Child process
        if (pid == 0) {
            // Execute command
            if(optarg){
              optind--;
              //printf("first logistic fd: %s, real fd is:%d\n", argv[optind],fd_vec[atoi(argv[optind])]);
              dup2(fd_vec[atoi(argv[optind++])],0);
              //printf("second logistic fd: %s, real fd is:%d\n", argv[optind],fd_vec[atoi(argv[optind])]);
              dup2(fd_vec[atoi(argv[optind++])],1);
              //printf("third logistic fd: %s, real fd is:%d\n", argv[optind],fd_vec[atoi(argv[optind])]);
              dup2(fd_vec[atoi(argv[optind++])],2);
              //optind++;
              //optind++;
              //printf("optind: %d, argc: %d\n", optind,argc);
              int i;
              for (i = 0;optind < argc; ++i,++optind)
              {
                if(*argv[optind] == '-' ){
                  if (optind<argc-1 && (*argv[optind+1] == '-'))
                    break;  //hit the next option
                }
                a[i] = argv[optind];
                //printf("%s\n", a[i]);
              }
              a[i] = NULL;
              execvp(a[0], a);
            }  
            else{
              // Error occurred
              char* error = strerror(errno);
              printf("shell: %s: %s\n", a[0], error);
              return 0;
            }
            exit(0);
        }

        // Parent process
        else {
            // Wait for child process to finish
            int childStatus;
            waitpid(pid, &childStatus, 0);
            return 1;
        } 
        break;

      case 'r':
        if((fd = open(optarg, O_RDONLY))!=-1){
          printf("read only is hit, the logic fd is %d\n",logic_fd);
          fd_vec[logic_fd++] = fd;  //store the true fd and make it can be visited via logic fd
        }
        else{
          printf("\n open() failed with error [%s]\n",strerror(errno));
          return 1;
        }
        break;
      case 'w':
        if((fd = open(optarg, O_WRONLY))!=-1){
          printf("write only is hit, the logic fd is %d\n",logic_fd);
          fd_vec[logic_fd++] = fd;  //store the true fd and make it can be visited via logic fd
          
        }
        else{
          printf("\n open() failed with error [%s]\n",strerror(errno));
          return 1;
        }
        break;
    }
    
  }
  //printf("optind: %d\n", optind);
  //printf("argc: %d\n", argc);
  //printf ("%s\n ", optarg);


  // if (optind < argc)
  //       {
  //         printf ("non-option ARGV-elements: ");
  //         while (optind < argc){
  //           printf ("%s\n ", argv[optind++]);
  //           printf ("%s\n ", optarg);
  //         }
  //         printf ("\n");
  //       }


  return 0; 
}