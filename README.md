
# SBN_RaspBerryPi

草这树莓派连接虚的一比不知道哪天就炸了要重装系统
upd:草我树莓派改了个host结果就能访问github了push的比我windows挂着梯子还快可恶

---

*2021-05-05upd:*

三个程序主要在armBot/Main下

* local_main_control.c主要负责往PCA9685里写指令从而控制舵机

  编译命令:gcc local_main_control.c -o local_main_control -lwiringPi

* face_rec_streamer.py主要负责从摄像头视频抽帧分析人脸位置人工智能识别一手，然后推流到服务器

  原本推的rtmp，结果发现flashgg了，又想推http-flv，结果发现nginx装好了再加http-flv-module太麻烦，于是推的hls

  因为运算速度远低于推流速度，为了防止堵塞通道和卡顿，又懒得调多线程（树莓派写代码体验差得离谱）索性直接20针分析一次，推流到http://123.56.12.187/hls/room1.m3u8，手机浏览器直接访问就可以播放（但是我大概率没开）（而且服务器只租了三个月估计很快也gg）

* get_data_from_server.py主要是从onenet上接受来自微信小程序的指令信息
* 三个程序间都是socket嵌套字通讯（草十分原始）然后c是server，两个python是client

总之先这样
