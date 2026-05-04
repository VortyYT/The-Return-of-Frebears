// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FFDInteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionTargetChanged, AActor*, NewTarget);

/**
 * Manages interaction with world objects (doors, cameras, collectibles).
 * Works via an interface check on targeted actors.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FREDBEARSFAMILYDINER_API UFFDInteractionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFFDInteractionComponent();

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void SetCurrentTarget(AActor* NewTarget);

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void TryInteract();

    UFUNCTION(BlueprintPure, Category = "Interaction")
    AActor* GetCurrentTarget() const { return CurrentTarget; }

    UFUNCTION(BlueprintPure, Category = "Interaction")
    bool HasValidTarget() const;

    UFUNCTION(BlueprintPure, Category = "Interaction")
    FText GetInteractionPrompt() const;

    UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
    FOnInteractionTargetChanged OnInteractionTargetChanged;

protected:
    UPROPERTY()
    TObjectPtr<AActor> CurrentTarget;
};
