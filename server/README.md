# 项目说明

具体部署和编译方式见课件。

课程代码对应:

1 项目整体架构和技术选型及部署

- server/application/chat_room1
- client/web

2 注册登录接口设计与实现 - 注意修改application/CMakeLists.txt允许编译chat_room2
- server/application/chat_room2
- client/web2

3 固定话题聊天实现 - 注意修改application/CMakeLists.txt允许编译chat_room3
- server/application/chat_room3
- client/web2  //客户端 第三节没有改动

4 聊天记录拉取设计与实现- 注意修改application/CMakeLists.txt允许编译chat_room4
- server/application/chat_room4
- client/web4  //客户端 第四节课有改动，只按课时编号，跳过3的编号

5 自定义聊天室- 注意修改application/CMakeLists.txt允许编译chat_room5
- server/application/chat_room5
- client/web4  //客户端 第五节课没有改动

6 产品上云发布和性能测试- 注意修改application/CMakeLists.txt允许编译chat_room6
- 特别需要注意的是，这节课了加了grpc server，相应job模块的调用，如果之前没有学习grpc和安装grpc环境，则这里编译会报错的。
- server/application/chat_room6
- client/web4  //客户端 第六节课修复了部分bug但没有修改目录名

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