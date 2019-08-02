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
    string ip="127.0.0.1", pwd="";
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
        values.clear();
        cout<<"测试scan:"<<endl;
        cout<<"next cursor is "<<redis.scan(0,"",-1,values)<<endl;
        cout<<"scan keys "<<endl;
        copy(values.begin(),values.end(),ostream_iterator<string>(cout," "));cout<<endl;
        for(int i=0; i<keys.size(); ++i)
        {
            redis.del(keys[i]);
        }
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
        redis.del("num");
        redis.del("z");
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
        redis.del("zset");

        cout<<"zIncrBy测试："<<endl;
        map<string,string> z;
        z["21"]="a";
        string score=redis.zincrby("zset",10,"a");
        cout<<"new score is "<<score<<endl;
        redis.del("zset");
    
        cout<<"zRange, zRevRange测试："<<endl;
        redis.zadd("zset",zset);
        vector<string> values;
        cout<< redis.zrange("zset",0,-1,values)<<endl;
        copy(values.begin(), values.end(),ostream_iterator<string>(cout," "));cout<<endl;
        redis.zrevrange("zset", 0, -1, values, true);
        copy(values.begin(), values.end(), ostream_iterator<string>(cout," "));cout<<endl;

        cout<<"zScore测试："<<endl;
        string sc = redis.zscore("zset","ab");
        cout<<sc<<endl;
    
        cout<<"zScan测试："<<endl;
        map<string,string> sm;
        cout<<"next cusor "<<redis.zscan("zset",0,"",10,sm)<<endl;
        for(map<string,string>::iterator it=sm.begin();it!=sm.end();++it)
        {
            cout<<it->first<<" "<<it->second<<endl;
        }
        cout<<endl;
        redis.del("zset");
    }
    {
        cout<<"lrange测试："<<endl;
        for(int i=0;i<10; ++i)
        {
            string cmd = "lpush list1 000"+redis.num2str<int>(i);
            redis.exec(cmd);
        }
        vector<string> v;
        redis.lrange("list1", 0, -1, v);
        copy(v.begin(), v.end(), ostream_iterator<string>(cout, " "));cout<<endl;
        redis.del("list1");
    }
    {
        cout<<"hmset,hgetall测试："<<endl;
        vector<string> k={"h1","h2","h3","h4"};
        vector<string> v={"hv1","hv2","hv3","hv4"};
        redis.hmset("hash",k,v);
        map<string, string> hash;
        redis.hgetall("hash",hash);
        for(map<string,string>::iterator it=hash.begin();it!=hash.end();++it)
        {
            cout<<it->first<<" "<<it->second<<endl;

        }
        cout<<endl;
        cout<<"hscan测试："<<endl;
        map<string,string> kv;
        cout<<"next cusor "<<redis.hscan("hash",0,"h?",10,kv)<<endl;
        for(map<string,string>::iterator it=kv.begin();it!=kv.end();++it)
        {
            cout<<it->first<<" "<<it->second<<endl;
        }
        cout<<endl;
        redis.del("hash");
    }
    {
        cout<<"exec测试: "<<endl;
        vector<string> v;
        redis.exec("set exec success");
        cout<<redis.get("exec")<<endl;
        redis.exec("zrange zset1 0 -1", v);
        cout<<"v.size() = "<<v.size()<<endl;
        copy(v.begin(), v.end(), ostream_iterator<string>(cout, " "));cout<<endl;
        redis.exec("scan 0 ", v);
        copy(v.begin(), v.end(), ostream_iterator<string>(cout, " "));cout<<endl;
        redis.exec("sadd set1 201");
        redis.exec("sadd set1 202");
        redis.exec("sadd set1 203");
        redis.exec("sadd set1 204");
        redis.exec("sadd set1 205");
        cout<<"sscan测试："<<endl;
        cout<<"next cursor is "<<redis.sscan("set1",0,"*0*",3,v)<<endl;
        copy(v.begin(), v.end(), ostream_iterator<string>(cout, " "));cout<<endl;

    }
    {
        cout<<"execScan测试："<<endl;
        vector<string> v;
        redis.execScan("scan 0",v);cout<<"scan 0"<<endl;
        copy(v.begin(),v.end(), ostream_iterator<string>(cout, " "));cout<<endl;
        redis.execScan("sscan set1 0",v);cout<<"sscan set1 0 count 10"<<endl;
        copy(v.begin(),v.end(), ostream_iterator<string>(cout, " "));cout<<endl;
        redis.execScan("hscan hash1 0 count 10",v);cout<<"hscan hash1 0 count 10"<<endl;
        copy(v.begin(),v.end(), ostream_iterator<string>(cout, " "));cout<<endl;
        redis.execScan("zscan zset1 0",v);cout<<"zscan zset 0"<<endl;
        copy(v.begin(),v.end(), ostream_iterator<string>(cout, " "));cout<<endl;
        cout<<"无效命令测试："<<endl;
        cout<<"返回值："<<redis.exec("sse ysh",v)<<endl;
        copy(v.begin(),v.end(), ostream_iterator<string>(cout, " "));cout<<endl;

    }
    //    cout<<"context exists"<<endl;
    redis.disconnect();//如非必要，不要使用这个接口
    //cout<<redis.get("string1")<<endl;//已经断开连接，会内存溢出
}
