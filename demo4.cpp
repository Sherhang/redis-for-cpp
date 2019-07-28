/*redis_connect_pool测试
 *
 * */

#include<iostream>
#include<algorithm>
#include<iterator>

#include"credis.h"
#include"redis_connect_pool.h"

using namespace std;

int main()
{   int pool_size = 2;
    string ip = "127.0.0.1";
    int port = 6379;
    string passwd ="yehang0201";
    RedisConnectPool rc_pool;
    if(!rc_pool.init(pool_size, ip, port, passwd))
    {
        cout<<"rc_pool init failure!"<<endl;
        exit(0);
    }
    //0
    cout<<"canUse num : "<<rc_pool.getConnectedNum()<<endl;
    cout<<"used num : "<<rc_pool.getUsedCount()<<endl;
    //1
    rc_pool.getConnect();
    cout<<"used num : "<<rc_pool.getUsedCount()<<endl;
    cout<<"canUse num : "<<rc_pool.getConnectedNum()<<endl;
    //2
    redisContext* context = rc_pool.getConnect();
    cout<<"used num : "<<rc_pool.getUsedCount()<<endl;
    cout<<"canUse num : "<<rc_pool.getConnectedNum()<<endl;
    rc_pool.releaseConnect(context);

    //3
    context = rc_pool.getConnect();
    cout<<"used num : "<<rc_pool.getUsedCount()<<endl;
    cout<<"canUse num : "<<rc_pool.getConnectedNum()<<endl;
    Redis redis(context);
    string v;
    v = redis.get("string1");
    cout<<"get key "<<v<<endl;
    //redis.disconnect();
    rc_pool.releaseConnect(context);//回收
    
    //4
    cout<<"used num : "<<rc_pool.getUsedCount()<<endl;
    cout<<"canUse num : "<<rc_pool.getConnectedNum()<<endl;
    context = rc_pool.getConnect();
    
    if(context==NULL)
        cout<<"no context"<<endl;
    rc_pool.releaseConnect(context);
    rc_pool.releaseConnect(context);//重复释放，内存溢出

}
