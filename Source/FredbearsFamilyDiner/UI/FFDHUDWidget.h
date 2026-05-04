// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FFDHUDWidget.generated.h"

class UTextBlock;
class UProgressBar;
class UImage;
class UCanvasPanel;

/**
 * Minimal in-game HUD showing:
 * - Current time (e.g., "2 AM")
 * - Power level bar
 * - Stamina bar (only visible when draining)
 * - Flashlight battery indicator
 * - Interaction prompt
 * - Power usage indicators
 */
UCLASS()
class FREDBEARSFAMILYDINER_API UFFDHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowInteractionPrompt(const FText& Prompt);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void HideInteractionPrompt();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowDeathScreen();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowVictoryScreen();

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateTime(int32 Hour);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdatePower(float PowerPercent);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateStamina(float StaminaPercent);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateBattery(float BatteryPercent);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdatePowerUsage(int32 UsageLevel);

protected:
    // --- Time Display ---

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TimeText;

    // --- Power ---

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> PowerText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> PowerBar;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> UsageLevelText;

    // --- Stamina ---

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> StaminaBar;

    // --- Battery ---

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> BatteryBar;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> FlashlightIcon;

    // --- Interaction ---

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> InteractionPromptText;

    // --- Overlays ---

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCanvasPanel> DeathOverlay;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UCanvasPanel> VictoryOverlay;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> VictoryTimeText;
};
