/********************************************************************************************
 *        File:  bootstrap.c
 *     Version:  1.0.0
 *   Copyright:  2011 (c) Guo Wenxue <guowenxue@gmail.com>
 * Description:  This C code is the first stage bootloader(named bootstrap) 
                 main code, test on FL2440 board.
 *   ChangeLog:  1, Release initial version on "Tue Jul 12 16:43:18 CST 2011"
 *
 *******************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "yaffsfs.h"
#include "s3c_board.h"

#define MALLOC_SIZE             20

int mkdir(const char *mp, const char *name)
{
    char full_name[100]; 
    sprintf(full_name, "%s/%s", mp, name); 
    printf("Create directory [%s]\n", full_name);
    return yaffs_mkdir(full_name, S_IREAD| S_IWRITE);
}

int mkfile(const char *mp, const char *name)
{
    char full_name[100];
    char buf[100];
    int h; 
    
    sprintf(full_name, "%s/%s", mp, name); 

    h = yaffs_open(full_name, O_RDWR | O_CREAT | O_TRUNC, S_IREAD| S_IWRITE); 
    yaffs_write(h, name, strlen(name)); 
    yaffs_lseek(h, 0, SEEK_SET);
    memset(buf, 0, sizeof(buf));
    yaffs_read(h, buf, sizeof(buf));
    printf("Create File [%s] content: [%s]\n", full_name, buf);
    yaffs_close(h);

    return 0;
}

int rm(const char *mp, const char *name)
{
    char full_name[100]; 

    if(name)
        sprintf(full_name, "%s/%s", mp, name); 
    else
        strcpy(full_name, mp);
    
    printf("Remove %s\n", full_name);
    return yaffs_rm(full_name, 1);
}

int ls(const char *mp, const char *name)
{
    int recursive = 1;
    char full_name[100]; 

    if(name)
        sprintf(full_name, "%s/%s", mp, name); 
    else
        strcpy(full_name, mp);
    
    printf("List folder '%s' %s recursive:\n", full_name, recursive?"with":"without");
    return yaffs_ls(full_name, recursive);
}


void dump_directory_tree_worker(const char *dname,int recursive)
{
    yaffs_DIR *d;
    struct yaffs_dirent *de;
    struct yaffs_stat s;
    char str[1000];

    d = yaffs_opendir(dname);

    if(!d)
    {
        printf("opendir failed\n");
    }
    else
    {
        while((de = yaffs_readdir(d)) != NULL)
        {
            sprintf(str,"%s/%s",dname,de->d_name);

            yaffs_lstat(str,&s);

            printf("%s inode %d obj %x length %lld mode %X ",
                str,s.st_ino,de->d_dont_use, s.st_size,s.st_mode);
            switch(s.st_mode & S_IFMT)
            {
                case S_IFREG: printf("data file"); break;
                case S_IFDIR: printf("directory"); break;
                case S_IFLNK: printf("symlink -->");
                              if(yaffs_readlink(str,str,100) < 0)
                                printf("no alias");
                              else
                                printf("\"%s\"",str);
                              break;
                default: printf("unknown"); break;
            }
            printf("\n");

            if((s.st_mode & S_IFMT) == S_IFDIR && recursive)
                dump_directory_tree_worker(str,1);
        }

        yaffs_closedir(d);
    }
}


static void dump_directory_tree(const char *dname)
{   
    dump_directory_tree_worker(dname,1);
    printf("\n");
    printf("Free space in %s is %d\n\n",dname,(int)yaffs_freespace(dname));
}   

void dumpDir(const char *dname)
{  
    dump_directory_tree_worker(dname,0);
    printf("\n");
    printf("Free space in %s is %d\n\n",dname,(int)yaffs_freespace(dname));
}   


void yaffs_test(const char *mountpt)
{
    yaffs_start_up();

#if 0
    yaffs_mount(mountpt);
    ls(mountpt, NULL);

    mkdir(mountpt, "foo");
    mkfile(mountpt, "foo/f1");

    return ;
#endif

    yaffs_format(mountpt,0,0,0);

    yaffs_mount(mountpt);
    printf("'%s' mounted\n", mountpt);

    mkdir(mountpt, "foo");
    mkfile(mountpt, "foo/f1");

    mkfile(mountpt, "foo/f2");
    mkfile(mountpt, "foo/f3");
    mkfile(mountpt, "foo/f4");

    mkdir(mountpt, "bar");
    mkfile(mountpt, "bar/f1");
    ls(mountpt, NULL);

    rm(mountpt, "foo/f4");
    rm(mountpt, "bar");
    ls(mountpt, NULL);

    printf("unmount and remount\n\n"); 
    
    /*  Unmount/remount yaffs_trace_mask */
    yaffs_unmount(mountpt);
    yaffs_mount(mountpt);
    ls(mountpt, NULL);
}

int bootstrap_main(void)
{
    char *ptr = NULL;
    int rv = -1;

    console_serial_init();
    printf("\b\n");
    printf("\bBootstrap nandflash yaffs2 test Version 0.0.1\n");

    /*  armboot_start is defined in the board-specific linker script */
    mem_malloc_init (TEXT_BASE - CONFIG_SYS_MALLOC_LEN, CONFIG_SYS_MALLOC_LEN);

    ptr = (char *)malloc(MALLOC_SIZE);
    strncpy(ptr, "Hello World!\n", MALLOC_SIZE);
    printf("Malloc address: %p, string: %s\n", ptr, ptr);
    free(ptr);

    yaffs_test(YAFFSFS_MNT_POINT);

hang:
    while(1)
        ;

    return 0;
}

