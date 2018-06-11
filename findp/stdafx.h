// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used 
#define STRICT

#include "targetver.h"

#include <Windows.h>
#include <Shlwapi.h>

#include <memory>
#include <regex>
#include <cstdlib>

// TODO: reference additional headers your program requires here

#include "Log.h"
#include "MikeHash.h"
#include "utils.h"

#include "IConcurrentQueue.h"
#include "IOCPQueueImpl.h"
#include "ParallelExec.h"

#include "findp.h"
#include "EnumDir.h"





