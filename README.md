# C++ 高性能聊天室

基于 Muduo 网络库的高性能实时聊天室，后端使用 C++，前端使用 Next.js。

<img width="1389" height="745" alt="image" src="https://github.com/user-attachments/assets/31d52603-f1a3-42f7-b3ef-b4aca43748fd" />

## 项目结构

- `server/` — C++ Linux 服务端
- `client/` — 前端代码（Next.js Web 客户端）

## 技术栈

**后端**
- Muduo 网络框架（Reactor 模式，EPoll，one-loop-per-thread）
- MySQL（用户、房间数据持久化）
- Redis（Token 缓存、消息 Pub/Sub）
- JSON（HTTP/WebSocket 消息协议）
- 可选分布式模式：gRPC（服务间通信）+ Kafka（消息分发）

**前端**
- Next.js + TypeScript
- Tailwind CSS + MUI（Material UI）
- WebSocket 实时通信

## 部署模式

- **单机模式**：单一 `chat-room` 进程，处理所有 HTTP/WebSocket 连接、业务逻辑和数据库交互
- **分布式模式**（可选）：拆分为 Comet（网关）、Logic（业务逻辑）、Job（消息消费）三个服务，通过 gRPC 互通，Kafka 分发消息

## 快速开始

### 前置依赖

- GCC/G++（支持 C++17）、CMake 3.6+
- 库：`mysqlclient`、`hiredis`、`jsoncpp`、`libuuid`、`libssl`
- 服务：MySQL、Redis
- 分布式模式额外需要：gRPC、Protobuf、librdkafka

### 数据库初始化

```bash
mysql -u root -p < server/chatroom.sql
```

### 编译服务端

```bash
cd server
mkdir build && cd build

# 单机模式
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# 分布式模式
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_RPC=ON ..
make
```

### 运行服务端

修改 `server/application/chat-room/chat-room.conf` 中的数据库和网络配置，然后：

```bash
./bin/chat-room ../application/chat-room/chat-room.conf
```

### 运行 Web 前端

```bash
cd client/web
npm install
npm run dev       # 开发模式，访问 http://localhost:3000
```

生产部署：

```bash
npm run build
npm start
```
