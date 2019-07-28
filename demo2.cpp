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
    {
        cout<<"hash操作："<<endl;
        vector<string> fields={"f1","f2","f3","f4","f5"};
        vector<string> values={"hv1","hv2","hv3","hv4","hv5"};
        redis.hmset("hash_test",fields,values);
        map<string,string> hash;
        redis.hgetall("hash_test",hash);
        for(map<string,string>::iterator it=hash.begin();it!=hash.end();++it)
        {
            cout<<it->first<<" "<<it->second<<endl;
        }
        cout<<endl;
        values.clear();
        redis.hmget("hash_test",fields,values);
        cout<<"hmget"<<endl;
        copy(values.begin(),values.end(),ostream_iterator<string>(cout," "));cout<<endl;
        redis.del("hash_test");
    }

    while(1);
    redis.disconnect();
    redis.disconnect();//重复dis看看
}
