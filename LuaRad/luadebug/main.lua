--1、if you fist time use ,please check you mian.lua file contain the folowing three lines:[5,6,23], or else you can't debug you exe
--2、if you project is the  new project ,you can just replace you mian.lua with this file simply
--2018.1.2 by cjoy:http://www.5icoin.com/LuaDebug.zip

local breakSocketHandle,debugXpCall = require("LuaDebugjit")("localhost",7003)
cc.Director:getInstance():getScheduler():scheduleScriptFunc(function() breakSocketHandle(); end, 0.3, false)

require "config"
require "cocos.init"

local function main()
    package.path = package.path .. ".;src/"
    package.cpath = package.cpath .. "."
    cc.FileUtils:getInstance():setPopupNotify(false)
    require("app.MyApp"):create():run()
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
