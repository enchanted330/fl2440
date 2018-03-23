/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<Email:guowenxue@gmail.com QQ:281143292>
 *                  All rights reserved.
 *
 *       Filename:  stacktem.cpp
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(08/03/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "08/03/2012 05:49:15 PM"
 *                 
 ********************************************************************************/

#include <iostream>
#include <string>
#include <cctype>
#include "stacktp.h"

using std::cin;
using std::cout;

int main(int argc, char **argv)
{
    Stack<std::string>   st;
    char                 ch;
    std::string          po;

    cout<<"Please enter A to add a purchase order:\n"
        <<"P to process a PO, or Q to quite.\n";

    while(cin>>ch && 'Q'!=std::toupper(ch))
    {
        while(cin.get() != '\n')
            continue;

        if(!std::isalpha(ch))
        {
            cout<<'\a';
            continue;
        }

        switch(ch)
        {
            case 'A':
            case 'a':
                cout << "Enter a PO number to add:";
                cin >> po;
                if(st.isfull())
                    cout << "Stack already full!\n";
                else
                    st.push(po);

                break;
        
            case 'P':
            case 'p':
                if(st.isempty())
                    cout << "Stack already empty!\n";
                else
                {
                    st.pop(po);
                    cout << "PO #" << po << "popped\n";
                }

                break;

            default:
                break;
        }


        cout<<"Please enter A to add a purchase order:\n"
            <<"P to process a PO, or Q to quite.\n";
    
    }

    cout << "Bye\n";
    return 0;
}

