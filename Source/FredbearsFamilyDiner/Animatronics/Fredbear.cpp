// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "Fredbear.h"
#include "FredbearsFamilyDiner/Player/FFDPlayerCharacter.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"

AFredbear::AFredbear()
{
    AnimatronicID = FName(TEXT("Fredbear"));
    DisplayName = FText::FromString(TEXT("Fredbear"));

    BaseMovementSpeed = 180.0f;
    HuntingSpeedMultiplier = 1.4f;
    AggressiveSpeedMultiplier = 2.2f;
    SightRange = 1800.0f;
    AttackRange = 130.0f;
    BaseAggressionLevel = 8;
    AggroThreshold = 40.0f;
}

void AFredbear::ActivateAnimatronic(float DifficultyMultiplier, int32 NightNumber)
{
    Super::ActivateAnimatronic(DifficultyMultiplier, NightNumber);

    bIsOnStage = true;
    StageTimer = 0.0f;

    // Earlier nights = longer stage delay
    StageLeaveDelay = FMath::Max(20.0f, StageLeaveDelay - (NightNumber * 8.0f));

    if (StagePosition)
    {
        SetActorLocation(StagePosition->GetActorLocation());
        SetActorRotation(StagePosition->GetActorRotation());
    }

    UE_LOG(LogFFDAI, Log, TEXT("[Fredbear] Stage leave delay: %.1fs"), StageLeaveDelay);
}

void AFredbear::OnCameraObserved()
{
    if (bIsOnStage)
    {
        // Looking at Fredbear on camera resets his stage timer partially
        StageTimer = FMath::Max(StageTimer - CameraObservationDelay, 0.0f);
        UE_LOG(LogFFDAI, Log, TEXT("[Fredbear] Camera observed. Stage timer reset to %.1f"),
            StageTimer);
    }
    else
    {
        // Seeing him elsewhere might cause him to freeze briefly
        CameraObservationTimer = CameraObservationDelay * 0.5f;
    }
}

void AFredbear::TickIdle(float DeltaTime)
{
    if (bIsOnStage)
    {
        StageTimer += DeltaTime * DifficultyMult;

        if (StageTimer >= StageLeaveDelay)
        {
            bIsOnStage = false;
            SetState(EAnimatronicState::Patrolling);
            UE_LOG(LogFFDAI, Log, TEXT("[Fredbear] Left the stage!"));
        }
        return;
    }

    // Normal idle behavior when off stage
    Super::TickIdle(DeltaTime);
}

void AFredbear::TickHunting(float DeltaTime)
{
    // Camera observation freezes Fredbear briefly
    if (CameraObservationTimer > 0.0f)
    {
        CameraObservationTimer -= DeltaTime;
        if (AAIController* AIC = Cast<AAIController>(GetController()))
        {
            AIC->StopMovement();
        }
        return;
    }

    Super::TickHunting(DeltaTime);
}

void AFredbear::TickStalking(float DeltaTime)
{
    // Fredbear prefers to find ambush positions
    if (FMath::FRand() < 0.02f * DifficultyMult)
    {
        TryAmbushPosition();
    }

    Super::TickStalking(DeltaTime);
}

void AFredbear::TickAggressive(float DeltaTime)
{
    // Fredbear's aggressive mode is relentless
    PlayMovementSound();
    Super::TickAggressive(DeltaTime);
}

void AFredbear::TryAmbushPosition()
{
    if (AmbushPositions.Num() == 0) return;

    AActor* Player = GetPlayerTarget();
    if (!Player) return;

    // Find the ambush point closest to the player
    AActor* BestAmbush = nullptr;
    float BestScore = -1.0f;

    for (AActor* AmbushPoint : AmbushPositions)
    {
        if (!AmbushPoint) continue;

        const float DistToPlayer = FVector::Dist(
            AmbushPoint->GetActorLocation(), Player->GetActorLocation());

        // Prefer positions that are close to the player but not too close
        if (DistToPlayer > AmbushRange * 0.3f && DistToPlayer < AmbushRange)
        {
            const float Score = 1.0f - (DistToPlayer / AmbushRange);
            if (Score > BestScore)
            {
                BestScore = Score;
                BestAmbush = AmbushPoint;
            }
        }
    }

    if (BestAmbush)
    {
        MoveToLocation(BestAmbush->GetActorLocation());
        UE_LOG(LogFFDAI, Verbose, TEXT("[Fredbear] Moving to ambush position"));
    }
}
