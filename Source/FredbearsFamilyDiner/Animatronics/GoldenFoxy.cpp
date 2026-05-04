// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "GoldenFoxy.h"
#include "FredbearsFamilyDiner/Player/FFDPlayerCharacter.h"
#include "FredbearsFamilyDiner/Core/FFDGameState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"

AGoldenFoxy::AGoldenFoxy()
{
    AnimatronicID = FName(TEXT("GoldenFoxy"));
    DisplayName = FText::FromString(TEXT("Golden Foxy"));

    BaseMovementSpeed = 250.0f;
    HuntingSpeedMultiplier = 1.8f;
    AggressiveSpeedMultiplier = 3.0f;
    SightRange = 2000.0f;
    HearingRange = 1800.0f;
    AttackRange = 120.0f;
    BaseAggressionLevel = 10;
    AggroThreshold = 60.0f;
}

void AGoldenFoxy::ActivateAnimatronic(float DifficultyMultiplier, int32 NightNumber)
{
    Super::ActivateAnimatronic(DifficultyMultiplier, NightNumber);

    bIsRushing = false;
    RushTimer = 0.0f;
    RushCooldownTimer = 0.0f;
    ActivationTimer = 0.0f;
    bHasBeenTriggered = false;

    // Start in storage room
    if (StorageRoomPosition)
    {
        SetActorLocation(StorageRoomPosition->GetActorLocation());
    }

    // Faster activation on later nights
    ActivationDelay = FMath::Max(30.0f, ActivationDelay - (NightNumber * 15.0f));

    UE_LOG(LogFFDAI, Log, TEXT("[GoldenFoxy] Activation delay: %.1fs"), ActivationDelay);
}

void AGoldenFoxy::Tick(float DeltaTime)
{
    // Handle rush state independently of the normal state machine
    if (bIsRushing)
    {
        TickRush(DeltaTime);
        return; // Skip normal tick during rush
    }

    // Handle rush cooldown
    if (RushCooldownTimer > 0.0f)
    {
        // Power level affects cooldown — lower power = shorter cooldown
        AFFDGameState* GameState = Cast<AFFDGameState>(
            UGameplayStatics::GetGameState(this));
        float CooldownMult = 1.0f;
        if (GameState && GameState->GetPowerLevel() < PowerThresholdForExtraAggression)
        {
            CooldownMult = 0.5f;
        }

        RushCooldownTimer -= DeltaTime * CooldownMult;
    }

    Super::Tick(DeltaTime);
}

void AGoldenFoxy::InitiateRush()
{
    if (bIsRushing || RushCooldownTimer > 0.0f) return;

    bIsRushing = true;
    RushTimer = 0.0f;

    // Set rush speed
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = RushSpeed * DifficultyMult;
    }

    // Play rush sound
    if (RushSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, RushSound, GetActorLocation());
    }

    // Move toward player
    MoveToPlayer();

    UE_LOG(LogFFDAI, Warning, TEXT("[GoldenFoxy] RUSH INITIATED!"));
}

void AGoldenFoxy::TickIdle(float DeltaTime)
{
    if (!bHasBeenTriggered)
    {
        ActivationTimer += DeltaTime * DifficultyMult;

        if (ActivationTimer >= ActivationDelay)
        {
            bHasBeenTriggered = true;
            SetState(EAnimatronicState::Patrolling);
            UE_LOG(LogFFDAI, Log, TEXT("[GoldenFoxy] Triggered! Leaving storage room."));
        }
        return;
    }

    Super::TickIdle(DeltaTime);
}

void AGoldenFoxy::TickHunting(float DeltaTime)
{
    // Golden Foxy prefers to rush rather than stalk
    if (CanSeePlayer() && RushCooldownTimer <= 0.0f)
    {
        const float RushChance = 0.02f * DifficultyMult;
        if (FMath::FRand() < RushChance)
        {
            InitiateRush();
            return;
        }
    }

    Super::TickHunting(DeltaTime);
}

void AGoldenFoxy::TickAggressive(float DeltaTime)
{
    // Always rush in aggressive mode
    if (!bIsRushing && RushCooldownTimer <= 0.0f)
    {
        InitiateRush();
        return;
    }

    Super::TickAggressive(DeltaTime);
}

void AGoldenFoxy::TickRush(float DeltaTime)
{
    RushTimer += DeltaTime;

    // Keep moving toward player during rush
    MoveToPlayer();

    // Check for attack
    if (CanAttackPlayer())
    {
        AttackPlayer();
        EndRush();
        return;
    }

    // Rush expires
    if (RushTimer >= RushDuration)
    {
        EndRush();

        // Check if a door blocked us
        UE_LOG(LogFFDAI, Log, TEXT("[GoldenFoxy] Rush ended (timed out)"));
    }
}

void AGoldenFoxy::EndRush()
{
    bIsRushing = false;
    RushTimer = 0.0f;
    RushCooldownTimer = RushCooldown;

    // Return to a hiding spot
    const FVector HideSpot = FindHidingSpot();
    if (!HideSpot.IsZero())
    {
        MoveToLocation(HideSpot);
    }

    SetState(EAnimatronicState::Retreating);

    // Reset speed
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = BaseMovementSpeed;
    }
}

FVector AGoldenFoxy::FindHidingSpot() const
{
    if (HidingSpots.Num() == 0)
    {
        return GetRandomPatrolPoint();
    }

    // Pick a random hiding spot that isn't too close to the player
    TArray<AActor*> ValidSpots;
    AActor* Player = GetPlayerTarget();

    for (AActor* Spot : HidingSpots)
    {
        if (!Spot) continue;

        if (Player)
        {
            const float DistToPlayer = FVector::Dist(
                Spot->GetActorLocation(), Player->GetActorLocation());
            if (DistToPlayer > 500.0f)
            {
                ValidSpots.Add(Spot);
            }
        }
        else
        {
            ValidSpots.Add(Spot);
        }
    }

    if (ValidSpots.Num() > 0)
    {
        const int32 Index = FMath::RandRange(0, ValidSpots.Num() - 1);
        return ValidSpots[Index]->GetActorLocation();
    }

    return GetRandomPatrolPoint();
}
