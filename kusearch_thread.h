#ifndef _KUSEARCH_THREAD_H_
    #define _KUSEARCH_THREAD_H_
    #include <string.h>
    #include <stdio.h>
    #include <unistd.h>
    #include <stdlib.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <pthread.h>
    #include <fcntl.h>
    #define MSGSZ 10
    #define DRECTORY /tmp/foo
    #define IDENTIFIER 42
    typedef struct paramiter
    {
        int massAm;
        int thrOrder;
        char *fileCont;
        char **searchTxt;
        int threadAm;
        int fileSize;
        int searchAm;
    }PRM;
#endif
