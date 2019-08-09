yehang@sjtu.edu.cn

2019/07/30

c++的redis的接口
redis版本:Redis server v=3.0.6 sha=00000000:0 malloc=jemalloc-3.6.0 bits=64 build=7785291a3d2152db
hiredis版本：https://github.com/redis/hiredis

使用说明：
依赖于hiredis静态库。
将redis.cpp, redis.h, redis_connect_pool.cpp, redis_connect_pool.h 添加到项目中即可。
makefile写法参见makefile。
提供了demo示例以供参考，直接make之后运行即可。

API功能介绍：提供常用redis命令接口，操作同redis终端。
提供部分高级接口如getKeys(), scan()。提供通用接口exec(), execScan()。
基本上redis的功能都可以通过三个通用接口实现。
提供连接池模式，自动管理连接。

