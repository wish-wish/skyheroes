--[[
write by cjoy 2015-07-10
]]
--------------------------------
-- @module schedulerbs


--[[--

全局计时器、计划任务

«该模块在框架初始化时不会自动载入» 

加载方式: local scheduler = require(cc.PACKAGE_NAME .. ".schedulerbs")

]]
local schedulerbs = {}

local sharedScheduler = cc.Director:getInstance():getScheduler()

-- start --

--------------------------------
-- 计划一个全局帧事件回调，并返回该计划的句柄。
-- @function [parent=#schedulerbs] scheduleUpdateGlobal
-- @param function 回调函数
-- @return mixed#mixed ret (return value: mixed)  schedule句柄

--[[--

计划一个全局帧事件回调，并返回该计划的句柄。

全局帧事件在任何场景中都会执行，因此可以在整个应用程序范围内实现较为精确的全局计时器。

该函数返回的句柄用作 schedulerbs.unscheduleGlobal() 的参数，可以取消指定的计划。 

]]

-- end --

function schedulerbs.scheduleUpdateGlobal(listener,node)
    return sharedScheduler:scheduleScriptFunc(listener, 0, false,node)
end

-- start --

--------------------------------
-- 计划一个以指定时间间隔执行的全局事件回调，并返回该计划的句柄。
-- @function [parent=#schedulerbs] scheduleGlobal
-- @param function listener 回调函数
-- @param number interval 间隔时间
-- @return mixed#mixed ret (return value: mixed)  schedule句柄

--[[--

计划一个以指定时间间隔执行的全局事件回调，并返回该计划的句柄。 

~~~ lua

local function onInterval(dt)
end
 
-- 每 0.5 秒执行一次 onInterval()
local handle = schedulerbs.scheduleGlobal(onInterval, 0.5) 

~~~

]]

-- end --

function schedulerbs.scheduleGlobal(listener, interval,node)
    return sharedScheduler:scheduleScriptFunc(listener, interval, false,node)
end

-- start --

--------------------------------
-- 取消一个全局计划
-- @function [parent=#schedulerbs] unscheduleGlobal
-- @param mixed schedule句柄

--[[--

取消一个全局计划 

schedulerbs.unscheduleGlobal() 的参数就是 schedulerbs.scheduleUpdateGlobal() 和 schedulerbs.scheduleGlobal() 的返回值。

]]

-- end --

function schedulerbs.unscheduleGlobal(handle)
    sharedScheduler:unscheduleScriptEntry(handle)
end

-- start --

--------------------------------
-- 计划一个全局延时回调，并返回该计划的句柄。
-- @function [parent=#schedulerbs] performWithDelayGlobal
-- @param function listener 回调函数
-- @param number time 延迟时间
-- @return mixed#mixed ret (return value: mixed)  schedule句柄

--[[--

计划一个全局延时回调，并返回该计划的句柄。

schedulerbs.performWithDelayGlobal() 会在等待指定时间后执行一次回调函数，然后自动取消该计划。

]]

-- end --

function schedulerbs.performWithDelayGlobal(listener,time,node)
    local handle
    handle = sharedScheduler:scheduleScriptFunc(function(anode,adelta)
        schedulerbs.unscheduleGlobal(handle)
        if listener~=nil then
            listener(anode,adelta)
        end
    end, time, false,node)
    return handle
end

return schedulerbs
