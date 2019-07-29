#include <iostream>
#include <hiredis/hiredis.h>
using namespace std;
int main()
{
    redisContext *conn = redisConnect("127.0.0.1", 6379);
    if (conn != NULL && conn->err) {
        printf("connection error: %s\n", conn->errstr);
        return 0;
    }

    redisReply *reply;
    reply = (redisReply*)redisCommand(conn, "auth %s", "yehang0201");
    reply = (redisReply*)redisCommand(conn, "SET %s %s", "foo", "bar");
    freeReplyObject(reply);

    reply =(redisReply*) redisCommand(conn, "GET %s", "foo");
    std::cout<< reply->str<<std::endl;
    reply = (redisReply*) redisCommand(conn, "lrange list1 0 %s","-1");
    cout<<reply->str<<endl;
    freeReplyObject(reply);
    redisFree(conn);
    return 0;
}
