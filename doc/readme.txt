1、如果要编译C++工程临时输出路径为“skyheroes\..\_Out”，所以不要解压在磁盘的根目录下
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

.sln文件
solution，即解决方案。它引用了这个解决方案中所包含的项目，一个解决方案一般会有几个项目，即.csproj。里面写有项目的路径，改变路径，打开.sln的时候，就看不到那个项目了。

.suo文件
是solution user option的缩写，它储存了用户界面的自定义配置，包括布局、断点和项目最后编译的而又没有关掉的文件(下次打开时用)等，以便于下一次你打开Visual Studio可以恢复这些设置，因此不要随便删除

2018.1.9 by cjoy


