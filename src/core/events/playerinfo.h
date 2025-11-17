#pragma once
#include "eventpp/callbacklist.h"

class PlayerInfo;
extern eventpp::CallbackList<void(PlayerInfo*)> g_EventPlayerInfoChanged;