# Simple-multi-process-server
[![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://opensource.org/licenses/MIT)

## Introduction

本项目为C++编写的Web服务器, 能解析基本的GET请求, 可处理静态资源

## Environment

* OS: Ubuntu 20.04 LTS
* Compiler: g++ version 9.4.0

## Build & Usage

g++ simple-server.cpp -o simple-server && ./simple-server

## Technical points

* 主进程只负责链接请求, 子进程负责解析请求并传递数据
* 使用信号处理让处理完请求的子进程及时挂掉

## Difficulties while implementing

* 当请求路径不存在时如何处理
* 如何不产生僵尸进程(signal())
* 如何传送文件(sendfile)
* strtok()切割字符串的使用(卡了好久好久)

## Demo

正常请求

[![image.png](https://i.postimg.cc/7LsKsNYW/image.png)](https://postimg.cc/XZ5KXd5f)

错误处理

[![Error-handle.png](https://i.postimg.cc/GtG2gqrP/Error-handle.png)](https://postimg.cc/zHD8Vwy3)



## Test

并发链接测试还行, 直接把我1G单核服务器搞崩了, 不得不重启

[![Result.png](https://i.postimg.cc/hPK7vQBC/Result.png)](https://postimg.cc/DmYzYZms)

