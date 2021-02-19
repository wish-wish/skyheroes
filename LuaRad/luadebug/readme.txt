1、如果你是首次使用，请解压luaDebug.zip文件
2、将LuaDebugjit.lua,LuaDebug.lua复制到工程文件的src目录下
3、修改mian.lua引用LuaDebug或者Debugjit,在渲染循环中调用breakSocketHandle()，如下：
local breakSocketHandle,debugXpCall = require("LuaDebugjit")("localhost",7003)
cc.Director:getInstance():getScheduler():scheduleScriptFunc(function() breakSocketHandle(); end, 0.3, false)
4、在function __G__TRACKBACK__(errorMessage)中调用debugXpCall();
function __G__TRACKBACK__(errorMessage)
    print("----------------------------------------")
    print("LUA ERROR: " .. tostring(errorMessage) .. "\n")
    print(debug.traceback("", 2))
    print("----------------------------------------")
    debugXpCall();
end
5、如果你是新工程，可以直接使用压缩包的main.lua，替换您的mian.lua
6、.vscode目录下的配置参考，压缩文件中的launch.json，注意其中的Cocos2-launch和Cocos2-launch-55，前者是LuaRad或luaide 0.3.7中的配置，后者是luaide 0.5.5的配置

2018.1 by cjoy