/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<Email:guowenxue@gmail.com QQ:281143292>
 *                  All rights reserved.
 *
 *       Filename:  main.cpp
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(07/27/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "07/27/2012 03:39:53 PM"
 *                 
 ********************************************************************************/


/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/

#include <iostream>
#include "mytime.h"

int main (int argc, char **argv)
{
    using std::cout;
    using std::endl;

    Time aida(3, 35);
    Time tosca(2, 48);

    Time temp;

    cout << "Aida and Tosca:\n";
    cout<<aida<<"; "<<tosca<<endl ;

    temp = aida + tosca;
    cout<<"aida+tosca: "<<temp<<endl ;

    temp= aida*1.17;
    cout<<"tosca*1.17: "<<temp<<endl ;

    cout<<"10.0*tosca: "<<10.0*tosca<<endl ;

    return 0;
} /* ----- End of main() ----- */

