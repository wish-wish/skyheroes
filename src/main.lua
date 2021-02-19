--1、if you fist time use ,please check you mian.lua file contain the folowing three lines:[5,6,23], or else you can't debug you exe
--2、if you project is the  new project ,you can just replace you mian.lua with this file simply
--2018.1.2 by cjoy:http://www.5icoin.com/luaDebug.zip

local breakSocketHandle,debugXpCall = require("LuaDebugjit")("localhost",7003)
cc.Director:getInstance():getScheduler():scheduleScriptFunc(function() breakSocketHandle(); end, 0.3, false)

package.path = package.path .. "src/;"
--package.cpath = package.cpath .."simulator/debugger/?.dll;C:/Program Files (x86)/Lua/5.1/lua/clibs/?.dll;C:/Program Files (x86)/Lua/5.1/lua/?.dll;C:/Program Files (x86)/Lua/5.1/lua/lib/?.dll;"

print("path:"..package.path);
print("cpath:"..package.cpath);

--local lrdb = require("lrdb_server");
--lrdb.activate(21110);
--require "luasql/mysql"

require "config"
--global check
require "cocos.init"

local function main()
    cc.FileUtils:getInstance():setPopupNotify(false);
    cc.FileUtils:getInstance():addSearchPath("res");
    require("apps.mapheart.MyApp"):create(nil,"mapheart"):run()
end

function __G__TRACKBACK__(errorMessage)
    print("----------------------------------------")
    print("LUA ERROR: " .. tostring(errorMessage) .. "\n")
    print(debug.traceback("", 2))
    print("----------------------------------------")
    debugXpCall();
end

local status, msg = xpcall(main, __G__TRACKBACK__)
if not status then
    print(msg)
end

--lrdb.deactivate();