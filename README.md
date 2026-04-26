# Socket 通信程序

包含两个版本的Socket通信程序：单线程版本和多线程版本。两个版本都采用面向对象设计，使用C++11标准编写。

## 项目结构

```
Socket/
├── SingleThread/          # 单线程版本
│   ├── Client/            # 客户端代码
│   ├── Server/            # 服务器代码
│   ├── Compiled/          # 编译输出
│   └── README.md          # 单线程版本说明
└── Threads/               # 多线程版本
    ├── Client/            # 客户端代码
    ├── Server/            # 服务器代码
    ├── Compiled/          # 编译输出
    └── README.md          # 多线程版本说明
```

## 编译运行

```bash
# 编译
g++ -std=c++11 -Wall Server/*.cpp -o Compiled/server
g++ -std=c++11 -Wall Client/*.cpp -o Compiled/client

# 运行
./Compiled/server      # 终端1
./Compiled/client      # 终端2（多线程版本可启动多个）
```

## 面向对象设计特点

### 1. 封装
- 使用类封装Socket操作，隐藏底层实现细节
- 私有成员变量保护内部状态
- 提供清晰的公共接口

### 2. 继承
- `Socket` 作为基类，提供通用的Socket功能
- `Server` 和 `Client` 继承自 `Socket`，各自实现特定功能
- 代码复用，减少重复代码

### 3. 单一职责原则
- `Socket` 类负责通用的Socket操作
- `Server` 类专注于服务器功能
- `Client` 类专注于客户端功能

## 功能说明

- 服务器默认监听所有网卡的8080端口
- 客户端默认连接到 127.0.0.1:8080
- 通信采用带4字节长度头部的数据格式
- 客户端每隔1秒向服务器发送一条消息
- 服务器将收到的消息回显给客户端

## 注意事项

- 请根据实际网络环境修改客户端连接的服务器IP地址
- 确保防火墙允许8080端口的通信
- 多线程版本编译时需要添加 `-pthread` 选项
- 如需修改端口，请同时修改服务器和客户端的端口设置

## 详细文档

- [单线程版本说明](SingleThread/README.md)
- [多线程版本说明](Threads/README.md)
