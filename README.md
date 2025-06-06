# SougouBlocker
这个小插件通过一些技术手段来尝试阻止你的搜狗输入法向外部发送不必要的数据。
同时禁用一些垃圾组件的启动。

# 要求
- 至少 C++ 17
- Windows
- Visual Studio 2022

# 使用
1. 从 Release 中下载 `version.dll`
2. 找到你的搜狗输入法的安装目录，例如我的是 `C:\Program Files (x86)\SogouInput\14.2.0.8968`
3. 把 `version.dll` 放到该目录下
4. 重启电脑(或者杀掉所有搜狗的进程) 以让搜狗立即加载本模块
5. 完成

# 保护程度 
以下是提供的保护
- 在不必要的程序启动时自动退出，并阻止一些不必要的程序启动。
- 阻止使用 `socket` 函数建立socket连接。
- 阻止使用 `winhttp.dll` 模块建立HTTP连接。
- 阻止使用 `wininet.dll` 模块建立HTTP连接。
- 禁止 `SGCurlHelper.dll` 模块的代码执行。
- 禁止任何组件启动。

<h6>对于不必要程序的评定标准为：不影响输入 = 不必要</h6>
<h6>禁止代码执行代表当程序尝试执行被禁止执行的代码时，程序将会引起的访问异常，导致程序立即崩溃。</h6>

注意：以上保护手段未经验证是否有效。（但是它有最低限度的保护）

# 计划清单
- 自定义规则集
- 日志
