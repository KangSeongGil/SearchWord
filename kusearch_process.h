#ifndef _KUSEARCH_PROCESS_H_
    #define _KUSEARCH_PROCESS_H_
    #include <string.h>
    #include <stdio.h>
    #include <unistd.h>
    #include <stdlib.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <sys/ipc.h>
    #include <sys/msg.h>
    #define MSGSZ 10
    #define DRECTORY /tmp/foo
    #define IDENTIFIER 42
    typedef struct msgbuf
    {
        long type;
        int num;
    }MSG;

     typedef struct msgbuf_2
    {
        long type;
        unsigned int num[3];
    }MSG_2;
#endif
