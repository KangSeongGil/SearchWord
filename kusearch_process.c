#include "kusearch_process.h"
int initFunction(int argc,char **argv,int *fileSize,int *processAm,int *searchAm,char **fileName,char ***searchTxt);
int readFile(char **fileCont,char *fileName,int fileSize);
int makeProcess(int *psOrder,pid_t *pid,int processAm,int *serverFlag);
int searchFunction(int massAm,int psOrder,char *fileCont,char *searchTxt,int startId,int endId);

int main (int argc , char *argv[])
{
	int fileSize,processAm,searchAm,massAm,psOrder=0,serverFlag=0,mq_id,mq_id_2,received,i,rtnVal;
	int *count,startId,endId,j;
	char *fileName,**searchTxt,*fileCont;
	pid_t *pid; 
	key_t ipckey,ipckey_2;
	MSG countMSG;
	MSG_2 indexMSG;
	
	initFunction(argc,argv,&fileSize,&processAm,&searchAm,&fileName,&searchTxt);
	count = (int *)calloc(sizeof(int),searchAm);
	readFile(&fileCont,fileName,fileSize);

	massAm = fileSize/processAm;

	pid = (pid_t *)malloc(sizeof(pid_t)*processAm);
	makeProcess(&psOrder,pid,processAm,&serverFlag);

	if(serverFlag==0)//client
	{
		ipckey_2 = 4000;
		mq_id_2=msgget(ipckey_2,IPC_CREAT|0666);
		ipckey = 3999;
		mq_id=msgget(ipckey,IPC_CREAT|0666);
		for(i=0;i<searchAm;i++)
		{
			if(msgrcv(mq_id_2,&indexMSG,sizeof(MSG_2)-sizeof(long),psOrder+1,0)==-1)
			{
				perror("msgrcv() test fail ");
			}
			else
			{
				rtnVal=searchFunction(massAm,psOrder,fileCont,searchTxt[indexMSG.num[2]],indexMSG.num[0],indexMSG.num[1]);
				countMSG.type=indexMSG.num[2]+1;
				countMSG.num=rtnVal; 
				if(msgsnd(mq_id,&countMSG,sizeof(int),0)==-1)
				{
					perror("msgsnd  error");
				}
			}

		}
	}
	else//server
	{
		ipckey_2 = 4000;	
		
		if((mq_id_2 = msgget(ipckey_2,IPC_CREAT|0666))==-1)
			perror("msgger Error");
	
		for(i=0;i<processAm;i++)
		{

			startId=i*massAm;
			for(j=0;j<searchAm;j++)
			{
				if((processAm-1)!=i)
				{
					endId=startId+massAm-1;
					endId=endId+(strlen(searchTxt[j])-1);
					if(endId>=fileSize-1) 
					{
						endId=fileSize-1;
					}
				}
				else
				{
					endId=fileSize-1;
				}
				indexMSG.type=i+1;
				indexMSG.num[0]=startId;
				indexMSG.num[1]=endId;
				indexMSG.num[2]=j;
				//printf("type %ld ,1 = %d,2 = %d,3 =%d \n",indexMSG.type,indexMSG.num[0],indexMSG.num[1],indexMSG.num[2]);
				if(msgsnd(mq_id_2,&indexMSG,sizeof(MSG_2)-sizeof(long),0)==-1)
					perror ("index send error");
				
			}
		}

		ipckey = 3999;
		mq_id=msgget(ipckey,IPC_CREAT|0666);
		
		for(i=0;i<processAm*searchAm;i++)
		{
			received = msgrcv(mq_id,&countMSG,sizeof(int),0,0);
			if(received == -1)
			{
				perror("msgrcv() fail ");
			}
			else
			{
				count[countMSG.type-1]+=countMSG.num;
			}
		}

		for(i=0;i<searchAm;i++)
		{
			printf("%s:%d\n",searchTxt[i],count[i]);
		}
	}
	
	free(count);
	free(fileName);
	free(fileCont);
    free(pid);
	
	for(i=0;i<searchAm;i++)
	{
		free(searchTxt[i]);
	}	
	free(searchTxt);

	return 0;
}

int searchFunction(int massAm,int psOrder,char *fileCont,char *searchTxt,int startId,int endId)
{
	int i,j,size,matchNum=0,checkId=0;
	char *cmpStr,*tmp;

	//printf("start:%d/end:%d\n",startId,endId);


	j=0;
	cmpStr = (char *)malloc(sizeof(char)*(endId-startId)+2);

	for(i=startId;i<=endId;i++)
	{
		cmpStr[j]=fileCont[i];
		j++;
	}

	cmpStr[j]='\0';
	//printf("\ncmpStr:%s\n",cmpStr);
	while(1)
	{
		tmp=strstr(cmpStr,searchTxt);
		if(tmp!=NULL)
		{
			matchNum++;
			*tmp='\n';
		}
		else
		{
			break;
		}
	}
	free(cmpStr);
	return matchNum;

}



int initFunction(int argc,char **argv,int *fileSize,int *processAm,int *searchAm,char **fileName,char ***searchTxt)
{
	 struct stat file_info;
	 int i,tmp;
	 *processAm = atoi(argv[1]);
	 *searchAm = argc-3;
	 *fileName = (char *)malloc(sizeof(char)*strlen(argv[2])+1);
	 fileName[0][strlen(argv[2])]='\0';
	 strncpy(fileName[0],argv[2],strlen(argv[2]));

	 if(0>stat(*fileName,&file_info))
	 {
	 	printf("No File\n");
	 	return -1;
	 }
	 *fileSize = file_info.st_size;

	 searchTxt[0] = (char **)malloc(sizeof(char *)*(*searchAm));
	 for(i=0;i<*searchAm;i++)
	 {
	 	tmp = strlen(argv[i+3]);
	 	searchTxt[0][i] = (char *)malloc(sizeof(char)*tmp+1);
	 	strncpy(searchTxt[0][i],argv[i+3],tmp);
	 	searchTxt[0][i][tmp]='\0';
	 }
	 return 1;
}

int readFile(char **fileCont,char *fileName,int fileSize)
{
	int i,fp;

	fileCont[0]=(char *)malloc(sizeof(char)*fileSize+1);
	
	if((fp = open(fileName,O_RDONLY))<0)
	{
		perror("error:");
		return -1;
	}

	if(read(fp,fileCont[0],fileSize)<=0)
	{
		perror("error:");
		return -1;
	}
	fileCont[0][fileSize]='\0';
	
	return 1;
}

int makeProcess(int *psOrder,pid_t *pid,int processAm,int *serverFlag)
{
	int i = 0;
	if(processAm == 0)
	{
		printf("잘못된 process 갯수 입력입니다.\n");
		exit(1);
	}

	for(i = 0;i<processAm;i++)
	{
		pid[i]=fork();
		if(pid[i]==0)
		{ 
			break;
		}
		else
		{
			(*psOrder)++;
			if(i==processAm-1)
				*serverFlag=1;
		}
	}
	return 0;
}











































