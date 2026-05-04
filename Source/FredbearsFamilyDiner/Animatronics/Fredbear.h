// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimatronicBase.h"
#include "Fredbear.generated.h"

/**
 * Fredbear — the main animatronic and primary threat.
 * 
 * Behavior: Starts on stage, gradually becomes more active as the night
 * progresses. Prefers to stalk the player before attacking. Can hide
 * in dark areas and ambush. Has a unique "stage check" mechanic where
 * looking at him on cameras delays his activation.
 */
UCLASS()
class FREDBEARSFAMILYDINER_API AFredbear : public AAnimatronicBase
{
    GENERATED_BODY()

public:
    AFredbear();

    virtual void ActivateAnimatronic(float DifficultyMultiplier, int32 NightNumber) override;

    /** Check if Fredbear is still on stage */
    UFUNCTION(BlueprintPure, Category = "Fredbear")
    bool IsOnStage() const { return bIsOnStage; }

    /** Called when the player looks at Fredbear through cameras */
    UFUNCTION(BlueprintCallable, Category = "Fredbear")
    void OnCameraObserved();

protected:
    virtual void TickIdle(float DeltaTime) override;
    virtual void TickHunting(float DeltaTime) override;
    virtual void TickStalking(float DeltaTime) override;
    virtual void TickAggressive(float DeltaTime) override;

    /** Fredbear-specific: attempt to hide in a dark area near the player */
    void TryAmbushPosition();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fredbear|Config")
    float StageLeaveDelay = 60.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fredbear|Config")
    float CameraObservationDelay = 15.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fredbear|Config")
    float AmbushRange = 500.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fredbear|State")
    bool bIsOnStage = true;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fredbear|State")
    float StageTimer = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fredbear|State")
    float CameraObservationTimer = 0.0f;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Fredbear|Config")
    TObjectPtr<AActor> StagePosition;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Fredbear|Config")
    TArray<AActor*> AmbushPositions;
};
