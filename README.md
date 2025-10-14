# AliyunDDNSCpp

通过调用 Aliyun OpenAPI 实现自动修改指定的dns记录，dns记录值将从配置的url获取。

> 由于使用了 Windows 注册表来存储 accessKey （这是比较安全的存储 secret 的方法），
  本应用目前仅支持 Windows 平台

## 用法

### > 安装

#### 直接安装：

只需下载 release 版本的单一可执行文件，放入任何你想要的目录中即可

#### 自行编译：

依赖项：
- VisualStudio 2022或以上版本
- OpenSSL:x64-windows-static
- cpphttplib
- nlomhann::json
- HYDRA15::Union

1. 克隆代码仓库 ``git clone https://github.com/HYDRA66666/AliyunDDNSCpp.git``
2. 使用 vcpkg 安装依赖项 `` vcpkg install openssl:x64-windows-static cpphttplib nlomhann-json ``
3. 依赖项 HYDRA15::Union 在代码仓库中附赠，由于该依赖项使用了远古版本，请勿随意更新此依赖项
4. 设置项目包含目录、静态库输入文件
5. 编译运行

### > 使用

#### 快速开始

1. 双击软件运行。    
在第一次执行时，按照提示要求输入有权限的账号的 阿里云 AccessKeyID 和 AccessKeySecret，
此数据将存储在注册表 ``HKEY_CURRENT_USER\Software\HYDRA15\AliyunDDNSCpp\`` 路径下。    
AccessKey 设置完成后，退出程序，修改程序创建的文件 ``config.json``: 

- urls: 获取本机公网ip的url，需要网址返回纯文本形式的ip，不能有任何格式修饰符。一般情况下保持不变即可。
  - 中国大陆用户对 ``api.ipify.org`` 的访问不稳定，需要自行寻找可用的源站。对于 ipv4，可以尝试 ``api.ipify.cn``
- domains: 待更新解析记录的域名
  - force_update: 强制更新解析记录。对于现有的 AliDNS API，如果解析记录的值没有变化，接口返回将会报错。

2. 保存文件，再次双击运行程序，观察输出。
3. 在windows计划任务管理器中设置定时任务，任务内容未执行本程序，无需附带任何参数，并配置操作起始路径为程序所在路径。

#### 命令行参数

`` .\AliyunDDNSCpp.exe [-option] [params] ``

- options: 
  - -h 打印帮助信息
  - -c 
    - 无参数：进入交互式配置界面，用于输入 AccessKeyID 和 AccessKeuSecret
    - 后跟随两个参数：直接配置 AccessKeyID 和 AccessKeySecret
  -  -d 删除所有本软件生成的注册表项
  - -s 静默运行




