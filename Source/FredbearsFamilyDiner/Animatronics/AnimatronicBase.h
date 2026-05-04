// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FredbearsFamilyDiner/FredbearsFamilyDiner.h"
#include "AnimatronicBase.generated.h"

class UAnimatronicPerceptionComponent;
class UAnimatronicStateComponent;
class UAudioComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAnimatronicStateChanged,
    EAnimatronicState, OldState, EAnimatronicState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnimatronicAttack, AActor*, Victim);

/**
 * Base class for all animatronics. Provides:
 * - State machine (Idle, Patrolling, Hunting, Stalking, Aggressive, Retreating)
 * - Perception (sight + hearing)
 * - Sound generation for player detection
 * - Attack/jumpscare triggering
 * - Fallback behaviors when pathfinding fails
 */
UCLASS(Abstract)
class FREDBEARSFAMILYDINER_API AAnimatronicBase : public ACharacter
{
    GENERATED_BODY()

public:
    AAnimatronicBase();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // --- Activation ---

    UFUNCTION(BlueprintCallable, Category = "Animatronic")
    virtual void ActivateAnimatronic(float DifficultyMultiplier, int32 NightNumber);

    UFUNCTION(BlueprintCallable, Category = "Animatronic")
    virtual void DeactivateAnimatronic();

    UFUNCTION(BlueprintPure, Category = "Animatronic")
    bool IsActive() const { return bIsActive; }

    // --- State ---

    UFUNCTION(BlueprintPure, Category = "Animatronic")
    EAnimatronicState GetCurrentState() const { return CurrentState; }

    UFUNCTION(BlueprintCallable, Category = "Animatronic")
    void SetState(EAnimatronicState NewState);

    // --- Combat ---

    UFUNCTION(BlueprintCallable, Category = "Animatronic")
    virtual void AttackPlayer();

    UFUNCTION(BlueprintPure, Category = "Animatronic")
    bool CanAttackPlayer() const;

    UFUNCTION(BlueprintPure, Category = "Animatronic")
    float GetAttackRange() const { return AttackRange; }

    // --- Perception ---

    UFUNCTION(BlueprintPure, Category = "Animatronic")
    bool CanSeePlayer() const;

    UFUNCTION(BlueprintPure, Category = "Animatronic")
    bool CanHearPlayer() const;

    UFUNCTION(BlueprintPure, Category = "Animatronic")
    float GetDistanceToPlayer() const;

    UFUNCTION(BlueprintPure, Category = "Animatronic")
    AActor* GetPlayerTarget() const;

    // --- Identity ---

    UFUNCTION(BlueprintPure, Category = "Animatronic")
    FName GetAnimatronicID() const { return AnimatronicID; }

    UFUNCTION(BlueprintPure, Category = "Animatronic")
    FText GetDisplayName() const { return DisplayName; }

    // --- Sound ---

    UFUNCTION(BlueprintCallable, Category = "Animatronic")
    void PlayMovementSound();

    UFUNCTION(BlueprintCallable, Category = "Animatronic")
    void PlayAmbientSound();

    // --- Delegates ---

    UPROPERTY(BlueprintAssignable, Category = "Animatronic|Events")
    FOnAnimatronicStateChanged OnStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Animatronic|Events")
    FOnAnimatronicAttack OnAttack;

protected:
    // --- Behavior (override in subclasses) ---

    virtual void TickIdle(float DeltaTime);
    virtual void TickPatrolling(float DeltaTime);
    virtual void TickHunting(float DeltaTime);
    virtual void TickStalking(float DeltaTime);
    virtual void TickAggressive(float DeltaTime);
    virtual void TickRetreating(float DeltaTime);

    /** Called when pathfinding fails — subclasses can override for unique fallback */
    virtual void HandlePathfindingFailure();

    /** Get a random patrol point from available waypoints */
    FVector GetRandomPatrolPoint() const;

    /** Check if we have line of sight to the player */
    bool PerformLineOfSightCheck() const;

    /** Move toward a target location using NavMesh */
    bool MoveToLocation(const FVector& Target);

    /** Move toward the player */
    bool MoveToPlayer();

    // --- Components ---

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UAudioComponent> MovementAudioComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UAudioComponent> AmbientAudioComponent;

    // --- Identity ---

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animatronic|Identity")
    FName AnimatronicID;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animatronic|Identity")
    FText DisplayName;

    // --- Config ---

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animatronic|Config")
    float BaseMovementSpeed = 200.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animatronic|Config")
    float HuntingSpeedMultiplier = 1.5f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animatronic|Config")
    float AggressiveSpeedMultiplier = 2.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animatronic|Config")
    float AttackRange = 150.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animatronic|Config")
    float SightRange = FFDConstants::AIBaseDetectionRange;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animatronic|Config")
    float HearingRange = FFDConstants::AIHearingRange;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animatronic|Config")
    float SightAngle = 90.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animatronic|Config")
    float PatrolWaitTimeMin = FFDConstants::AIPatrolWaitMin;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animatronic|Config")
    float PatrolWaitTimeMax = FFDConstants::AIPatrolWaitMax;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animatronic|Config")
    float AggroDecayRate = 2.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animatronic|Config")
    float AggroThreshold = 50.0f;

    /** Base aggression level (0-20). Affects how readily this animatronic hunts. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animatronic|Config",
        meta = (ClampMin = "0", ClampMax = "20"))
    int32 BaseAggressionLevel = 5;

    // --- Audio ---

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animatronic|Audio")
    TObjectPtr<USoundBase> MovementSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animatronic|Audio")
    TObjectPtr<USoundBase> AmbientSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animatronic|Audio")
    TObjectPtr<USoundBase> JumpscareSound;

    // --- Patrol Waypoints ---

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Animatronic|Patrol")
    TArray<AActor*> PatrolWaypoints;

    // --- Runtime State ---

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animatronic|State")
    EAnimatronicState CurrentState = EAnimatronicState::Deactivated;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animatronic|State")
    bool bIsActive = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animatronic|State")
    float CurrentAggro = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animatronic|State")
    float DifficultyMult = 1.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animatronic|State")
    FVector LastKnownPlayerLocation = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animatronic|State")
    float TimeSinceLastSawPlayer = 0.0f;

    float PatrolWaitTimer = 0.0f;
    bool bIsWaitingAtPatrolPoint = false;
    int32 CurrentPatrolIndex = 0;
    int32 PathfindingFailCount = 0;
    static constexpr int32 MaxPathfindingFailures = 5;
};
