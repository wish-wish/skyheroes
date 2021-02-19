========================================================================
    控制台应用程序：openssl 项目概述
========================================================================

setargv.obj 微软库，支持命令行的?*文件或路径   Expanding Wildcard Arguments     https://msdn.microsoft.com/en-us/library/8bch7bkk.aspx
When you run a C program, you can use either of the two wildcards — the question mark (?) and the asterisk (*) — to specify filename and path arguments on the command line.

1. 右键项目--属性--vc++目录--包含目录
$(MSBuildProjectDirectory)\include
$(MSBuildProjectDirectory)

2.右键项目--属性--库管理--常规--附加依赖项   每行一个
ws2_32.lib gdi32.lib advapi32.lib crypt32.lib user32.lib $(MSBuildProjectDirectory)\libcrypto-1_1-x64.lib $(MSBuildProjectDirectory)\libssl-1_1-x64.lib  setargv.obj


openssl.exe使用
openssl dgst -md5 text.txt  生成text.txt的md5
openssl.exe list --digest-commands   支持的摘要算法列表
openssl.exe list -digest-algorithms

des算法加密文件
openssl enc -des -in text.txt -out text.desenc   加密，需要输入两次密码
openssl enc -des -d -in text.desenc -out text.desdeenc  解密
openssl.exe list -cipher-commands   加密命令列表
openssl.exe list -cipher-algorithms

openssl.exe list -commands  列出可以使用的命令 有enc dgst等等