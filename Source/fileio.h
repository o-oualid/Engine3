#pragma once

#include "baseTypes.h"
struct DebugReadFileResult {
    int64 Size;
    void *Data;
};

DebugReadFileResult DebugReadFile(const char *FileName);
