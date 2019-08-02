#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include "redis_connect_pool.h"

using namespace std;

//TC_ThreadLock RedisConnectPool::_lock;

RedisConnectPool::RedisConnectPool()
{
	pthread_mutex_init(&_mutex, NULL);

}

RedisConnectPool::~RedisConnectPool()
{
    {
	MutexGuard guard(&_mutex);
	list<redisContext*>::iterator it = _connectPool.begin();
	list<redisContext*>::iterator itEnd = _connectPool.end();
	for (; it != itEnd; ++it)
	{
	    //if(*it != NULL)
            {
                redisFree(*it);
                *it = NULL;
            }
	}
	_connectPool.clear();
    }

	pthread_mutex_destroy(&_mutex);
}

bool RedisConnectPool::init(int iPoolSize, const string& redisIP, int redisPort, const string& passwd)
{
    MutexGuard guard(&_mutex);    
    _redisIP = redisIP;
    _redisProt = redisPort;
    _passwd = passwd;
    _iPoolSize = iPoolSize;
    _iUsedCount = 0;
    _needPasswd = true;
    return initConnectPool();   
}

bool RedisConnectPool::initConnectPool()
{
	for (int i = 0; i < _iPoolSize; ++i)
	{
	    redisContext *redisConn = connect();
            if(redisConn)
            {
    	        _connectPool.push_back(redisConn);
            }
        }
	
	return true;
}

redisContext * RedisConnectPool::connect()
{
    redisContext *redisConn = NULL;
    redisConn = redisConnect(_redisIP.c_str(), _redisProt);
    if(redisConn != NULL && redisConn->err)
    {
        if (redisConn->errstr != NULL)
        {
        }
        
        if (NULL != redisConn)
        {
            redisFree(redisConn);
        }
        redisConn = NULL;
        return NULL;
    }
    if (!_needPasswd ) 
    {
        //debug 
        return redisConn;
    }
    std::string cmd = "AUTH " + (_passwd.empty()? "1245678" : _passwd);
    redisReply *reply = (redisReply*)::redisCommand(redisConn, cmd.c_str());
    if (reply == NULL && redisConn->err != 0)
    {
        return NULL;
    }

    if (reply->type == REDIS_REPLY_STATUS && strcasecmp(reply->str, "OK") != 0)
    {
        return NULL;
    }
    if (reply->type == REDIS_REPLY_ERROR) 
    {
        if (strcasecmp(reply->str, "ERR Client sent AUTH, but no password is set") == 0) 
        {
            _needPasswd = false;
            reConnect(redisConn);
        } 
        else 
        {
            return NULL;
        }
    }
    if(reply)
    {
        freeReplyObject(reply);
        reply = NULL;
    }
    
    return redisConn;
}

redisContext * RedisConnectPool::getConnect()
{
	redisContext *retConn = NULL;
	MutexGuard guard(&_mutex);    
	{
		if (_connectPool.empty())
		{
			return retConn;
		}
		else
		{
		    retConn = _connectPool.front();
		    _connectPool.pop_front();
                    _iUsedCount++;
		}
	}

	if (retConn == NULL)
	{
		reConnect(retConn);
	}

	return retConn;
}

redisContext * RedisConnectPool::getConnect(int retry)
{
    redisContext * context = getConnect();
    while(context == NULL && retry-->0)
    {
        context = getConnect();
    }
    if(context == NULL)
    {
        return NULL;
    }

    return context;
}

void RedisConnectPool::releaseConnect(redisContext *redisConn)
{
    if(redisConn == NULL)
        return ;
    if(find(_connectPool.begin(), _connectPool.end(), redisConn) != _connectPool.end())
    {
        return;
    }
    MutexGuard guard(&_mutex);    
	if(redisConn != NULL)
    {
            if(redisConn->err)
            {
                if (!reConnect(redisConn)) 
                {
                    return;
                }
            }
        if(isUseful(redisConn))
        {
	        _connectPool.push_back(redisConn);
            _iUsedCount--;
        }
	}

	return;
}

bool RedisConnectPool::reConnect(redisContext* &redisConn)
{
    if(redisConn)
    {
	redisFree(redisConn);
        redisConn = NULL;
    }
	redisConn = connect();
    if(redisConn == NULL)
        return false;
    
	return true;
}

bool RedisConnectPool::isUseful(redisContext* &redisConn)
{
	if (NULL == redisConn)
	{
		return false;
	}
    
	bool bPingRet = true;
	string command = "PING";
	redisReply *reply = (redisReply*)redisCommand(redisConn, command.c_str());
	if (reply == NULL && redisConn->err != 0)
	{
		bPingRet = false;
	}

	if (reply->type == REDIS_REPLY_STATUS)
	{
		string strRet = reply->str;
		if (strRet.find("PONG") != string::npos)
		{
			freeReplyObject(reply);
            reply = NULL;
		}
	}
    if(reply)
    {
	    freeReplyObject(reply);
        reply = NULL;
    }
	
	return bPingRet;
}
