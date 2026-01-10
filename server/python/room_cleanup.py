#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pymysql
import redis
import schedule
import time
from datetime import datetime
import logging

# 配置日志
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    filename='room_cleanup.log'
)

# 数据库配置
MYSQL_CONFIG = {
    'host': 'localhost',
    'user': 'darren',  # 替换为你的MySQL用户名
    'password': '123456',  # 替换为你的MySQL密码
    'database': '0voice_chatroom',
    'charset': 'utf8mb4'
}

# Redis配置
REDIS_CONFIG = {
    'host': 'localhost',
    'port': 6379,
    'db': 0,
    'password': None  # 如果有密码，请设置
}

def cleanup_rooms():
    try:
        logging.info("开始清理房间数据...")
        
        # 连接MySQL
        mysql_conn = pymysql.connect(**MYSQL_CONFIG)
        mysql_cursor = mysql_conn.cursor()
        
        # 连接Redis
        redis_client = redis.Redis(**REDIS_CONFIG)
        
        try:
            # 1. 读取所有房间ID
            mysql_cursor.execute("SELECT room_id FROM room_info")
            rooms = mysql_cursor.fetchall()
            
            # 2. 删除Redis中的相关key
            for room in rooms:
                room_id = room[0]
                # 删除房间相关的Redis key             
                key = room_id
                try:
                    redis_client.delete(key)
                    logging.info(f"已删除Redis key: {key}")
                except Exception as e:
                    logging.error(f"删除Redis key失败 {key}: {str(e)}")
        
            # 3. 清空room_info表
            mysql_cursor.execute("TRUNCATE TABLE room_info")
            mysql_conn.commit()
            logging.info("已清空room_info表")
            
        except Exception as e:
            logging.error(f"处理过程中发生错误: {str(e)}")
            mysql_conn.rollback()
        finally:
            mysql_cursor.close()
            mysql_conn.close()
            redis_client.close()
            
        logging.info("清理房间数据完成")
        
    except Exception as e:
        logging.error(f"连接数据库失败: {str(e)}")

def main():
    logging.info("房间清理脚本启动")
    cleanup_rooms()
    # 设置每天凌晨2点执行
    schedule.every().day.at("02:00").do(cleanup_rooms)
    
    # 如果脚本启动时已经过了凌晨2点，立即执行一次
    current_hour = datetime.now().hour
    if current_hour >= 2:
        cleanup_rooms()
    
    # 保持脚本运行
    while True:
        schedule.run_pending()
        time.sleep(60)  # 每分钟检查一次是否需要执行任务

if __name__ == "__main__":
    main()