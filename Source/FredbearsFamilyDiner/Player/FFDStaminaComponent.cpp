// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "FFDStaminaComponent.h"

UFFDStaminaComponent::UFFDStaminaComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    CurrentStamina = MaxStamina;
}

void UFFDStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    const float PrevStamina = CurrentStamina;

    if (bIsDraining)
    {
        // Drain stamina while sprinting
        CurrentStamina = FMath::Max(CurrentStamina - (DrainPerSecond * DeltaTime), 0.0f);
        RegenDelayTimer = 0.0f;

        if (CurrentStamina <= 0.0f && !bIsExhausted)
        {
            bIsExhausted = true;
            OnStaminaExhausted.Broadcast();
            UE_LOG(LogFFD, Log, TEXT("Player exhausted"));
        }
    }
    else
    {
        // Regenerate stamina when not sprinting (after delay)
        RegenDelayTimer += DeltaTime;

        if (RegenDelayTimer >= RegenDelay)
        {
            CurrentStamina = FMath::Min(
                CurrentStamina + (RegenPerSecond * DeltaTime), MaxStamina);

            // Clear exhaustion once recovered enough
            if (bIsExhausted && GetStaminaPercent() >= ExhaustionRecoveryThreshold)
            {
                bIsExhausted = false;
                OnStaminaRecovered.Broadcast();
                UE_LOG(LogFFD, Log, TEXT("Player recovered from exhaustion"));
            }
        }
    }

    if (CurrentStamina != PrevStamina)
    {
        OnStaminaChanged.Broadcast(CurrentStamina);
    }
}

void UFFDStaminaComponent::StartDraining()
{
    if (!bIsExhausted)
    {
        bIsDraining = true;
    }
}

void UFFDStaminaComponent::StopDraining()
{
    bIsDraining = false;
    RegenDelayTimer = 0.0f;
}

float UFFDStaminaComponent::GetStaminaPercent() const
{
    return (MaxStamina > 0.0f) ? (CurrentStamina / MaxStamina) * 100.0f : 0.0f;
}
