// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FFDExtrasWidget.generated.h"

class UButton;
class UTextBlock;
class UScrollBox;
class UWidgetSwitcher;

/**
 * Extras menu unlocked after completing the game.
 * Shows lore entries, game statistics, animatronic profiles,
 * and secret content.
 */
UCLASS()
class FREDBEARSFAMILYDINER_API UFFDExtrasWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, Category = "Extras")
    void RefreshContent();

protected:
    UFUNCTION()
    void OnLoreClicked();

    UFUNCTION()
    void OnStatsClicked();

    UFUNCTION()
    void OnAnimatronicsClicked();

    UFUNCTION()
    void OnBackClicked();

    void PopulateLoreEntries();
    void PopulateStatistics();
    void PopulateAnimatronicProfiles();

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> LoreButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> StatsButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> AnimatronicsButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> BackButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UWidgetSwitcher> ContentSwitcher;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UScrollBox> LoreScrollBox;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UScrollBox> StatsScrollBox;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UScrollBox> AnimatronicsScrollBox;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> ExtrasTitle;
};
