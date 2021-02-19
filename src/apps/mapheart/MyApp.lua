
local MyApp = class("MyApp", cc.load("mvc").AppBase)

function MyApp:onCreate()
    math.randomseed(os.time())
    --cc.Director:getInstance():setDisplayStats(false);
end

return MyApp
