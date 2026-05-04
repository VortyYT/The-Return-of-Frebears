// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "AnimatronicBase.h"
#include "FredbearsFamilyDiner/Player/FFDPlayerCharacter.h"
#include "FredbearsFamilyDiner/Core/FFDGameMode.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"

AAnimatronicBase::AAnimatronicBase()
{
    PrimaryActorTick.bCanEverTick = true;

    // Audio components
    MovementAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MovementAudio"));
    MovementAudioComponent->SetupAttachment(GetRootComponent());
    MovementAudioComponent->bAutoActivate = false;

    AmbientAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AmbientAudio"));
    AmbientAudioComponent->SetupAttachment(GetRootComponent());
    AmbientAudioComponent->bAutoActivate = false;

    // AI controller
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    // Movement defaults
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = BaseMovementSpeed;
        MoveComp->bOrientRotationToMovement = true;
        MoveComp->RotationRate = FRotator(0.0f, 180.0f, 0.0f);
    }
}

void AAnimatronicBase::BeginPlay()
{
    Super::BeginPlay();

    // Register with game mode
    if (AFFDGameMode* GameMode = Cast<AFFDGameMode>(
        UGameplayStatics::GetGameMode(this)))
    {
        GameMode->RegisterAnimatronic(this);
    }
}

void AAnimatronicBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsActive) return;

    // Update player tracking
    if (CanSeePlayer())
    {
        AActor* Player = GetPlayerTarget();
        if (Player)
        {
            LastKnownPlayerLocation = Player->GetActorLocation();
            TimeSinceLastSawPlayer = 0.0f;

            // Build aggro based on proximity and visibility
            const float Distance = GetDistanceToPlayer();
            const float ProximityFactor = 1.0f - FMath::Clamp(
                Distance / SightRange, 0.0f, 1.0f);
            CurrentAggro += ProximityFactor * DifficultyMult * 30.0f * DeltaTime;
        }
    }
    else
    {
        TimeSinceLastSawPlayer += DeltaTime;
        CurrentAggro -= AggroDecayRate * DeltaTime;
    }

    // Check if player is making noise (hearing)
    if (CanHearPlayer())
    {
        AActor* Player = GetPlayerTarget();
        if (Player)
        {
            AFFDPlayerCharacter* PlayerChar = Cast<AFFDPlayerCharacter>(Player);
            if (PlayerChar)
            {
                const float NoiseContribution =
                    PlayerChar->GetNoiseLevel() * 20.0f * DifficultyMult * DeltaTime;
                CurrentAggro += NoiseContribution;

                if (PlayerChar->GetNoiseLevel() > 0.5f)
                {
                    LastKnownPlayerLocation = Player->GetActorLocation();
                }
            }
        }
    }

    CurrentAggro = FMath::Clamp(CurrentAggro, 0.0f, 100.0f);

    // State-based tick
    switch (CurrentState)
    {
    case EAnimatronicState::Idle:        TickIdle(DeltaTime);        break;
    case EAnimatronicState::Patrolling:  TickPatrolling(DeltaTime);  break;
    case EAnimatronicState::Hunting:     TickHunting(DeltaTime);     break;
    case EAnimatronicState::Stalking:    TickStalking(DeltaTime);    break;
    case EAnimatronicState::Aggressive:  TickAggressive(DeltaTime);  break;
    case EAnimatronicState::Retreating:  TickRetreating(DeltaTime);  break;
    default: break;
    }
}

void AAnimatronicBase::ActivateAnimatronic(float DifficultyMultiplier, int32 NightNumber)
{
    bIsActive = true;
    DifficultyMult = DifficultyMultiplier;
    CurrentAggro = 0.0f;
    PathfindingFailCount = 0;

    // Scale aggression with night number
    const float NightScaling = 1.0f + (NightNumber - 1) * 0.15f;
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = BaseMovementSpeed * FMath::Min(NightScaling, 2.0f);
    }

    SetState(EAnimatronicState::Idle);

    UE_LOG(LogFFDAI, Log, TEXT("[%s] Activated. Difficulty: %.2f, Night: %d"),
        *AnimatronicID.ToString(), DifficultyMultiplier, NightNumber);
}

void AAnimatronicBase::DeactivateAnimatronic()
{
    bIsActive = false;
    SetState(EAnimatronicState::Deactivated);

    if (AAIController* AIC = Cast<AAIController>(GetController()))
    {
        AIC->StopMovement();
    }

    UE_LOG(LogFFDAI, Log, TEXT("[%s] Deactivated"), *AnimatronicID.ToString());
}

void AAnimatronicBase::SetState(EAnimatronicState NewState)
{
    if (CurrentState == NewState) return;

    const EAnimatronicState OldState = CurrentState;
    CurrentState = NewState;

    // Adjust movement speed based on state
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        switch (NewState)
        {
        case EAnimatronicState::Hunting:
            MoveComp->MaxWalkSpeed = BaseMovementSpeed * HuntingSpeedMultiplier * DifficultyMult;
            break;
        case EAnimatronicState::Aggressive:
            MoveComp->MaxWalkSpeed = BaseMovementSpeed * AggressiveSpeedMultiplier * DifficultyMult;
            break;
        case EAnimatronicState::Stalking:
            MoveComp->MaxWalkSpeed = BaseMovementSpeed * 0.5f;
            break;
        default:
            MoveComp->MaxWalkSpeed = BaseMovementSpeed;
            break;
        }
    }

    OnStateChanged.Broadcast(OldState, NewState);

    UE_LOG(LogFFDAI, Log, TEXT("[%s] State: %d -> %d"), *AnimatronicID.ToString(),
        static_cast<int32>(OldState), static_cast<int32>(NewState));
}

void AAnimatronicBase::AttackPlayer()
{
    AActor* Player = GetPlayerTarget();
    if (!Player) return;

    AFFDPlayerCharacter* PlayerChar = Cast<AFFDPlayerCharacter>(Player);
    if (!PlayerChar || !PlayerChar->IsAlive()) return;

    // Play jumpscare sound
    if (JumpscareSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, JumpscareSound, GetActorLocation());
    }

    OnAttack.Broadcast(Player);
    PlayerChar->KillPlayer();

    UE_LOG(LogFFDAI, Warning, TEXT("[%s] ATTACKED PLAYER!"), *AnimatronicID.ToString());
}

bool AAnimatronicBase::CanAttackPlayer() const
{
    if (!bIsActive) return false;

    const float Distance = GetDistanceToPlayer();
    return Distance <= AttackRange && Distance >= 0.0f;
}

bool AAnimatronicBase::CanSeePlayer() const
{
    if (!bIsActive) return false;

    AActor* Player = GetPlayerTarget();
    if (!Player) return false;

    const float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
    if (Distance > SightRange) return false;

    // Check angle
    const FVector ToPlayer = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    const float DotProduct = FVector::DotProduct(GetActorForwardVector(), ToPlayer);
    const float Angle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));

    if (Angle > SightAngle * 0.5f) return false;

    return PerformLineOfSightCheck();
}

bool AAnimatronicBase::CanHearPlayer() const
{
    if (!bIsActive) return false;

    AActor* Player = GetPlayerTarget();
    if (!Player) return false;

    AFFDPlayerCharacter* PlayerChar = Cast<AFFDPlayerCharacter>(Player);
    if (!PlayerChar) return false;

    const float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
    const float EffectiveRange = HearingRange * PlayerChar->GetNoiseLevel();

    return Distance <= EffectiveRange;
}

float AAnimatronicBase::GetDistanceToPlayer() const
{
    AActor* Player = GetPlayerTarget();
    if (!Player) return MAX_FLT;

    return FVector::Dist(GetActorLocation(), Player->GetActorLocation());
}

AActor* AAnimatronicBase::GetPlayerTarget() const
{
    return UGameplayStatics::GetPlayerCharacter(this, 0);
}

void AAnimatronicBase::PlayMovementSound()
{
    if (MovementSound && MovementAudioComponent && !MovementAudioComponent->IsPlaying())
    {
        MovementAudioComponent->SetSound(MovementSound);
        MovementAudioComponent->Play();
    }
}

void AAnimatronicBase::PlayAmbientSound()
{
    if (AmbientSound && AmbientAudioComponent && !AmbientAudioComponent->IsPlaying())
    {
        AmbientAudioComponent->SetSound(AmbientSound);
        AmbientAudioComponent->Play();
    }
}

// --- Default State Behaviors ---

void AAnimatronicBase::TickIdle(float DeltaTime)
{
    PatrolWaitTimer += DeltaTime;

    // Random chance to start patrolling
    const float WaitTime = FMath::RandRange(PatrolWaitTimeMin, PatrolWaitTimeMax);
    if (PatrolWaitTimer >= WaitTime)
    {
        PatrolWaitTimer = 0.0f;
        SetState(EAnimatronicState::Patrolling);
    }

    // React to high aggro
    if (CurrentAggro >= AggroThreshold)
    {
        SetState(EAnimatronicState::Hunting);
    }
    else if (CanSeePlayer() && CurrentAggro >= AggroThreshold * 0.5f)
    {
        SetState(EAnimatronicState::Stalking);
    }
}

void AAnimatronicBase::TickPatrolling(float DeltaTime)
{
    if (bIsWaitingAtPatrolPoint)
    {
        PatrolWaitTimer += DeltaTime;
        const float WaitTime = FMath::RandRange(PatrolWaitTimeMin, PatrolWaitTimeMax);

        if (PatrolWaitTimer >= WaitTime)
        {
            bIsWaitingAtPatrolPoint = false;
            PatrolWaitTimer = 0.0f;
        }

        // Randomly play ambient sounds while waiting
        if (FMath::FRand() < 0.01f)
        {
            PlayAmbientSound();
        }
    }
    else
    {
        const FVector PatrolTarget = GetRandomPatrolPoint();
        if (!MoveToLocation(PatrolTarget))
        {
            HandlePathfindingFailure();
        }
        else
        {
            PathfindingFailCount = 0;

            // Check if we reached the patrol point
            if (FVector::Dist(GetActorLocation(), PatrolTarget) < 100.0f)
            {
                bIsWaitingAtPatrolPoint = true;
                PlayMovementSound();
            }
        }
    }

    // Transition to hunting if aggro is high
    if (CurrentAggro >= AggroThreshold)
    {
        SetState(EAnimatronicState::Hunting);
    }
    else if (CanSeePlayer() && CurrentAggro >= AggroThreshold * 0.3f)
    {
        SetState(EAnimatronicState::Stalking);
    }
}

void AAnimatronicBase::TickHunting(float DeltaTime)
{
    PlayMovementSound();

    if (CanSeePlayer())
    {
        if (!MoveToPlayer())
        {
            HandlePathfindingFailure();
        }
        else
        {
            PathfindingFailCount = 0;
        }
    }
    else
    {
        // Move toward last known location
        if (!MoveToLocation(LastKnownPlayerLocation))
        {
            HandlePathfindingFailure();
        }

        // Lost the player for too long
        if (TimeSinceLastSawPlayer > FFDConstants::AIAggroTimeout)
        {
            SetState(EAnimatronicState::Patrolling);
            CurrentAggro *= 0.5f;
        }
    }

    // Close enough to attack — go aggressive
    if (CanAttackPlayer())
    {
        SetState(EAnimatronicState::Aggressive);
    }

    // Aggro decayed — return to patrol
    if (CurrentAggro < AggroThreshold * 0.3f)
    {
        SetState(EAnimatronicState::Patrolling);
    }
}

void AAnimatronicBase::TickStalking(float DeltaTime)
{
    // Slowly approach the player while maintaining some distance
    if (CanSeePlayer())
    {
        const float Distance = GetDistanceToPlayer();

        if (Distance > AttackRange * 3.0f)
        {
            MoveToPlayer();
        }
        else
        {
            // Stop and stalk
            if (AAIController* AIC = Cast<AAIController>(GetController()))
            {
                AIC->StopMovement();
            }
        }

        // Build aggro while stalking
        CurrentAggro += 5.0f * DifficultyMult * DeltaTime;
    }
    else
    {
        // Lost sight — transition based on aggro
        if (CurrentAggro >= AggroThreshold)
        {
            SetState(EAnimatronicState::Hunting);
        }
        else if (TimeSinceLastSawPlayer > FFDConstants::AIAggroTimeout * 0.5f)
        {
            SetState(EAnimatronicState::Patrolling);
        }
    }

    // High aggro — go hunting
    if (CurrentAggro >= AggroThreshold * 1.5f)
    {
        SetState(EAnimatronicState::Hunting);
    }

    // Random ambient sound for creepiness
    if (FMath::FRand() < 0.005f)
    {
        PlayAmbientSound();
    }
}

void AAnimatronicBase::TickAggressive(float DeltaTime)
{
    if (CanAttackPlayer())
    {
        AttackPlayer();
    }
    else
    {
        // Chase aggressively
        if (!MoveToPlayer())
        {
            HandlePathfindingFailure();
        }

        // If player escaped, go back to hunting
        const float Distance = GetDistanceToPlayer();
        if (Distance > AttackRange * 5.0f)
        {
            SetState(EAnimatronicState::Hunting);
        }
    }
}

void AAnimatronicBase::TickRetreating(float DeltaTime)
{
    // Move back to a patrol point or starting position
    const FVector RetreatTarget = GetRandomPatrolPoint();
    MoveToLocation(RetreatTarget);

    if (FVector::Dist(GetActorLocation(), RetreatTarget) < 150.0f)
    {
        CurrentAggro = 0.0f;
        SetState(EAnimatronicState::Idle);
    }
}

void AAnimatronicBase::HandlePathfindingFailure()
{
    PathfindingFailCount++;

    UE_LOG(LogFFDAI, Warning, TEXT("[%s] Pathfinding failed (count: %d)"),
        *AnimatronicID.ToString(), PathfindingFailCount);

    if (PathfindingFailCount >= MaxPathfindingFailures)
    {
        // Teleport to nearest patrol point as fallback
        if (PatrolWaypoints.Num() > 0)
        {
            AActor* NearestWaypoint = nullptr;
            float NearestDist = MAX_FLT;

            for (AActor* WP : PatrolWaypoints)
            {
                if (!WP) continue;
                const float Dist = FVector::Dist(GetActorLocation(), WP->GetActorLocation());
                if (Dist < NearestDist)
                {
                    NearestDist = Dist;
                    NearestWaypoint = WP;
                }
            }

            if (NearestWaypoint)
            {
                SetActorLocation(NearestWaypoint->GetActorLocation());
                UE_LOG(LogFFDAI, Warning,
                    TEXT("[%s] Teleported to nearest waypoint as fallback"),
                    *AnimatronicID.ToString());
            }
        }

        PathfindingFailCount = 0;
        SetState(EAnimatronicState::Idle);
    }
}

FVector AAnimatronicBase::GetRandomPatrolPoint() const
{
    if (PatrolWaypoints.Num() > 0)
    {
        const int32 Index = FMath::RandRange(0, PatrolWaypoints.Num() - 1);
        if (PatrolWaypoints[Index])
        {
            return PatrolWaypoints[Index]->GetActorLocation();
        }
    }

    // Fallback: random point on NavMesh near current location
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (NavSys)
    {
        FNavLocation NavLoc;
        if (NavSys->GetRandomReachablePointInRadius(GetActorLocation(), 1500.0f, NavLoc))
        {
            return NavLoc.Location;
        }
    }

    return GetActorLocation();
}

bool AAnimatronicBase::PerformLineOfSightCheck() const
{
    AActor* Player = GetPlayerTarget();
    if (!Player) return false;

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.AddIgnoredActor(Player);

    // Trace from eye height
    const FVector Start = GetActorLocation() + FVector(0, 0, 80.0f);
    const FVector End = Player->GetActorLocation() + FVector(0, 0, 60.0f);

    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult, Start, End, ECC_Visibility, Params);

    // If we didn't hit anything, we have line of sight
    return !bHit;
}

bool AAnimatronicBase::MoveToLocation(const FVector& Target)
{
    AAIController* AIC = Cast<AAIController>(GetController());
    if (!AIC) return false;

    const EPathFollowingRequestResult::Type Result =
        AIC->MoveToLocation(Target, 50.0f, true, true, false, true);

    return Result != EPathFollowingRequestResult::Failed;
}

bool AAnimatronicBase::MoveToPlayer()
{
    AActor* Player = GetPlayerTarget();
    if (!Player) return false;

    AAIController* AIC = Cast<AAIController>(GetController());
    if (!AIC) return false;

    const EPathFollowingRequestResult::Type Result =
        AIC->MoveToActor(Player, AttackRange * 0.8f, true, true, false, nullptr, true);

    return Result != EPathFollowingRequestResult::Failed;
}
