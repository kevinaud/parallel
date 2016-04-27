#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>

void fileStuff( int fd, char ch )
{
    char *msg = "Child: ";

    /*
    if ( (fd = open("tFile", O_RDWR)) == -1 )
    {
         perror("Bad file");
         exit(-1);
    }
    */
    struct flock myLock;

    myLock.l_type = F_WRLCK;
    myLock.l_whence = SEEK_SET;
    myLock.l_start = 0;
    myLock.l_len = 1;

    fcntl(fd, F_SETLKW, &myLock);

    write(1, msg, 7);
    write(1 ,&ch ,1);
    write(1, "\n", 1);

    lseek(fd, 0, SEEK_SET);
    write(fd, &ch, 1);

    myLock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, & myLock);

    close(fd);

    exit(0);
}

int main()
{
    int i, fd;
    pid_t pid;

    if ( (fd = open("tFile", O_RDWR)) == -1 )
    {
         perror("Bad file");
         exit(-1);
    }

    for ( i = 0; i < 10; i++) 
    {
        pid = fork();
        if( pid == 0 )
            fileStuff( fd, (char)i + '0');
    }

    for(i = 0; i < 10; ++i)
    {
        pid = wait(NULL);
    }

    return 0;
}
