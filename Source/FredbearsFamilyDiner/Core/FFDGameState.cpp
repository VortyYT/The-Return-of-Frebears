// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "FFDGameState.h"

AFFDGameState::AFFDGameState()
{
}

void AFFDGameState::SetPowerLevel(float NewPower)
{
    CurrentPower = FMath::Clamp(NewPower, 0.0f, FFDConstants::MaxPower);
}

void AFFDGameState::SetThreatLevel(float NewThreat)
{
    CurrentThreatLevel = FMath::Clamp(NewThreat, 0.0f, 10.0f);
}

void AFFDGameState::CollectLoreItem(FName ItemID)
{
    if (!ItemID.IsNone() && !CollectedLoreItems.Contains(ItemID))
    {
        CollectedLoreItems.Add(ItemID);
        UE_LOG(LogFFD, Log, TEXT("Collected lore item: %s (Total: %d)"),
            *ItemID.ToString(), CollectedLoreItems.Num());
    }
}

bool AFFDGameState::HasCollectedLoreItem(FName ItemID) const
{
    return CollectedLoreItems.Contains(ItemID);
}
