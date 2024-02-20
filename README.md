# SougouBlocker
这个小插件通过劫持dll和hook两个api来尝试阻止你的搜狗输入法向外部发送不必要的数据。

# 要求
- 至少 C++ 17
- Windows
- Visual Studio 2022

# 使用
1. 从 Release 中下载 version.dll
2. 找到你的搜狗输入法的安装目录，我的是`C:\Program Files (x86)\SogouInput\14.2.0.8968`
3. 把 `version.dll` 放到该目录下
4. 重启电脑(建议，非必须)
5. 完成

# 说明
这里仅仅通过几个简单的规则集来阻止一些不必要的程序启动加上简单的两个hook来阻止http请求，而且未经验证是否有效。（但是它有最低限度的隐私保护）

# 计划清单
- 自定义规则集
- 日志
