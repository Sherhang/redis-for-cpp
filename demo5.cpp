#include <iostream>
#include "credis.h"
#include <algorithm>
#include <iterator>
#include <fstream>
using namespace std;

int main()
{
    Redis redis;
    redis.init("127.0.0.1",6379,"yehang0201");
    redis.connect();
    for(char k='a';k<='z';++k)
    {
    vector<string> keys(50000);
    vector<string> vals(50000);
    for (int i =0; i<keys.size(); ++i)
    {
        keys[i] = redis.num2str<int>(i);
        if(keys[i].size()<8)
        {
            string num = "00000000";
            keys[i]=num.substr(keys[i].size()) + keys[i] + k ;
        }
        vals[i]= keys[i];
        for(int j =0;j<vals[i].size();++j)
        {
            if(vals[i][j]<='9'&& vals[i][j]>='0')
                vals[i][j]=vals[i][j]-'0'+'a';
        }
        random_shuffle(vals[i].begin(),vals[i].end());//打乱
    }
    //cout<<keys[987]<<" "<<vals[987]<<endl;
    redis.mset(keys,vals);

    }
    vector<string> v;
    redis.getKeys("string", "", 10000, v);
    //redis.keys("*",v);
    
    ofstream file_for_write;
    file_for_write.open("./file/data.txt", ios::trunc);
    file_for_write << "get all keys:"<<endl;
    int ka=1;
    for(int i=0;i<v.size();++i,++ka)
    {
        file_for_write << v[i] <<" ";
        if(ka%10 ==0)
            file_for_write<<endl;
    }
    file_for_write<<endl<<endl<<endl<<endl;
    vector<string> vs;
    redis.mget(v, vs);
    file_for_write<<"get values:"<<endl;
    int kt=1;
    for(int i=0;i<vs.size();++i,++kt)
    {
        file_for_write << vs[i] << " ";
        if(kt >= 10)
        {
            kt = 0x01;
            file_for_write<<endl;
        }
    }

    cout<<endl;
}
