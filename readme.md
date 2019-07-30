# yehang@sjtu.edu.cn
# 2019/07/30
c++的redis的接口
redis版本:Redis server v=3.0.6 sha=00000000:0 malloc=jemalloc-3.6.0 bits=64 build=7785291a3d2152db
hiredis版本：https://github.com/redis/hiredis

#使用说明：
#依赖于hiredis静态库
#将redis.cpp, redis.h, redis_connect_pool.cpp, redis_connect_pool.h 添加到项目中即可。
makefile写法参见makefile
#提供了两个demo文件，demo2是单连接的使用示例，demo4是多连接的使用示例。
