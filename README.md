# SougouBlocker
这个小插件通过一些技术手段来尝试阻止你的搜狗输入法向外部发送不必要的数据。

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

# 保护程度 
以下是提供的保护
- 阻止一些不必要的程序启动。（评定标准为：不影响输入 = 不必要）
- 阻止使用 `winhttp.dll` 模块建立HTTP连接。（Windows的一个HTTP模块）
- 阻止使用 `wininet.dll` 模块建立HTTP连接。（Windows的一个HTTP模块）
- 禁止 `SGCurlHelper.dll` 的代码执行（似乎是搜狗的curl包装模块），在尝试执行 `SGCurlHelper.dll` 中的代码时将会立即导致程序崩溃，并在禁止代码执行失败时直接退出程序。

注意：以上保护手段未经验证是否有效。（但是它有最低限度的隐私保护）

# 计划清单
- 自定义规则集
- 日志
