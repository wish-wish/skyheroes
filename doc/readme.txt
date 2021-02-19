1、如果要编译C++工程临时输出路径为“skyheroes\..\_Out”，所以不要解压在磁盘的根目录或一级目录下，如e:\或者e:\xxx都是不允许的，除了自己修改输出路径
2、如果你不要编译原生C++工程，请随意；
3、sdf=请修改至其他目录：VS菜单：Tool\Options\Text Editor\C/C++\Advance\Fallback Location
Always Use Fallback Location=true
Do Not Warn If Fallback Location=true
Fallback Location="E:\_sdf",为临时路径
4、请使用VS2013已经够用

c:\Program Files (x86)\Microsoft Visual Studio <version>\VC\bin\vcvars32.bat

PS：
VS相关后缀：
http://blog.csdn.net/zhuxiaoyang2000/article/details/6387247
ilk和exp都是用于链接的，pdb用于调试的符号

2018.1.9 by cjoy


