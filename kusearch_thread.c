#include "kusearch_thread.h"
int initFunction(int argc,char **argv,int *fileSize,int *threadAm,int *searchAm,char **fileName,char ***searchTxt);
int readFile(char **fileCont,char *fileName,int fileSize);
int makeProcess(int *psOrder,pid_t *pid,int threadAm,int *serverFlag);
void *searchFunction(void *arg);
int *count;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main (int argc , char *argv[])
{
	int fileSize,threadAm,searchAm,massAm,threadOrder=0,serverFlag=0,mq_id,received,i,rtnVal;
	char *fileName,**searchTxt,*fileCont;
	pthread_t *p_thread; 
	key_t ipckey;
	PRM *searchPara;
	
	initFunction(argc,argv,&fileSize,&threadAm,&searchAm,&fileName,&searchTxt);
	count = (int *)calloc(sizeof(int),searchAm);
	readFile(&fileCont,fileName,fileSize);

	massAm = fileSize/threadAm;
	p_thread = (pthread_t *)malloc(sizeof(pid_t)*threadAm);
	searchPara = (PRM *)malloc(sizeof(PRM)*threadAm);

	//printf("threadAm=%d\n",threadAm);

	for(i=0;i<threadAm;i++)
	{
		pthread_mutex_lock(&mutex);
			searchPara[i].massAm=massAm;
			searchPara[i].thrOrder=threadOrder;
			searchPara[i].fileCont=fileCont;
			searchPara[i].searchTxt=searchTxt;
			searchPara[i].threadAm=threadAm;
			searchPara[i].fileSize=fileSize;
			searchPara[i].searchAm=searchAm;
		pthread_create(&p_thread[i],NULL,searchFunction,&searchPara[i]);
		pthread_mutex_unlock(&mutex);
		pthread_mutex_lock(&mutex);
		threadOrder++;
		pthread_mutex_unlock(&mutex);
	}

	for(i=0;i<threadAm;i++)
	{
		pthread_join (p_thread[i],NULL);
	}

	for(i=0;i<searchAm;i++)
	{
		printf("%s:%d\n",searchTxt[i],count[i]);
	}

	free(count);
	free(fileName);
	free(fileCont);
    	free(p_thread);
	
	for(i=0;i<searchAm;i++)
	{
		free(searchTxt[i]);
	}	
	free(searchTxt);
	return 0;
}


void *searchFunction(void *searchPara)
{
	int startId,endId,i,j,k,size,matchNum=0,checkId=0;
	char *cmpStr,*tmp;
	PRM *para = searchPara;
	int order = para->thrOrder;
	pthread_mutex_lock(&mutex);
	//printf("%d order start\n",order);
	pthread_mutex_unlock(&mutex);
	for(i = 0;i<para->searchAm;i++)
	{
		pthread_mutex_lock(&mutex);
			startId=(order)*(para->massAm);
			
			if((para->threadAm-1)!=order)
			{
				endId=startId+para->massAm-1;
				endId=endId+(strlen(para->searchTxt[i])-1);
				if(endId>=para->fileSize-1)
				{
					endId=para->fileSize-1;
				}
			}
			else
			{
				endId=para->fileSize-1;
			}

			j=0;
			//printf("%d order indexing\n",order);
		pthread_mutex_unlock(&mutex);

		cmpStr = (char *)malloc(sizeof(char)*(endId-startId)+2);

		pthread_mutex_lock(&mutex);
			for(k=startId;k<=endId;k++)
			{
				cmpStr[j]=para->fileCont[k];
				j++;
			}
			//printf("안녕\n");
			cmpStr[j]='\0';
			//printf("%d order copy\n",order);
		pthread_mutex_unlock(&mutex);
		matchNum=0;
		while(1)
		{
			pthread_mutex_lock(&mutex);
				tmp=strstr(cmpStr,para->searchTxt[i]);
			pthread_mutex_unlock(&mutex);
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
		//printf("%d order search\n",order);
		pthread_mutex_lock(&mutex);
		count[i]+=matchNum;
		//printf("%d order counting\n",order);
		pthread_mutex_unlock(&mutex);
	}
	pthread_mutex_lock(&mutex);
	//printf("%d order end\n",order);
	pthread_mutex_unlock(&mutex);
	return NULL;
}



int initFunction(int argc,char **argv,int *fileSize,int *threadAm,int *searchAm,char **fileName,char ***searchTxt)
{
	 struct stat file_info;
	 int i,tmp;
	 *threadAm = atoi(argv[1]);
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












































