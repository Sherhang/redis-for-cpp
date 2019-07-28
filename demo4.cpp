/*redis_connect_pool测试
 *
 * */

#include<iostream>
/*redis连接池方式使用
 *
 *
 * */
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
    cout<<"used num : "<<rc_pool.getUsedCount()<<endl;
    cout<<"canUse num : "<<rc_pool.getConnectedNum()<<endl;
    
    //1
    cout<<"used num : "<<rc_pool.getUsedCount()<<endl;
    cout<<"canUse num : "<<rc_pool.getConnectedNum()<<endl;
    rc_pool.getConnect();
    
    //2
    cout<<"used num : "<<rc_pool.getUsedCount()<<endl;
    cout<<"canUse num : "<<rc_pool.getConnectedNum()<<endl;
    redisContext* context = rc_pool.getConnect();
    rc_pool.releaseConnect(context);//回收

    //3
    cout<<"used num : "<<rc_pool.getUsedCount()<<endl;
    cout<<"canUse num : "<<rc_pool.getConnectedNum()<<endl;
    context = rc_pool.getConnect();
    Redis redis(context);//初始化，这种方式会自动禁用disconnect
    redis.set("str", "hello");
    string v;
    v = redis.get("str");
    cout<<"get key "<<v<<endl;
    redis.disconnect();//这句话无效
    rc_pool.releaseConnect(context);//回收
    
    //4
    cout<<"used num : "<<rc_pool.getUsedCount()<<endl;
    cout<<"canUse num : "<<rc_pool.getConnectedNum()<<endl;
    context = rc_pool.getConnect();    
    if(context==NULL)
        cout<<"no context"<<endl;
    Redis redis1(context);
    cout<<redis1.get("str")<<endl;

    rc_pool.releaseConnect(context);
    rc_pool.releaseConnect(context);//重复回收
    cout<<"used num : "<<rc_pool.getUsedCount()<<endl;
    cout<<"canUse num : "<<rc_pool.getConnectedNum()<<endl;
}
