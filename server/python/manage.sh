#!/bin/bash

# 保存进程 ID 的文件
PID_FILE="/home/liaoqingfu/github-chatroom/server/python/room_cleanup.pid"
# 日志文件
LOG_FILE="/home/liaoqingfu/github-chatroom/server/python/logs/room_cleanup.log"
# Python 脚本路径
SCRIPT_PATH="/home/liaoqingfu/github-chatroom/server/python/room_cleanup.py"

start() {
    if [ -f $PID_FILE ]; then
        echo "程序已经在运行中"
        exit 1
    fi
    
    nohup python3 $SCRIPT_PATH > $LOG_FILE 2>&1 &
    echo $! > $PID_FILE
    echo "程序已启动，PID: $!"
}

stop() {
    if [ -f $PID_FILE ]; then
        PID=$(cat $PID_FILE)
        kill $PID
        rm $PID_FILE
        echo "程序已停止"
    else
        echo "程序未运行"
    fi
}

status() {
    if [ -f $PID_FILE ]; then
        PID=$(cat $PID_FILE)
        if ps -p $PID > /dev/null; then
            echo "程序正在运行，PID: $PID"
        else
            echo "程序已崩溃，PID 文件存在但进程不存在"
            rm $PID_FILE
        fi
    else
        echo "程序未运行"
    fi
}

case "$1" in
    start)
        start
        ;;
    stop)
        stop
        ;;
    restart)
        stop
        sleep 2
        start
        ;;
    status)
        status
        ;;
    *)
        echo "使用方法: $0 {start|stop|restart|status}"
        exit 1
        ;;
esac