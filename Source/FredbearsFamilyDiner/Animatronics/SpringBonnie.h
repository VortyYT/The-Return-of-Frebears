// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimatronicBase.h"
#include "SpringBonnie.generated.h"

/**
 * Spring Bonnie — the secondary animatronic.
 * 
 * Behavior: More mobile than Fredbear, prefers vent systems and
 * back hallways. Uses hit-and-run tactics: approaches quickly,
 * retreats if spotted, then flanks from another direction.
 * Reacts strongly to player noise.
 */
UCLASS()
class FREDBEARSFAMILYDINER_API ASpringBonnie : public AAnimatronicBase
{
    GENERATED_BODY()

public:
    ASpringBonnie();

    virtual void ActivateAnimatronic(float DifficultyMultiplier, int32 NightNumber) override;

    /** Called when Spring Bonnie reaches a vent entrance */
    UFUNCTION(BlueprintCallable, Category = "SpringBonnie")
    void EnterVent();

    UFUNCTION(BlueprintCallable, Category = "SpringBonnie")
    void ExitVent();

    UFUNCTION(BlueprintPure, Category = "SpringBonnie")
    bool IsInVent() const { return bIsInVent; }

protected:
    virtual void TickPatrolling(float DeltaTime) override;
    virtual void TickHunting(float DeltaTime) override;
    virtual void TickStalking(float DeltaTime) override;

    /** Find a flanking route to the player */
    FVector GetFlankingPosition() const;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpringBonnie|Config")
    float VentTravelTime = 5.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpringBonnie|Config")
    float RetreatChance = 0.3f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpringBonnie|Config")
    float NoiseReactionMultiplier = 2.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpringBonnie|State")
    bool bIsInVent = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpringBonnie|State")
    float VentTimer = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpringBonnie|State")
    bool bIsRetreating = false;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "SpringBonnie|Config")
    TArray<AActor*> VentEntrances;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "SpringBonnie|Config")
    TArray<AActor*> VentExits;
};
