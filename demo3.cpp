/*credis测试
 * */
#include<iostream>
#include"credis.h"
#include<algorithm>
#include<iterator>
using namespace std;
int main()
{
    Redis redis;
    string ip="127.0.0.1", pwd="yehang0201";
    int port = 6379;
    redis.init(ip, port, pwd);
    redis.connect();
    
    {
        cout<<"测试set, get, del:"<<endl;
        redis.set((string)"today", "sunday");//原函数是const，不加转换也可以
        string day;
        day = redis.get("today");
        cout<<day<<endl;
        int ret;
        ret=redis.del("today");
        cout<<"del "<<ret<<"个 today"<<endl;
        day = redis.get("today");
        if(day.empty())
            cout<<"not exist key today"<<endl;
    }
    {
        cout<<"测试mset,mget:"<<endl;
        vector<string> keys={"a","b","c","d","e"};
        vector<string> values={"AA","BB","CC","DD","EE"};
        if(redis.mset(keys,values))
            cout<<"mset success"<<endl;
        redis.mget(keys,values);
        copy(values.begin(),values.end(),ostream_iterator<string>(cout," "));cout<<endl;
    }
    {
        cout<<"测试incr, incrBy, incrByFloat, decr, decrBy"<<endl;
        redis.set("num", "0");
        int ret = redis.incr("num");
        cout<<"incr, num is "<<ret<<endl;
        ret = redis.incrby("num", 3);
        cout<<"incryBy 3, num is "<<ret<<endl;
        ret = redis.decr("num");
        cout<<"decr, num is "<<ret<<endl;
        ret = redis.decrby("num", 5);
        cout<<"decrBy 5, num is "<<ret<<endl;
        float retf = redis.incrbyfloat("num", -0.82);
        cout<<"-0.82, num is "<<retf<<endl;
        ret = redis.incrby("z", -3);
        string value;
        value = redis.get("z");
        cout<<"不存在key测试"<<endl<<value<<" "<<ret<<endl;
    }
    {
        cout<<"zAdd, zCard, zCount 测试："<<endl;
        map<string, string> zset;
        zset["010"]="ab";
        zset["011"]="cd";
        zset["012"]="ef";
        int ret=redis.zadd("zset",zset);
        cout<<"成功添加元素个数："<<ret<<endl;
        int count = redis.zcard("zset");
        cout<<"zset中元素个数是："<<count<<endl;
        int num = redis.zcount("zset",10, 12);
        cout<<"zset中分数在[10,12]的元素有"<<num<<"个"<<endl;
    }
    {
        cout<<"zIncrBy测试："<<endl;
        string score=redis.zincrby("zset",10,"ab");
        cout<<"new score is "<<score<<endl;
    }
    {
        cout<<"zRange, zRevRange测试："<<endl;
        vector<string> values;
        cout<< redis.zrange("zset",0,-1,values, true)<<endl;
        copy(values.begin(), values.end(),ostream_iterator<string>(cout," "));cout<<endl;
        redis.zrevrange("zset", 0, -1, values, true);
        copy(values.begin(), values.end(), ostream_iterator<string>(cout," "));cout<<endl;
    }
    {
        cout<<"zScore测试："<<endl;
        string sc = redis.zscore("zset","ab");
        cout<<sc<<endl;
    }
    {
        cout<<"zScan测试："<<endl;
        vector<string> v;
        redis.zscan("zset",0,"*",-1,v);
        copy(v.begin(),v.end(), ostream_iterator<string>(cout," "));
    }
    {
        cout<<"lrange测试："<<endl;
        vector<string> v;
        redis.lrange("list1", 0, -1, v);
        copy(v.begin(), v.end(), ostream_iterator<string>(cout, " "));cout<<endl;
    }
    
    //    cout<<"context exists"<<endl;
    redis.disconnect();
}
