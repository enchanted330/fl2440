/********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<Email:guowenxue@gmail.com QQ:281143292>
 *                  All rights reserved.
 *
 *       Filename:  stacktp.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(08/03/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "08/03/2012 05:36:38 PM"
 *                 
 ********************************************************************************/

#ifndef __STACKTP_H
#define __STACKTP_H

template <class T>  class Stack
{
    private:
        enum           { MAX = 10 };
        T              items[MAX];
        int            top;

    public:
        Stack()
        {
            top = 0;
        }

        bool isempty()
        {
            return 0 == top;
        }

        bool isfull()
        {
            return MAX == top;
        }

        bool push(const T & item);
        bool pop(T &itme);
};

template <class T> bool Stack<T>::push(const T & item)
{
    if(top < MAX)
    {
        items[top++] = item;
        return true;
    }
    else
        return false;
}

template <class T> bool Stack<T>::pop(T &item)
{
    if(top > 0)
    {
        item = items[--top];
        return true;
    }
    else
        return false;

}

#endif
