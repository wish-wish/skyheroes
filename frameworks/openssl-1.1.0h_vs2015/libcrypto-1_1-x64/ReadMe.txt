========================================================================
    静态库：libcrypto 项目概述
========================================================================

生成libcrypto-1_1-x64.lib

1.右键项目--属性--c++ -- 预处理--预处理器定义  就是 CFLAGS -D
OPENSSL_USE_APPLINK
DSO_WIN32
OPENSSL_THREADS
OPENSSL_NO_STATIC_ENGINE
OPENSSLDIR="C:\\Program Files\\Common Files\\SSL\\"
OPENSSL_SYS_WIN32
WIN32_LEAN_AND_MEAN
L_ENDIAN
_CRT_SECURE_NO_DEPRECATE
UNICODE
ENGINESDIR="C:\\Program Files\\OpenSSL\\lib\\engines-1_1\\"

2.右键项目--属性--库管理--常规--附加依赖项     每行一个
ws2_32.lib gdi32.lib advapi32.lib crypt32.lib user32.lib $(MSBuildProjectDirectory)\uplink-x86_64.obj
uplink-x86_64.obj  需要先手动编译openssl后整合到vs，这是使用strawberry perl编译openssl后生成uplink-x86_64.asm再由nasm汇编产生

3.右键项目--属性--链接器-->输入-->模块定义文件  不然没有.lib生成
$(MSBuildProjectDirectory)\libcrypto-1_1-x64.def

4. 右键项目--属性--vc++目录--包含目录
$(MSBuildProjectDirectory)\include
$(MSBuildProjectDirectory)


