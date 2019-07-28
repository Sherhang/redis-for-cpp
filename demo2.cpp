/*基本使用
 * */
#include<iostream>
#include"credis.h"
#include<algorithm>
#include<iterator>
using namespace std;
int main()
{
    Redis redis;
    string ip="127.0.0.1";
    int port = 6379;
    string password="yehang0201";
    redis.init(ip, port, password);
    redis.connect();
    vector<string> keys={"str1","str2","str3","str4","str5"};
    vector<string> values={"1s","2s","3s","4s","5s"};
    redis.mset(keys,values);
    keys.clear();
    redis.keys("str?", keys);
    sort(keys.begin(),keys.end());
    cout<<"all keys are:"<<endl;
    copy(keys.begin(),keys.end(),ostream_iterator<string>(cout," "));
    cout<<endl;
    vector<string> v;
    redis.mget(keys,v);
    cout<<"values ars"<<endl;
    copy(v.begin(), v.end(),ostream_iterator<string>(cout, " "));cout<<endl;
    cout<<"delete all keys like str?"<<endl;
    for(int i=0;i<keys.size();++i)
    {
        redis.del(keys[i]);
    }
    string key = "str1";
    string t = redis.type(key);
    cout<<"type of str1 is "<<t<<endl;
    redis.disconnect();
    redis.disconnect();//重复dis看看
}
