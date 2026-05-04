// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FFDInteractableInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UFFDInteractableInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 * Interface for all interactable objects in the diner.
 * Implemented by doors, cameras, collectibles, switches, etc.
 */
class FREDBEARSFAMILYDINER_API IFFDInteractableInterface
{
    GENERATED_BODY()

public:
    /** Called when the player interacts with this object */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void OnInteract(AActor* Interactor);

    /** Get the prompt text shown to the player */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    FText GetInteractionPrompt() const;

    /** Whether this object can currently be interacted with */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    bool CanBeInteracted() const;
};
