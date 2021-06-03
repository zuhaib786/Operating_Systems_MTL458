#include<iostream>
using namespace std;
int main()
{
    string s1, s2;
    cin>>s1>>s2;
    int final_dist = 0;
    for (int i = 0;i<s1.size();i++)
    {
        if (s1[i]=='+')
        {
            final_dist+=1;
        }
        else
        {
            final_dist-=1;
        }
        
    }
    cout<<final_dist<<endl;
}