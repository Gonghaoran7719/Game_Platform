# Game_Platform
大三在校学生，热爱IT  
民间自发组织的Study Team 完成的休闲游戏平台
# 项目展示
https://www.bilibili.com/video/BV1uQ4y1r7NJ?share_source=copy_web


## :ghost:
基本功能： 类似于Steam、TGP等<br>
小队语音，游戏辅助，游戏下载安装，游戏推荐，游戏动态、直播，<br>
## :ghost:
客户端：QT进行UI图形界面开发；
粗略分为：登录界面，主界面（游戏列表、房间创建、游戏动态、游戏推荐等），<br>
         游戏辅助界面（语音、群聊、管理房间）， <br>
         视屏（直播、房间成员视频上传录制等）； <br>
         管理者界面（游戏管理，上传，更新，删除，版本迭代等）；  <br>

服务器：Linux平台下开发，epoll IO模型，线程池，MySQL数据库，Redis数据库    <br>
       解决高并发、高可用
 ## :ghost:  
采用TCP协议，设计传输协议包，网络信息结构体  <br>
使用MD5加密  <br>
Nginx服务器，RTMP协议推流视屏  <br>
FFmpeg编码  <br>
OPenCV类库采集摄像头及进行视屏处理  <br>

# 版本
## :ghost: 0.0 

模型选择，界面草图设计
基本功能的实现方式
## :ghost: 1.0

实现了基本功能
## :ghost: 2.0

增加了网络游戏模块，设计了直播动态界面
## :ghost: 3.0

实现了文件传输，秒传，提升稳定性
