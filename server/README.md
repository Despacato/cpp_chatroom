# 项目说明

具体部署和编译方式见课件。

课程代码对应:

1 项目整体架构和技术选型及部署
- server/application/chat_room
- client/web  //客户端
## 服务端部署
```
cd server
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

如需要支持分布式功能
前提是已经编译支持grpc,kafka相关的库:
```
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_RPC=ON  ..
```
