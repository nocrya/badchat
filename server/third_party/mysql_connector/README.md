# 第三方运行时 DLL

此目录里的所有 `*.dll` 会在构建结束后被 CMake 自动拷贝到各 server 的 exe 目录
（`build/bin/<Config>/`），避免每次手动复制。

## 需要放的文件

目前只有：

- `mysqlcppconn-9-vs14.dll`（MySQL Connector/C++ 9.x Release 版运行时 DLL）

来源：下载 MySQL Connector/C++ Release 版（不是 Debug 版），安装后从
`<connector_root>\lib64\vs14\mysqlcppconn-9-vs14.dll`
拷一份过来，**直接丢到当前目录**即可。

> Debug 构建理论上应用 `lib64\vs14\debug\` 里的 DLL。如果项目只跑 Release，
> 放一份 Release DLL 就够了。未来若需要 Debug DLL，复制 `mysqlcppconn-9-vs14d.dll`
> 到这里即可（CMake 会一并拷过去）。

## 添加其它第三方 DLL

如果以后有别的需要跟随 exe 分发的 DLL（比如 OpenSSL 的 `libcrypto-3-x64.dll`、
`libssl-3-x64.dll` 等），**直接往这个目录里扔**就行，CMake 的拷贝逻辑是按
`*.dll` glob 的，不用改构建脚本。

## 为什么不直接依赖系统 PATH

把 DLL 随仓库一起提交的好处：

1. 新同事 clone 后 `build → run` 一条龙，不用各自配系统环境变量。
2. 升级 DLL 版本只需替换此目录里的文件，构建系统会感知变化自动重拷。
3. 发布时也可直接把 `build/bin/Release/` 整包压缩带走。
