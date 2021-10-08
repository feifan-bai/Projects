#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<dirent.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include <errno.h>

#define BUFSIZE 1000

int copy(char *path1,char *path2);
void List(char *path, char *path2,int indent)
{
    struct    dirent*ent = NULL;
    DIR        *pDir;
    char    dir[512],dir2[512];
    struct stat statbuf;

    if( (pDir=opendir(path))==NULL )
    {
        fprintf(stderr, "Cannot open directory:%s\n", path);
        return;
    }
    while( (ent=readdir(pDir))!=NULL )
    {
        //�õ���ȡ�ļ��ľ���·����
    	snprintf(dir, 512,"%s/%s",path,ent->d_name);
	    snprintf(dir2, 512,"%s/%s",path2,ent->d_name);
        //�õ��ļ���Ϣ
    	lstat(dir, &statbuf);
        //�ж���Ŀ¼�����ļ����ļ��򷵻�1
    	if(S_ISDIR(statbuf.st_mode) )
       	{
         //�ų���ǰĿ¼���ϼ�Ŀ¼
            if(strcmp(".",ent->d_name) == 0 || strcmp( "..",ent->d_name) == 0)
            {
            continue;
            }
            //�������Ŀ¼,�ݹ���ú�������,ʵ����Ŀ¼���ļ�����
			// ȷ���ļ��Ƿ���ڣ������򷵻�0�����򷵻�-1
	        if (access(dir2,0)==-1)
	        {
            mkdir(dir2, S_IRWXU);//����һ��Ŀ¼��ӵ�ж���д�Ͳ���Ȩ��
	        }
			//�ݹ���ã�������Ŀ¼���ļ�
            List(dir, dir2,indent+4 );
       }
       else
            {
                if (access(dir2, 0)==-1)
		        {
			        copy(dir,dir2);
			        printf("%s has been copied.\n",dir);
		        }
		        else
		        {
			        struct stat st1,st2;
			        stat(dir,&st1);
			        stat(dir2,&st2);
			        if (st1.st_mtime!=st2.st_mtime)
			        {
				         copy(dir,dir2);
				         printf("%s has been copied.\n",dir);
			        }
		        }
             }
       }
    closedir(pDir);
}

void Delete(char *path2, char *path, int indent)
{
    struct    dirent*ent = NULL;
    DIR        *pDir;
    char    dir[512],dir2[512];
    struct stat statbuf;
    char c;
    	if( (pDir=opendir(path))==NULL )
    	{
        	fprintf(stderr,"Cannot open directory:%s\n", path );
        	return;
    	}
	while( (ent=readdir(pDir))!=NULL )
    	{
		snprintf( dir,512,"%s/%s", path, ent->d_name );
		snprintf( dir2,512,"%s/%s", path2, ent->d_name );
		//lstat������ȡһЩ�ļ������Ϣ
		lstat(dir, &statbuf);
		if(S_ISDIR(statbuf.st_mode) )//��Ŀ¼�����
		{
           		if(strcmp( ".",ent->d_name) == 0||strcmp( "..",ent->d_name) == 0)
            		{
                		continue;
            		}
            		if (access(dir2,0)==-1)
			{
				Delete(dir2,dir,indent+4 );
				remove(dir);//ɾ��һ���ļ�
				printf("%s has been deleted.\n",dir);
				}
			else
				Delete(dir2,dir,indent+4);
		}
		else
		{
			if (access(dir2, 0)==-1)
			{
				remove(dir);
				printf("%s has been deleted.\n",dir);
				}
			}
		}
    closedir(pDir);
}

int copy(char *path1,char *path2)
{
    int c;
    FILE *fpSrc, *fpDest;  //��������ָ���ļ���ָ��
    fpSrc = fopen(path1, "rb");    //�Զ�ȡ�����Ƶķ�ʽ��Դ�ļ�
    if(fpSrc==NULL){
        printf("Unable to open source file.\n");  //Դ�ļ������ڵ�ʱ����ʾ����
        return 0;
    }
    fpDest = fopen(path2, "wb");  // //��д������Ƶķ�ʽ��Ŀ���ļ�
    if(fpDest==NULL){
        printf("Destination file open failure.\n");
        return 0;
    }
    while((c=fgetc(fpSrc))!=EOF){   //��Դ�ļ��ж�ȡ����֪����β
        fputc(c, fpDest);
    }
    fclose(fpSrc);  //�ر��ļ�ָ�룬�ͷ��ڴ�
    fclose(fpDest);
    return 0;
}

int main()
{
    char  path1[512],path2[512];
    printf("Please input path1 path2.\n");
    scanf("%s %s",path1,path2);
    List(path1,path2,2);
    Delete(path1,path2,2);
    printf("All work has been done.\n");
    return 0;
}
