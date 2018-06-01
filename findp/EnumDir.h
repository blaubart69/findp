#pragma once

#include "stdafx.h"

typedef void(*EnumDirHandler)(WIN32_FIND_DATA *finddata);

void EnumDir(std::wstring *fulldirname, std::function<void(WIN32_FIND_DATA*)> OnDirEntry);
void EnumDir(LPCWSTR fulldirnameWithBackSlashStar, const EnumDirHandler OnDirEntry);