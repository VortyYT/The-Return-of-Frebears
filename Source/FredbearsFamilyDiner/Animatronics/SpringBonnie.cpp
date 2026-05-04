// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "SpringBonnie.h"
#include "FredbearsFamilyDiner/Player/FFDPlayerCharacter.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"

ASpringBonnie::ASpringBonnie()
{
    AnimatronicID = FName(TEXT("SpringBonnie"));
    DisplayName = FText::FromString(TEXT("Spring Bonnie"));

    BaseMovementSpeed = 220.0f;
    HuntingSpeedMultiplier = 1.6f;
    AggressiveSpeedMultiplier = 2.0f;
    SightRange = 1400.0f;
    HearingRange = 2500.0f; // Extra-sensitive hearing
    AttackRange = 140.0f;
    BaseAggressionLevel = 6;
    AggroThreshold = 35.0f;
}

void ASpringBonnie::ActivateAnimatronic(float DifficultyMultiplier, int32 NightNumber)
{
    Super::ActivateAnimatronic(DifficultyMultiplier, NightNumber);
    bIsInVent = false;
    VentTimer = 0.0f;
    bIsRetreating = false;
}

void ASpringBonnie::EnterVent()
{
    bIsInVent = true;
    VentTimer = 0.0f;

    // Hide the animatronic while in vents
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);

    UE_LOG(LogFFDAI, Log, TEXT("[SpringBonnie] Entered vent system"));
}

void ASpringBonnie::ExitVent()
{
    bIsInVent = false;

    // Find the vent exit closest to the player
    AActor* Player = GetPlayerTarget();
    if (Player && VentExits.Num() > 0)
    {
        AActor* BestExit = VentExits[0];
        float BestDist = MAX_FLT;

        for (AActor* Exit : VentExits)
        {
            if (!Exit) continue;
            const float Dist = FVector::Dist(
                Exit->GetActorLocation(), Player->GetActorLocation());
            if (Dist < BestDist)
            {
                BestDist = Dist;
                BestExit = Exit;
            }
        }

        if (BestExit)
        {
            SetActorLocation(BestExit->GetActorLocation());
        }
    }

    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);

    UE_LOG(LogFFDAI, Log, TEXT("[SpringBonnie] Exited vent system"));
}

void ASpringBonnie::TickPatrolling(float DeltaTime)
{
    if (bIsInVent)
    {
        VentTimer += DeltaTime;
        if (VentTimer >= VentTravelTime)
        {
            ExitVent();
            SetState(EAnimatronicState::Hunting);
        }
        return;
    }

    // Spring Bonnie has a chance to use vents
    if (VentEntrances.Num() > 0 && FMath::FRand() < 0.005f * DifficultyMult)
    {
        // Find nearest vent entrance
        AActor* NearestVent = nullptr;
        float NearestDist = MAX_FLT;

        for (AActor* Vent : VentEntrances)
        {
            if (!Vent) continue;
            const float Dist = FVector::Dist(GetActorLocation(), Vent->GetActorLocation());
            if (Dist < NearestDist)
            {
                NearestDist = Dist;
                NearestVent = Vent;
            }
        }

        if (NearestVent && NearestDist < 300.0f)
        {
            EnterVent();
            return;
        }
        else if (NearestVent)
        {
            MoveToLocation(NearestVent->GetActorLocation());
            return;
        }
    }

    // React strongly to player noise
    if (CanHearPlayer())
    {
        AFFDPlayerCharacter* PlayerChar = Cast<AFFDPlayerCharacter>(GetPlayerTarget());
        if (PlayerChar && PlayerChar->GetNoiseLevel() > 0.4f)
        {
            CurrentAggro += PlayerChar->GetNoiseLevel() * NoiseReactionMultiplier *
                DifficultyMult * DeltaTime * 30.0f;
        }
    }

    Super::TickPatrolling(DeltaTime);
}

void ASpringBonnie::TickHunting(float DeltaTime)
{
    if (bIsInVent)
    {
        VentTimer += DeltaTime;
        if (VentTimer >= VentTravelTime)
        {
            ExitVent();
        }
        return;
    }

    // Hit-and-run: if player spots us while hunting, chance to retreat
    if (CanSeePlayer())
    {
        AFFDPlayerCharacter* PlayerChar = Cast<AFFDPlayerCharacter>(GetPlayerTarget());
        if (PlayerChar)
        {
            // Check if player is looking at us
            const FVector ToAnimatronic = (GetActorLocation() -
                PlayerChar->GetActorLocation()).GetSafeNormal();
            const FVector PlayerForward = PlayerChar->GetActorForwardVector();
            const float Dot = FVector::DotProduct(PlayerForward, ToAnimatronic);

            if (Dot > 0.7f && FMath::FRand() < RetreatChance)
            {
                SetState(EAnimatronicState::Retreating);
                return;
            }
        }
    }

    Super::TickHunting(DeltaTime);
}

void ASpringBonnie::TickStalking(float DeltaTime)
{
    // Try to flank the player
    const FVector FlankPos = GetFlankingPosition();
    if (!FlankPos.IsZero())
    {
        MoveToLocation(FlankPos);
    }

    // Build aggro faster through noise
    if (CanHearPlayer())
    {
        CurrentAggro += NoiseReactionMultiplier * DifficultyMult * DeltaTime * 10.0f;
    }

    // Use base stalking for state transitions
    Super::TickStalking(DeltaTime);
}

FVector ASpringBonnie::GetFlankingPosition() const
{
    AActor* Player = GetPlayerTarget();
    if (!Player) return FVector::ZeroVector;

    // Get a position behind or to the side of the player
    const FVector PlayerLoc = Player->GetActorLocation();
    const FVector PlayerForward = Player->GetActorForwardVector();

    // Try behind the player first
    const FVector BehindPlayer = PlayerLoc - PlayerForward * 400.0f;

    UNavigationSystemV1* NavSys =
        FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (NavSys)
    {
        FNavLocation NavLoc;
        if (NavSys->ProjectPointToNavigation(BehindPlayer, NavLoc))
        {
            return NavLoc.Location;
        }

        // Try to the side
        const FVector RightOfPlayer =
            PlayerLoc + FVector::CrossProduct(PlayerForward, FVector::UpVector) * 400.0f;
        if (NavSys->ProjectPointToNavigation(RightOfPlayer, NavLoc))
        {
            return NavLoc.Location;
        }
    }

    return FVector::ZeroVector;
}
