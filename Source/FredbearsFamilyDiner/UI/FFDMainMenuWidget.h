// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FredbearsFamilyDiner/FredbearsFamilyDiner.h"
#include "FFDMainMenuWidget.generated.h"

class UButton;
class UTextBlock;
class UWidgetSwitcher;

/**
 * Main menu widget with Play, Settings, Extras, and Quit buttons.
 * Shows night selection and difficulty options before starting.
 */
UCLASS()
class FREDBEARSFAMILYDINER_API UFFDMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:
    // --- Button Handlers ---

    UFUNCTION()
    void OnPlayClicked();

    UFUNCTION()
    void OnSettingsClicked();

    UFUNCTION()
    void OnExtrasClicked();

    UFUNCTION()
    void OnQuitClicked();

    UFUNCTION()
    void OnStartNightClicked();

    UFUNCTION()
    void OnBackClicked();

    UFUNCTION()
    void OnDifficultyChanged(EDifficulty NewDifficulty);

    // --- UI Elements ---

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> PlayButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> SettingsButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> ExtrasButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> QuitButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> StartNightButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> BackButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TitleText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> NightSelectText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UWidgetSwitcher> MenuSwitcher;

    // --- Difficulty Selection ---

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> EasyButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> NormalButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> HardButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> NightmareButton;

    EDifficulty SelectedDifficulty = EDifficulty::Normal;
    int32 SelectedNight = 1;
};
