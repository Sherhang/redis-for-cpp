/*多线程版本redis接口
 *@yehang@sjtu.edu.cn
 *如果使用此api, 禁止使用单连接版本提供的disconnect接口, 当然你使用也没问题，只是被自动忽略
 *无需手动断开连接，所有连接会在析构函数里面断开
 *无需担心重复回收连接导致的内存溢出问题
 */
#pragma once

#include <vector>
#include <list>
#include <pthread.h>
#include "credis.h"

struct RedisConnects
{
    bool bIsAllConnOK;
    std::vector<redisContext*> connects;
    RedisConnects()
    {
	bIsAllConnOK = false;
	connects.clear();
    }
};

//互斥锁
class MutexGuard 
{
public:
	MutexGuard(pthread_mutex_t *pMutex)
	{
		_pMutex = pMutex;
		pthread_mutex_lock(_pMutex);
	};
	~MutexGuard()
	{
		pthread_mutex_unlock(_pMutex);
	};
private:
	pthread_mutex_t *_pMutex;
};

//连接池
class RedisConnectPool
{
public:
        RedisConnectPool();
        RedisConnectPool(bool lock);
	    ~RedisConnectPool();
        //初始化时会连接_iPoolSize 个 client，放在链表里面
	    bool init(int iPoolSize, const std::string& redisIP, int redisPort, const std::string& passwd);
	    //从链表头部取得一个连接
        redisContext * getConnect();
        //重复连接retry次, 这是为了防止取到NULL，基本不用
        redisContext * getConnect(int retry);
        //回收连接，放回链表末尾
        void releaseConnect(redisContext *redisConn);
        bool isUseful(redisContext* &redisConn);
protected:
        redisContext * connect();
	    bool initConnectPool();
	    //bool isUseful(redisContext* &redisConn);
	    bool reConnect(redisContext* &redisConn);

private:
	    int _iPoolSize;
	    int _iUsedCount;
        std::string _redisIP;
        std::string _passwd;
        int _redisProt;
        std::list<redisContext*> _connectPool;
	    pthread_mutex_t _mutex;
        bool _needPasswd;
        bool _lock;
public:
        int getUsedCount()
        {
            return _iUsedCount;
        }
        int getCanUseNum()
        {
            return _connectPool.size();
        }
};

