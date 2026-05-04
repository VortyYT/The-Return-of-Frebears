// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FFDSettingsWidget.generated.h"

class USlider;
class UTextBlock;
class UButton;
class UComboBoxString;

/**
 * Settings menu for Graphics, Audio, and Keybinds.
 */
UCLASS()
class FREDBEARSFAMILYDINER_API UFFDSettingsWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:
    // --- Audio Settings ---

    UFUNCTION()
    void OnMasterVolumeChanged(float Value);

    UFUNCTION()
    void OnMusicVolumeChanged(float Value);

    UFUNCTION()
    void OnSFXVolumeChanged(float Value);

    UFUNCTION()
    void OnMouseSensitivityChanged(float Value);

    // --- Graphics Settings ---

    UFUNCTION()
    void OnQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

    UFUNCTION()
    void OnFullscreenToggled();

    UFUNCTION()
    void OnVSyncToggled();

    // --- Actions ---

    UFUNCTION()
    void OnApplyClicked();

    UFUNCTION()
    void OnResetClicked();

    UFUNCTION()
    void OnBackClicked();

    void LoadCurrentSettings();
    void SaveSettings();

    // --- Audio UI ---

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USlider> MasterVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> MasterVolumeValue;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USlider> MusicVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> MusicVolumeValue;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USlider> SFXVolumeSlider;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> SFXVolumeValue;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<USlider> MouseSensitivitySlider;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> MouseSensitivityValue;

    // --- Graphics UI ---

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UComboBoxString> QualityComboBox;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> FullscreenButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> VSyncButton;

    // --- Action Buttons ---

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> ApplyButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> ResetButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> BackButton;

    bool bFullscreen = false;
    bool bVSync = true;
    int32 QualityLevel = 3; // 0=Low, 1=Med, 2=High, 3=Ultra
};
