# MiniScreencaster

- 语言:C++
- 环境：Linux(服务器) Windows10

提供点对点的实时投屏服务,暂时没有声音转发服务

# 注意事项
Screener(发送屏幕信息方)要先启动，然后再启动viewer(接收屏幕信息方)

# 使用
```git clone https://github.com/Monster12138/MiniScreencaster.git```
## 发送方
```MiniScreencaster/ScreenRecorder/Release/ScreenRecorder.exe```
## 接收方
```MiniScreencaster/viewer/Release/viewer.exe```
## 服务器
默认服务器地址是我的云服务器，可以提供转发服务
自建服务器:```./MiniScreencaster/server/serv```
