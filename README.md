# Badchat

基于 C++ / Qt / gRPC 的多服务即时通讯系统，包含 Windows 桌面客户端和一组分布式后端服务。

## 功能

- 用户注册 / 登录 / 找回密码（邮箱验证码）
- 好友申请、好友列表、文本 / 图片消息
- 多 ChatServer 之间通过 gRPC 转发消息
- 文件资源服务（上传 / 下载 / 缩略图）
- Redis 在线状态 + MySQL 持久化

## 目录结构

```
badchat/
├── client/                       Qt 桌面客户端
│   ├── src/                      源码 (.cpp/.h/.ui)
│   ├── res/  static/  style/     图片、样式、静态资源
│   ├── rc.qrc                    Qt 资源清单
│   ├── CMakeLists.txt
│   ├── CMakePresets.json
│   └── config.ini
└── server/
    ├── common/                   公共库 (ConfigMgr / 线程池 / 分布式锁)
    ├── proto/                    message.proto + 生成规则
    ├── services/
    │   ├── gate/                 GateServer (HTTP 接入)
    │   ├── chat/                 ChatServer (IM 主服务)
    │   ├── status/               StatusServer (分配 ChatServer、token)
    │   └── resource/             ResourceServer (文件 / 资源)
    ├── VarifyServer/             Node.js 验证码邮件服务
    ├── cmake/  conan/            构建与依赖配置
    ├── config/                   各服务的 .ini 配置 + .env.example
    ├── db/init/                  MySQL 初始化脚本
    ├── deploy/                   docker-compose.yml
    ├── scripts/                  PowerShell 开发脚本
    ├── CMakeLists.txt
    └── conanfile.py
```

## 技术栈

| 组件         | 说明 |
|--------------|------|
| C++17        | 服务端主语言 |
| Boost.Asio   | 网络与线程池 |
| gRPC / Protobuf | 服务间 RPC |
| MySQL Connector/C++ | 数据库 |
| hiredis      | Redis 客户端 |
| jsoncpp      | JSON |
| Qt6 / Qt5    | 客户端 GUI |
| Conan 2      | C++ 依赖管理 |
| CMake ≥ 3.20 | 构建 |
| Node.js      | VarifyServer（邮箱验证码） |
| Docker       | MySQL / Redis / VarifyServer 容器 |

## 快速开始

### 1. 环境准备

- Python 3.8+、Conan 2：`pip install conan`
- Visual Studio 2019（或兼容 MSVC v142）
- CMake 3.20+
- Qt 6.x（客户端需要）
- Docker Desktop
- MySQL Connector/C++（手动安装，设置 `MYSQL_CONNECTOR_ROOT`）

首次使用：

```powershell
conan profile detect --force
```

### 2. 配置

```powershell
cd server
Copy-Item config\.env.example config\.env
# 编辑 config\.env：填写 MYSQL_PASSWORD / REDIS_PASSWORD / EMAIL_USER / EMAIL_PASS
powershell -ExecutionPolicy Bypass -File .\scripts\sync-config.ps1
```

`sync-config.ps1` 会根据 `config/.env` 重新生成 `config/*.ini` 以及客户端的 `config.ini`，并为原文件创建 `.bak` 备份。

### 3. 启动基础设施 (MySQL / Redis / VarifyServer)

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\dev.ps1 -Action Up
```

等效于 `docker compose up -d --build`。数据库初次启动会自动执行 `db/init/qiqi.sql`。

### 4. 构建 C++ 服务端

```powershell
# 一键：conan install + cmake configure + cmake build
powershell -ExecutionPolicy Bypass -File .\scripts\conan-build-cmake.ps1

# Debug：
powershell -ExecutionPolicy Bypass -File .\scripts\conan-build-cmake.ps1 -Configuration Debug

# 清理并重构：
powershell -ExecutionPolicy Bypass -File .\scripts\conan-build-cmake.ps1 -Clean
```

产物位于 `server/build/bin/<Release|Debug>/`：
`GateServer.exe` / `StatusServer.exe` / `ChatServer.exe` / `ResourceServer.exe`。

### 5. 运行 C++ 服务端

```powershell
# 全部启动
powershell -ExecutionPolicy Bypass -File .\scripts\run-all-cpp.ps1

# 或单独启动
powershell -ExecutionPolicy Bypass -File .\scripts\run-cpp-server.ps1 -Server Status
powershell -ExecutionPolicy Bypass -File .\scripts\run-cpp-server.ps1 -Server Chat1
powershell -ExecutionPolicy Bypass -File .\scripts\run-cpp-server.ps1 -Server Chat2
powershell -ExecutionPolicy Bypass -File .\scripts\run-cpp-server.ps1 -Server Gate
powershell -ExecutionPolicy Bypass -File .\scripts\run-cpp-server.ps1 -Server Resource
```

每个进程会以对应 `config/*.ini` 启动。加 `-Foreground` 让它在当前终端前台运行。

### 6. 自检

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\dev.ps1 -Action Check
```

检查内容：
- `.env` 必填项是否齐全
- 容器运行状态 (mysql / redis / varifyserver)
- 各端口监听
- `config/*.ini` 与 `.env` 是否一致

返回码非 0 表示存在 `ERROR`。

### 7. 停止基础设施

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\dev.ps1 -Action Down
```

### 8. 构建客户端

```powershell
cd client
# 需预先设置环境变量 QT_DIR 指向 Qt 安装目录
cmake --preset qt-debug
cmake --build build\qt-debug
```

可执行文件会被拷贝到 `client/bin/`，并自动复制 `config.ini` 和 `static/` 目录。

## 脚本一览

| 脚本 | 作用 |
|------|------|
| `scripts\dev.ps1 -Action Up\|Down\|Check` | docker 基础设施控制 + 启动自检 |
| `scripts\sync-config.ps1` | 从 `.env` 生成所有 `config/*.ini` |
| `scripts\conan-build-cmake.ps1` | Conan 装依赖 + CMake 配置 + 编译 |
| `scripts\conan-clean.ps1` | 清理 `conan_output`（加 `-All` 清全局缓存）|
| `scripts\run-cpp-server.ps1 -Server X` | 启动单个 C++ 服务 |
| `scripts\run-all-cpp.ps1` | 按顺序启动全部 C++ 服务 |

## 端口约定

| 服务 | 默认端口 | 说明 |
|------|----------|------|
| GateServer | 8080 | HTTP 入口 |
| VarifyServer | 50051 | gRPC（邮箱验证码） |
| StatusServer | 50052 | gRPC（分配 Chat 节点 / token）|
| ChatServer1 | 8090 / 50055 | TCP / gRPC |
| ChatServer2 | 8091 / 50056 | TCP / gRPC |
| ResourceServer | 9090 / 51055 | TCP / gRPC |
| MySQL | 3308 | 宿主机映射 |
| Redis | 6379 | 宿主机映射 |

以上端口全部由 `config/.env` 控制，可随意修改。

## 故障排查

- **Conan 找不到**：`pip install conan`，然后 `conan profile detect --force`。
- **下载慢**：`conan remote add conancenter https://center.conan.io --index 0`。
- **编译找不到头文件**：重跑 `conan-build-cmake.ps1 -Clean`。
- **C++ 服务无法连接 MySQL / Redis**：`dev.ps1 -Action Check` 看端口与容器状态；确认 `MYSQL_CONNECTOR_ROOT` 指向本机 MySQL Connector/C++ 安装路径。
- **VarifyServer 发邮件失败**：确认 `.env` 里 `EMAIL_USER` / `EMAIL_PASS` 是邮箱的应用专用密码，不是登录密码。

## 许可证

本项目仅供学习使用。
