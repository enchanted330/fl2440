/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  sqlite_test.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(11/29/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "11/29/2012 10:09:29 AM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sqlite3.h>

#define SQL_CMD_LEN     256

int data_callback(void * para, int n_column, char ** column_value, char ** column_name)
{
    char     *arg = (char *)para;
    int      i;

    printf("%s", arg);

    for(i=0; i<n_column; i++)
    {
        printf("%4s: %s\n", column_name[i], column_value[i]);
    }

    return 0;
}

/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    int               rv;
    sqlite3           *db;
    char              *errmsg = NULL;
    char              sqlcmd[SQL_CMD_LEN];

    /* open the database.db, if it not exist then create it */
    if(SQLITE_OK != (rv=sqlite3_open("database.db", &db)) )
    {
        printf("Open/create database failed\n");
        return -1;
    }

    /* create a table if it doesn't exist  */
    strncpy(sqlcmd, "create table if not exists cplus(ID integer primary key autoincrement, name nvarchar(32), time not null default current_timestamp)", sizeof(sqlcmd));
    if(SQLITE_OK != (rv=sqlite3_exec(db, sqlcmd, NULL, NULL, &errmsg)) )
    {
        printf("create ID table failed [%d]: %s\n", rv, errmsg);
        sqlite3_free(errmsg);
    }

    //snprintf(sqlcmd, sizeof(sqlcmd), "insert into cplus(name) values('%s')", "guowenxue");
    snprintf(sqlcmd, sizeof(sqlcmd), "insert into cplus values(1, '%s', datetime('2012-01-01 20:35:10'))", "guowenxue");
    if(SQLITE_OK != (rv=sqlite3_exec(db, sqlcmd, NULL, NULL, &errmsg)) )
    {
        printf("Add item into cplus failed [%d]: %s\n", rv, errmsg);
        sqlite3_free(errmsg);
    }

    //snprintf(sqlcmd, sizeof(sqlcmd), "insert into cplus(name) values('%s')", "jason");
    snprintf(sqlcmd, sizeof(sqlcmd), "insert into cplus values(2, '%s', datetime('now'))", "jason");
    if(SQLITE_OK != (rv=sqlite3_exec(db, sqlcmd, NULL, NULL, &errmsg)) )
    {
        printf("Add item into cplus failed [%d]: %s\n", rv, errmsg);
        sqlite3_free(errmsg);
    }

    //snprintf(sqlcmd, sizeof(sqlcmd), "insert into cplus(name) values('%s')", "adrian");
    snprintf(sqlcmd, sizeof(sqlcmd), "insert into cplus values(3, '%s', datetime('now'))", "adrian");
    if(SQLITE_OK != (rv=sqlite3_exec(db, sqlcmd, NULL, NULL, &errmsg)) )
    {
        printf("Add item into cplus failed [%d]: %s\n", rv, errmsg);
        sqlite3_free(errmsg);
    }

    strncpy(sqlcmd, "select * from cplus", sizeof(sqlcmd));
    if(SQLITE_OK != (rv=sqlite3_exec(db, sqlcmd, data_callback, "\n==== Query all the items ====\n", &errmsg)) )
    {
        printf("Query all items from cplus failed [%d]: %s\n", rv, errmsg);
        sqlite3_free(errmsg);
    }

    strncpy(sqlcmd, "select * from cplus where name='guowenxue'", sizeof(sqlcmd));
    if(SQLITE_OK != (rv=sqlite3_exec(db, sqlcmd, data_callback, "\n==== Query all guowenxue ====\n", &errmsg)) )
    {
        printf("Query guowexue from cplus failed [%d]: %s\n", rv, errmsg);
        sqlite3_free(errmsg);
    }

    printf("\n==== delete all guowenxue in the table ====\n");
    strncpy(sqlcmd, "delete from cplus where name='guowenxue'", sizeof(sqlcmd));
    if(SQLITE_OK != (rv=sqlite3_exec(db, sqlcmd, NULL, NULL, &errmsg)) )
    {
        printf("Delete guowenxue from cplus failed [%d]: %s\n", rv, errmsg);
        sqlite3_free(errmsg);
    }

    strncpy(sqlcmd, "select * from cplus", sizeof(sqlcmd));
    if(SQLITE_OK != (rv=sqlite3_exec(db, sqlcmd, data_callback, "\n==== Query all the items ====\n", &errmsg)) )
    {
        printf("Query all items from cplus failed [%d]: %s\n", rv, errmsg);
        sqlite3_free(errmsg);
    }

    {
        char   **result;
        int    rows, cols;
        int    i, j, index;

        strncpy(sqlcmd, "select * from cplus", sizeof(sqlcmd));
        if(SQLITE_OK != (rv=sqlite3_get_table(db, sqlcmd, &result, &rows, &cols, &errmsg)) )
        {
            printf("%s command failed: %s\n", sqlcmd, errmsg);
        }

        printf("rows:%d cols:%d\n", rows, cols);

        index = cols;

        for(i=0; i<rows; i++)
        {
            for(j=0; j<cols; j++)
            {
                printf("KeyName: %4s  KeyValue: %s\n", result[j], result[index]);
                index ++;
            }
        }

        sqlite3_free_table(result);
    }

cleanup:
    sqlite3_close(db);
    return 0;
} /* ----- End of main() ----- */

