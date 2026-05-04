// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FFDInteractableInterface.h"
#include "LoreCollectible.generated.h"

class UStaticMeshComponent;
class UWidgetComponent;

UENUM(BlueprintType)
enum class ELoreType : uint8
{
    Poster      UMETA(DisplayName = "Poster"),
    Newspaper   UMETA(DisplayName = "Newspaper"),
    Note        UMETA(DisplayName = "Note"),
    Recording   UMETA(DisplayName = "Recording"),
    Photo       UMETA(DisplayName = "Photo"),
    HiddenItem  UMETA(DisplayName = "Hidden Item")
};

/**
 * Collectible lore item that reveals story elements.
 * Can be posters, notes, recordings, or hidden items.
 */
UCLASS()
class FREDBEARSFAMILYDINER_API ALoreCollectible : public AActor, public IFFDInteractableInterface
{
    GENERATED_BODY()

public:
    ALoreCollectible();

    virtual void BeginPlay() override;

    // --- IFFDInteractableInterface ---
    virtual void OnInteract_Implementation(AActor* Interactor) override;
    virtual FText GetInteractionPrompt_Implementation() const override;
    virtual bool CanBeInteracted_Implementation() const override;

    UFUNCTION(BlueprintPure, Category = "Lore")
    FName GetLoreID() const { return LoreID; }

    UFUNCTION(BlueprintPure, Category = "Lore")
    FText GetLoreTitle() const { return LoreTitle; }

    UFUNCTION(BlueprintPure, Category = "Lore")
    FText GetLoreContent() const { return LoreContent; }

    UFUNCTION(BlueprintPure, Category = "Lore")
    ELoreType GetLoreType() const { return LoreType; }

    UFUNCTION(BlueprintPure, Category = "Lore")
    bool IsCollected() const { return bCollected; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Mesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lore|Config")
    FName LoreID;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lore|Config")
    FText LoreTitle;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lore|Config",
        meta = (MultiLine = "true"))
    FText LoreContent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lore|Config")
    ELoreType LoreType = ELoreType::Note;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lore|Config")
    TObjectPtr<USoundBase> CollectSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lore|Config")
    TObjectPtr<USoundBase> AudioRecording;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lore|Config")
    TObjectPtr<UTexture2D> LoreImage;

    /** Whether to hide this collectible (for secret lore) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lore|Config")
    bool bIsHidden = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lore|State")
    bool bCollected = false;
};
