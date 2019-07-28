#include "credis.h"
#include <iostream>
#define DEBUG std::cout<<"[DEBUG] "<<__FILE__<<" "<<__func__<<" "<<__LINE__<<" "
#define REDIS_REPLY_STRING 1
#define REDIS_REPLY_ARRAY 2
#define REDIS_REPLY_INTEGER 3
#define REDIS_REPLY_NIL 4
#define REDIS_REPLY_STATUS 5
#define REDIS_REPLY_ERROR 6

Redis::Redis() : _context(NULL), _reply(NULL), _connected(false) {}
Redis::Redis(redisContext *context) : _context(context), _connected(false) {}
Redis::~Redis() 
{
    if(_connected)
    {
        //DEBUG<<"free _context"<<std::endl;
        ::redisFree(_context);
        _connected = false;
    }
}

void Redis::init(const std::string& ip, int port, const std::string& pwd)
{
    _ip = ip;
    _port = port;
    _pwd = pwd;
}

void Redis::connect()
{
    if(_connected)
    {
        //DEBUG
        return;
    }
    _connected = true;
    _context = ::redisConnect(_ip.c_str(), _port);
    if (_context && _context->err)
    {
        exit(EXIT_FAILURE);
    }
    if (auth(_pwd))
    {
    }
    else
    {
        exit(EXIT_FAILURE);
    }

}

void Redis::disconnect()
{
    if(_connected)
    {
        //DEBUG<<"free _context"<<std::endl;
        ::redisFree(_context);
        _connected = false;
    }
}


void Redis::freeReply()
{
    if(_reply)
    {
        ::freeReplyObject(_reply);
        _reply = NULL;
    }
}

void Redis::freeReply(redisReply * reply)
{
    if(reply)
    {
        ::freeReplyObject(reply);
        reply = NULL;
    }
}


bool Redis::isError(redisReply * reply)
{
    if (NULL == reply)
    {
        return true;
    }
    return false;
}

int64_t Redis::del(const std::string& key)
{
    int64_t ret=0;
    std::string cmd = "DEL " + key;
    if(execReplyInt(cmd, ret))
    {
        return ret;
    }
    DEBUG<<"execReplyInt error "<<std::endl;
}

bool Redis::exists(const std::string& key)
{  
    int64_t ret = 0;
    std::string cmd = "DEL " + key;
    execReplyInt(cmd, ret);
    return (ret > 0);
}

bool Redis::expire(const std::string& key, int64_t seconds)
{
    int64_t ret = 0;
    std::string cmd = "EXPIRE " + key + " " +  num2str<int64_t>(seconds);
    execReplyInt(cmd, ret);
    return (ret > 0);
}

bool Redis::expireat(const std::string& key, int64_t timestamp)
{
    int64_t ret = 0;
    std::string cmd = "EXPIREAT " + key + " " + num2str<int64_t>(timestamp);
    execReplyInt(cmd, ret);
    return (ret > 0);
}

bool Redis::keys(const std::string& pattern, std::vector<std::string>& key_list)
{
    std::string cmd = "KEYS " + pattern; 
    if (execReplyArray(cmd, key_list))
    {
        return true;
    }
    return false;
}

int64_t Redis::ttl(const std::string& key)
{
    int64_t ret = -3;
    std::string cmd = "TTL " + key;
    execReplyInt(cmd, ret);
    return ret;
}

std::string Redis::type(const std::string& key)
{
    std::string ret;
    std::string cmd = "TYPE " + key;
    execReplyString(cmd, ret);
    return ret;
}

bool Redis::set(const std::string& key, const std::string& value)
{
    std::string cmd = "SET " + key + " " + value;
    return execReplyStatus(cmd);
}

std::string Redis::get(const std::string & key)
{
    std::string value;
    if(_context==NULL)
    {
        return value;
    }
    
    redisReply* _reply = (redisReply*)::redisCommand(_context, "GET %s", key.c_str());
    if (!isError(_reply))
    {
        if (_reply->type == REDIS_REPLY_STRING)
        {
            value = std::string(_reply->str, _reply->len);
            freeReply(_reply);
            return value;
        }
        if (_reply->type == REDIS_REPLY_NIL)
        {
            freeReply(_reply);
            return value;
        }
    }
    freeReply(_reply);
    return value;
}

bool Redis::mset(const std::vector<std::string> & keys, const std::vector<std::string> & values)
{
    if(_context==NULL)
    {
        return false;
    }
    
    if(keys.size() != values.size())
    {
        return false;
    }
    
    int len = keys.size()+values.size();
    int argc = 1;
    size_t argvlen[len+1];
    argvlen[0] = 4; //"mget"
    
    const char* argv[len+1];
    char argv1[] = "MSET";
    argv[0] = argv1;

    for(size_t t=0; t<keys.size(); t++)
    {
        argvlen[argc] = keys[t].size();
        argv[argc] = keys[t].c_str();
        argc++;
        
        argvlen[argc] = values[t].size();
        argv[argc] = values[t].c_str();
        argc++;
    }
    //redis.mget(argc, argv, argvlen);
    redisReply* _reply = (redisReply*)::redisCommandArgv(_context, argc, argv, argvlen);
    if (_reply && _reply->type == REDIS_REPLY_STATUS )
    {
        if(strcasecmp(_reply->str, "OK") == 0)
        {
            freeReply(_reply);
            return true;
        }
        else
        {
        }
    }
    freeReply(_reply);
    
    return false;

}

bool Redis::mget(const std::vector<std::string>& keys, std::vector<std::string>& values)
{
    if(_context==NULL)
    {
        return false;
    }
    values.clear(); 
    uint32_t len = keys.size();
    values.reserve(len);
    uint32_t argc = 1;
    size_t argvlen[len+1];
    argvlen[0] = 4; //"mget"
    
    const char* argv[len+1];
    char argv1[] = "MGET";
    argv[0] = argv1;
    
    for(size_t t=0; t<keys.size(); t++)
    {
        argvlen[t+1] = keys[t].size();
        argv[t+1] = keys[t].c_str();
        argc++;
    }

    //redis.mget(argc, argv, argvlen);
    redisReply* _reply = (redisReply*)::redisCommandArgv(_context, argc, argv, argvlen);
    
    if (_reply && _reply->type == REDIS_REPLY_ARRAY)
    {
	    for (unsigned int i = 0; i < _reply->elements; i++)
	    {
                std::string strTemp(_reply->element[i]->str,_reply->element[i]->len);
                if(strTemp.size() <= 0)
                    strTemp="";
                values.push_back(strTemp);
	    }
    }

    freeReply(_reply);
    return true;
}


int64_t Redis::incr(const std::string& key)
{
    int64_t ret = 0;
    std::string cmd =  "INCR " + key;
    execReplyInt(cmd, ret);
    return ret;
}

int64_t Redis::incrby(const std::string& key, int64_t incr)
{
    int64_t ret = 0;
    std::string cmd =  "INCRBY " + key + " " + num2str<int64_t>(incr);
    execReplyInt(cmd, ret);
    return ret;
}

float Redis::incrbyfloat(const std::string& key, float incr)
{
    float ret  = 0;
    std::string cmd =  "INCRBYFLOAT " + key + " " + num2str<float>(incr);
    std::string s;
    if (execReplyString(cmd, s))
    {
        ret = str2num<float>(s);
        return ret;
    }
    return ret;
}

int64_t Redis::decr(const std::string& key)
{
    int64_t ret = 0;
    std::string cmd =  "DECR " + key;
    execReplyInt(cmd, ret);
    return ret;
}

int64_t Redis::decrby(const std::string& key, int64_t decr)
{
    int64_t ret = 0;
    std::string cmd =  "DECRBY " + key + " " + num2str<int64_t>(decr);
    execReplyInt(cmd, ret);
    return ret;
}

int64_t Redis::zadd(const std::string& key, const std::map<std::string, std::string>& members)
{
    int64_t ret = -1;
    std::string cmd =  "ZADD " + key;
    std::map<std::string, std::string>::const_iterator it = members.begin();
    for (; it != members.end(); ++it)
    {
        cmd += " " + it->first + " " + it->second;
    }
    execReplyInt(cmd, ret);
    return ret;
}

int64_t Redis::zcard(const std::string& key)
{
    int64_t ret = -1;
    std::string cmd =  "ZCARD " + key;
    execReplyInt(cmd, ret);
    return ret;
}

int64_t Redis::zcount(const std::string& key, double min, double max)
{
    int64_t ret = -1;
    std::string cmd = "ZCOUNT " + key + " " + num2str<double>(min) + " " + num2str<double>(max);
    //DEBUG<<cmd<<std::endl;
    execReplyInt(cmd, ret);
    return ret;
}

std::string Redis::zincrby(const std::string& key, double incr, const std::string& member)//TODO
{
    std::string ret;
    std::string cmd = "ZINCRBY " + key + " " + num2str<double>(incr) + " " + member;
    execReplyString(cmd, ret);
    return ret;
}

bool Redis::zrange(const std::string& key, int64_t start, int64_t stop, std::vector<std::string>& values, bool with_scores)
{
    std::string cmd = "ZRANGE " + key + " " + num2str<int64_t>(start) + " "
        + num2str<int64_t>(stop) + (with_scores ? " WITHSCORES" : "");
    values.clear();
    if (execReplyArray(cmd, values))
    {
        return true;
    }
    return false;
}

bool Redis::zrevrange(const std::string& key, int64_t start, int64_t stop, std::vector<std::string>& values, bool with_scores)
{
    std::string cmd = "ZREVRANGE " + key + " " + num2str<int64_t>(start) + " "
        + num2str<int64_t>(stop) + (with_scores ? " WITHSCORES" : "");
    values.clear();
    if (execReplyArray(cmd, values))
    {
        return true;
    }
    return false;
}

int64_t Redis::zremrangebyrank(const std::string& key, int64_t start, int64_t stop)
{
    int64_t ret = -1;
    std::string cmd = "ZREMRANGEBYRANK " + key + " " + num2str<int64_t>(start) + " " + num2str<int64_t>(stop);
    execReplyInt(cmd, ret);
    return ret;
}

int64_t Redis::zremrangebyscore(const std::string& key, double min, double max)
{
    int64_t ret = -1;
    std::string cmd = "ZREMRANGEBYSCORE " + key + " " + num2str<double>(min) + " "
        + num2str<double>(max);
    execReplyInt(cmd, ret);
    return ret;
}

std::string Redis::zscore(const std::string& key, const std::string& member)
{
    std::string score;
    std::string cmd = "ZSCORE " + key + " " + member;
    execReplyString(cmd, score);
    return score;
}

// TODO 返回list，yhh
bool Redis::zscan(const std::string& key, int64_t cursor, const std::string& pattern, int64_t count,
                    std::vector<std::string> & values)
{
    if (count <= 0)
    {
        // DEBUG
        return false;
    }
    std::string cmd = "ZSCAN " + key + " " + num2str<int64_t>(cursor)
                    + (pattern.empty() ? "" : " PATTERN " + pattern)
                    + (count < 0 ? "" : " COUNT " + num2str<int64_t>(count));
    if (execReplyArray(cmd, values))
    {
        return true;
    }
    return false;
}


bool Redis::hgetall(const std::string& key, std::map<std::string, std::string>& value)
{
    if(_context == NULL) 
    {
        return false;
    }

    value.clear();
    redisReply* _reply = (redisReply*)::redisCommand(_context, "HGETALL %s", key.c_str());
    if (!isError(_reply)) 
    {
        if (_reply->type == REDIS_REPLY_ERROR) 
        {
            freeReply(_reply);
            return false;
        }
        if (_reply->type == REDIS_REPLY_ARRAY) 
        {
            for (size_t i = 0; i < _reply->elements; i = i+2) 
            {
                std::string key(_reply->element[i]->str, _reply->element[i]->len);
                std::string val(_reply->element[i+1]->str, _reply->element[i+1]->len);
                value[key] = val;
            }
            freeReply(_reply);
            return true;
        }
    }
    freeReply(_reply);
    return false;
}

bool Redis::lrange(const std::string& key, int64_t start, int64_t end, std::vector<std::string>& value)
{
    if(_context == NULL)
    {
        return false;
    }
    std::string cmd = "LRANGE " + key + " " + num2str<int64_t>(start) + " " + num2str<int64_t>(end) ;
    redisReply* _reply = (redisReply*)::redisCommand(_context, cmd.c_str());
    if (!isError(_reply)) 
    {
        if (_reply->type == REDIS_REPLY_ERROR) 
        {
            freeReply(_reply);
            return false;
        }
        if (_reply->type == REDIS_REPLY_ARRAY) 
        {
            value.reserve(_reply->elements);
            for (size_t i = 0; i < _reply->elements; ++i) 
            {
		value.push_back(_reply->element[i]->str);
            }
            freeReply(_reply);
            return true;
        }
    }
    freeReply(_reply);
    return false;
}

bool Redis::exec(const std::string& cmd)
{
    if(_context==NULL) 
    {
        return false;
    }
    redisReply* _reply = (redisReply*)::redisCommand(_context, cmd.c_str());
    if(isError(_reply))
    {
        freeReply(_reply);
        return false;
    }
    return true;
}

bool Redis::exec(const std::string& cmd, std::vector<std::string>& values)
{
    values.clear();
    if(_context==NULL) 
    {
        return false;
    }
    redisReply* _reply = (redisReply*)::redisCommand(_context, cmd.c_str());
    if(isError(_reply))
    {
        freeReply(_reply);
        return false;
    }
    //DEBUG<<"_reply->type = "<<_reply->type<<std::endl;
    switch (_reply->type)
    {
        case REDIS_REPLY_ERROR:
        {
            freeReply(_reply);
            return false;
        }
        break;

        case REDIS_REPLY_ARRAY:
        {
            int32_t elements = _reply->elements;
            values.reserve(elements);
            for(int32_t i=0; i<elements; ++i)
            {
                std::string strTemp(_reply->element[i]->str, _reply->element[i]->len);
                values.push_back(strTemp);
            }
            freeReply(_reply);
            return true;
        }
        break;
        
        case REDIS_REPLY_INTEGER:
        {
            std::string num = num2str<int64_t>(_reply->integer);
            values.push_back(num);
            freeReply(_reply);
            return true;
        }
        break;

        case REDIS_REPLY_STRING:
        {
            std::string tmp = _reply->str;
            values.push_back(tmp);
            freeReply(_reply);
            return true;
        }
        break;

        case REDIS_REPLY_STATUS:
        {
            std::string tmp = _reply->str;
            values.push_back(tmp);
            freeReply(_reply);
            return true;
        }
        break;

        case REDIS_REPLY_NIL:
        {
            freeReply(_reply);
            return true;
        }
        break;

        default:
        {
            freeReply(_reply);
            return false;
        }
    }
}

//private:
bool Redis::execReplyStatus(const std::string& cmd)
{
    if(_context==NULL)
    {
        //DEBUG<<"_contex == NULL"<<std::endl;
        return false;
    }
    redisReply* _reply = (redisReply*)::redisCommand(_context, cmd.c_str());
    if (!isError(_reply))
    {
        //DEBUG<<"_reply->type "<<_reply->type<<std::endl;
        //DEBUG<<"_reply->str "<<_reply->str<<std::endl;
        if (_reply->type == REDIS_REPLY_STATUS && strcasecmp(_reply->str, "OK") == 0)
        {
            freeReply(_reply);
            return true;
        }
    }
    freeReply(_reply);
    return false;
}

bool Redis::execReplyString(const std::string& cmd, std::string& ret)
{
    if(_context==NULL)
    {
        //DEBUG<<"_context is NULL"<<std::endl;
        return false;
    }
    redisReply* _reply = (redisReply*)::redisCommand(_context, cmd.c_str());
    if (!(isError(_reply)))
    {
        if (_reply->type == REDIS_REPLY_STRING || _reply->type == REDIS_REPLY_STATUS)// status --> string
        {
            ret = _reply->str;
            freeReply(_reply);
            return true;
        }

        if (_reply->type == REDIS_REPLY_ERROR)
        {
            std::string reason = _reply->str;
            reason.resize(_reply->len);
            freeReply(_reply);
            return false;
        }
    }
    else
        DEBUG<<"isError(_reply)"<<std::endl;
    freeReply(_reply);
    return false;
}


bool Redis::execReplyInt(const std::string& cmd, int64_t& ret)
{
    if(_context==NULL)
    {
        return false;
    }

    redisReply* _reply = (redisReply*)::redisCommand(_context, cmd.c_str());
    if (!isError(_reply))
    {
        //DEBUG<<"_reply->type "<<_reply->type<<std::endl;
        if (_reply->type == REDIS_REPLY_INTEGER)
        {
            ret = _reply->integer;
            freeReply(_reply);
            return true;
        }
        if (_reply->type == REDIS_REPLY_ERROR)
        {
            std::string reason = _reply->str;
            reason.resize(_reply->len);
            freeReply(_reply);
            return false;
        }
    }
    freeReply(_reply);
    return false;
}

bool Redis::execReplyArray(const std::string& cmd, std::vector<std::string>& ret)
{
    if(_context==NULL)
    {
        return false;
    }

    redisReply* _reply = (redisReply*)::redisCommand(_context, cmd.c_str());
    if (!isError(_reply))
    {
        if (_reply->type == REDIS_REPLY_ARRAY)
        {
            int32_t elements = _reply->elements;
            ret.reserve(elements);
            for(int32_t i=0; i<elements; ++i)
            {
                std::string strTemp(_reply->element[i]->str, _reply->element[i]->len);
                ret.push_back(strTemp);
            }
            freeReply(_reply);
            return true;
        }
    }
    freeReply(_reply);
    return false;
}

bool Redis::auth(const std::string& value)
{
    std::string cmd = "AUTH " + value;
    return execReplyStatus(cmd);
}
