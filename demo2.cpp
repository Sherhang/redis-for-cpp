#include<iostream>
#include"credis.h"
#include"hiredis/hiredis.h"
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
    vector<string> keys;
    string pattern="string*";
    redis.keys(pattern, keys);
    sort(keys.begin(),keys.end());
    cout<<"all keys are:"<<endl;
    copy(keys.begin(),keys.end(),ostream_iterator<string>(cout," "));
    cout<<endl;

    for(int i=0;i<keys.size(); ++i)
    {
        string value, t;
        value = redis.get(keys[i]);
        t = redis.type(keys[i]);
        cout<<t<<" "<<value<<endl;
    }
    cout<<endl;
    string t;
    string key = "hash1";
    t = redis.type(key);
    cout<<t<<endl;
    redis.disconnect();
    //redis.disconnect();
}
