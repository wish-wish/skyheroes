/********************************************************************************
* create:cclin
* date:2016-12-26
* ver:
* latest update:
* function:希望创建一个Socket模块，可以用于各种客户端（包括跨平台,IOS,Android,WP,Linux,Mac,WIN*,Tizen），适配各种服务端，实现短链接，长链接，断线重连等
* update log:
* <author>  <time>           <version >   <desc>
*********************************************************************************/
#pragma once

#include "CircleQueue.h"
#include "PriorityCycleQueue.h"
#include "PriorityQueue.h"
#include "MessageQueue.h"
#include "SafetySocket.h"
#include "SafetyHttpSocket.h"
#include "SafetyLongSocket.h"
#include "SafetyShortSocket.h"
#include "SafetyNetClient.h"
#include "SafetyNetGuard.h"
#include "SystemConfig.h"
