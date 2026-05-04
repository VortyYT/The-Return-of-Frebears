// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "FFDSaveGame.h"

UFFDSaveGame::UFFDSaveGame()
{
    SaveTimestamp = FDateTime::Now();
}

void UFFDSaveGame::PreSave()
{
    SaveTimestamp = FDateTime::Now();
}
