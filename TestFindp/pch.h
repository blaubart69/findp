//
// pch.h
// Header for standard system include files.
//

#pragma once

#include "gtest/gtest.h"

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used 
#define STRICT
#include <Windows.h>

//#include "..\findp\beeLib.h"
#include "..\findp\utils.h"
#include "..\findp\StringBuilder.h"
#include "..\findp\LineWriter.h"
#include "..\findp\Log.h"
#include "..\findp\MikeHash.h"
#include "..\findp\IConcurrentQueue.h"
#include "..\findp\IOCPQueueImpl.h"
#include "..\findp\ParallelExec.h"
#include "..\findp\findp.h"
#include "..\findp\EnumDir.h"