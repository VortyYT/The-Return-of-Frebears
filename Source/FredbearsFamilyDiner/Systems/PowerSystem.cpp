// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "PowerSystem.h"
#include "FredbearsFamilyDiner/Core/FFDGameState.h"
#include "Kismet/GameplayStatics.h"

APowerSystem::APowerSystem()
{
    PrimaryActorTick.bCanEverTick = true;
    CurrentPower = MaxPower;
}

void APowerSystem::BeginPlay()
{
    Super::BeginPlay();
    CurrentPower = MaxPower;
    bPowerDepleted = false;
}

void APowerSystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bPowerDepleted)
    {
        UpdatePowerDrain(DeltaTime);
    }
}

void APowerSystem::AddPowerConsumer(FName ConsumerID, float DrainRate)
{
    FPowerConsumer Consumer;
    Consumer.ID = ConsumerID;
    Consumer.DrainRate = DrainRate;
    Consumer.bIsActive = false;

    PowerConsumers.Add(ConsumerID, Consumer);

    UE_LOG(LogFFDPower, Log, TEXT("Added power consumer: %s (drain: %.3f/s)"),
        *ConsumerID.ToString(), DrainRate);
}

void APowerSystem::RemovePowerConsumer(FName ConsumerID)
{
    PowerConsumers.Remove(ConsumerID);
}

void APowerSystem::SetConsumerActive(FName ConsumerID, bool bActive)
{
    if (FPowerConsumer* Consumer = PowerConsumers.Find(ConsumerID))
    {
        Consumer->bIsActive = bActive;

        // Check if usage level changed
        const int32 NewUsageLevel = GetUsageLevel();
        if (NewUsageLevel != LastUsageLevel)
        {
            LastUsageLevel = NewUsageLevel;
            OnPowerUsageChanged.Broadcast(NewUsageLevel);
        }
    }
}

float APowerSystem::GetPowerPercent() const
{
    return (MaxPower > 0.0f) ? (CurrentPower / MaxPower) * 100.0f : 0.0f;
}

float APowerSystem::GetTotalDrainRate() const
{
    float Total = BaseDrainRate;

    for (const auto& Pair : PowerConsumers)
    {
        if (Pair.Value.bIsActive)
        {
            Total += Pair.Value.DrainRate;
        }
    }

    return Total;
}

int32 APowerSystem::GetUsageLevel() const
{
    int32 ActiveCount = 0;
    for (const auto& Pair : PowerConsumers)
    {
        if (Pair.Value.bIsActive)
        {
            ActiveCount++;
        }
    }

    // Usage level 1-5 based on active consumers
    return FMath::Clamp(ActiveCount + 1, 1, 5);
}

void APowerSystem::RestorePower(float Amount)
{
    CurrentPower = FMath::Clamp(CurrentPower + Amount, 0.0f, MaxPower);
    OnPowerChanged.Broadcast(CurrentPower);

    if (CurrentPower > 0.0f)
    {
        bPowerDepleted = false;
    }
}

void APowerSystem::UpdatePowerDrain(float DeltaTime)
{
    const float DrainAmount = GetTotalDrainRate() * DeltaTime;
    const float PrevPower = CurrentPower;

    CurrentPower = FMath::Max(CurrentPower - DrainAmount, 0.0f);

    if (CurrentPower != PrevPower)
    {
        OnPowerChanged.Broadcast(CurrentPower);

        // Update game state
        if (AFFDGameState* GameState = Cast<AFFDGameState>(
            UGameplayStatics::GetGameState(this)))
        {
            GameState->SetPowerLevel(CurrentPower);
        }
    }

    if (CurrentPower <= 0.0f && !bPowerDepleted)
    {
        HandlePowerDepletion();
    }
}

void APowerSystem::HandlePowerDepletion()
{
    bPowerDepleted = true;
    OnPowerDepleted.Broadcast();

    // Deactivate all consumers
    for (auto& Pair : PowerConsumers)
    {
        Pair.Value.bIsActive = false;
    }

    UE_LOG(LogFFDPower, Warning, TEXT("POWER DEPLETED!"));
}
