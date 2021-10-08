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
        //得到读取文件的绝对路径名
    	snprintf(dir, 512,"%s/%s",path,ent->d_name);
	    snprintf(dir2, 512,"%s/%s",path2,ent->d_name);
        //得到文件信息
    	lstat(dir, &statbuf);
        //判断是目录还是文件，文件则返回1
    	if(S_ISDIR(statbuf.st_mode) )
       	{
         //排除当前目录和上级目录
            if(strcmp(".",ent->d_name) == 0 || strcmp( "..",ent->d_name) == 0)
            {
            continue;
            }
            //如果是子目录,递归调用函数本身,实现子目录中文件遍历
			// 确定文件是否存在，存在则返回0，否则返回-1
	        if (access(dir2,0)==-1)
	        {
            mkdir(dir2, S_IRWXU);//建立一个目录，拥有读，写和操作权限
	        }
			//递归调用，遍历子目录中文件
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
		//lstat函数获取一些文件相关信息
		lstat(dir, &statbuf);
		if(S_ISDIR(statbuf.st_mode) )//是目录的情况
		{
           		if(strcmp( ".",ent->d_name) == 0||strcmp( "..",ent->d_name) == 0)
            		{
                		continue;
            		}
            		if (access(dir2,0)==-1)
			{
				Delete(dir2,dir,indent+4 );
				remove(dir);//删除一个文件
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
    FILE *fpSrc, *fpDest;  //定义两个指向文件的指针
    fpSrc = fopen(path1, "rb");    //以读取二进制的方式打开源文件
    if(fpSrc==NULL){
        printf("Unable to open source file.\n");  //源文件不存在的时候提示错误
        return 0;
    }
    fpDest = fopen(path2, "wb");  // //以写入二进制的方式打开目标文件
    if(fpDest==NULL){
        printf("Destination file open failure.\n");
        return 0;
    }
    while((c=fgetc(fpSrc))!=EOF){   //从源文件中读取数据知道结尾
        fputc(c, fpDest);
    }
    fclose(fpSrc);  //关闭文件指针，释放内存
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
