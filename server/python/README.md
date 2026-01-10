# 这个目录只供有需要的同学
主要用于线上部署时，避免每天被用户创建各种聊天室以及发各种消息
执行这个脚本每天凌晨2点清除聊天室以及聊天记录

使用说明：

1. 首先需要安装必要的Python包：
```bash
pip install pymysql redis schedule
```

2. 配置文件权限：
```bash
chmod +x room_cleanup.py
```

3. 修改配置：
- 修改 `MYSQL_CONFIG` 中的数据库连接信息
- 修改 `REDIS_CONFIG` 中的Redis连接信息
- 根据实际情况修改需要删除的Redis key pattern

4. 运行脚本：
```bash
python3 room_cleanup.py
```

5. 后台运行，可以使用脚本
修改mange.sh脚本
主要修改(根据自己实际路径修改)：
```
# 日志文件
LOG_FILE="/opt/chatroom/logs/room_cleanup.log"
# Python 脚本路径
SCRIPT_PATH="/opt/chatroom/scripts/room_cleanup.py"
```

```
# 启动程序
./manage.sh start

# 查看状态
./manage.sh status

# 停止程序
./manage.sh stop

# 重启程序
./manage.sh restart
```
