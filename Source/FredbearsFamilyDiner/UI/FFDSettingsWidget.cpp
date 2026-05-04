// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "FFDSettingsWidget.h"
#include "FredbearsFamilyDiner/Core/FFDGameInstance.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "GameFramework/GameUserSettings.h"

void UFFDSettingsWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind sliders
    if (MasterVolumeSlider) MasterVolumeSlider->OnValueChanged.AddDynamic(
        this, &UFFDSettingsWidget::OnMasterVolumeChanged);
    if (MusicVolumeSlider) MusicVolumeSlider->OnValueChanged.AddDynamic(
        this, &UFFDSettingsWidget::OnMusicVolumeChanged);
    if (SFXVolumeSlider) SFXVolumeSlider->OnValueChanged.AddDynamic(
        this, &UFFDSettingsWidget::OnSFXVolumeChanged);
    if (MouseSensitivitySlider) MouseSensitivitySlider->OnValueChanged.AddDynamic(
        this, &UFFDSettingsWidget::OnMouseSensitivityChanged);

    // Bind buttons
    if (ApplyButton) ApplyButton->OnClicked.AddDynamic(
        this, &UFFDSettingsWidget::OnApplyClicked);
    if (ResetButton) ResetButton->OnClicked.AddDynamic(
        this, &UFFDSettingsWidget::OnResetClicked);
    if (BackButton) BackButton->OnClicked.AddDynamic(
        this, &UFFDSettingsWidget::OnBackClicked);
    if (FullscreenButton) FullscreenButton->OnClicked.AddDynamic(
        this, &UFFDSettingsWidget::OnFullscreenToggled);
    if (VSyncButton) VSyncButton->OnClicked.AddDynamic(
        this, &UFFDSettingsWidget::OnVSyncToggled);

    // Quality options
    if (QualityComboBox)
    {
        QualityComboBox->AddOption(TEXT("Low"));
        QualityComboBox->AddOption(TEXT("Medium"));
        QualityComboBox->AddOption(TEXT("High"));
        QualityComboBox->AddOption(TEXT("Ultra"));
        QualityComboBox->OnSelectionChanged.AddDynamic(
            this, &UFFDSettingsWidget::OnQualityChanged);
    }

    LoadCurrentSettings();
}

void UFFDSettingsWidget::OnMasterVolumeChanged(float Value)
{
    if (MasterVolumeValue)
    {
        MasterVolumeValue->SetText(FText::FromString(
            FString::Printf(TEXT("%d%%"), FMath::RoundToInt(Value * 100.0f))));
    }
}

void UFFDSettingsWidget::OnMusicVolumeChanged(float Value)
{
    if (MusicVolumeValue)
    {
        MusicVolumeValue->SetText(FText::FromString(
            FString::Printf(TEXT("%d%%"), FMath::RoundToInt(Value * 100.0f))));
    }
}

void UFFDSettingsWidget::OnSFXVolumeChanged(float Value)
{
    if (SFXVolumeValue)
    {
        SFXVolumeValue->SetText(FText::FromString(
            FString::Printf(TEXT("%d%%"), FMath::RoundToInt(Value * 100.0f))));
    }
}

void UFFDSettingsWidget::OnMouseSensitivityChanged(float Value)
{
    if (MouseSensitivityValue)
    {
        MouseSensitivityValue->SetText(FText::FromString(
            FString::Printf(TEXT("%.1f"), Value * 5.0f)));
    }
}

void UFFDSettingsWidget::OnQualityChanged(FString SelectedItem,
    ESelectInfo::Type SelectionType)
{
    if (SelectedItem == TEXT("Low")) QualityLevel = 0;
    else if (SelectedItem == TEXT("Medium")) QualityLevel = 1;
    else if (SelectedItem == TEXT("High")) QualityLevel = 2;
    else QualityLevel = 3;
}

void UFFDSettingsWidget::OnFullscreenToggled()
{
    bFullscreen = !bFullscreen;
}

void UFFDSettingsWidget::OnVSyncToggled()
{
    bVSync = !bVSync;
}

void UFFDSettingsWidget::OnApplyClicked()
{
    SaveSettings();
}

void UFFDSettingsWidget::OnResetClicked()
{
    if (MasterVolumeSlider) MasterVolumeSlider->SetValue(1.0f);
    if (MusicVolumeSlider) MusicVolumeSlider->SetValue(0.8f);
    if (SFXVolumeSlider) SFXVolumeSlider->SetValue(1.0f);
    if (MouseSensitivitySlider) MouseSensitivitySlider->SetValue(0.2f);
    QualityLevel = 3;
    bFullscreen = false;
    bVSync = true;

    if (QualityComboBox) QualityComboBox->SetSelectedOption(TEXT("Ultra"));
}

void UFFDSettingsWidget::OnBackClicked()
{
    RemoveFromParent();
}

void UFFDSettingsWidget::LoadCurrentSettings()
{
    UFFDGameInstance* GI = Cast<UFFDGameInstance>(GetGameInstance());
    if (!GI) return;

    if (MasterVolumeSlider) MasterVolumeSlider->SetValue(GI->GetMasterVolume());
    if (MusicVolumeSlider) MusicVolumeSlider->SetValue(GI->GetMusicVolume());
    if (SFXVolumeSlider) SFXVolumeSlider->SetValue(GI->GetSFXVolume());
    if (MouseSensitivitySlider) MouseSensitivitySlider->SetValue(
        GI->GetMouseSensitivity() / 5.0f);

    // Load graphics settings
    UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings();
    if (UserSettings)
    {
        QualityLevel = UserSettings->GetOverallScalabilityLevel();
        bFullscreen = UserSettings->GetFullscreenMode() == EWindowMode::Fullscreen;
        bVSync = UserSettings->IsVSyncEnabled();

        if (QualityComboBox)
        {
            const TArray<FString> QualityNames = {
                TEXT("Low"), TEXT("Medium"), TEXT("High"), TEXT("Ultra")
            };
            if (QualityNames.IsValidIndex(QualityLevel))
            {
                QualityComboBox->SetSelectedOption(QualityNames[QualityLevel]);
            }
        }
    }

    // Trigger value display updates
    OnMasterVolumeChanged(GI->GetMasterVolume());
    OnMusicVolumeChanged(GI->GetMusicVolume());
    OnSFXVolumeChanged(GI->GetSFXVolume());
    OnMouseSensitivityChanged(GI->GetMouseSensitivity() / 5.0f);
}

void UFFDSettingsWidget::SaveSettings()
{
    UFFDGameInstance* GI = Cast<UFFDGameInstance>(GetGameInstance());
    if (GI)
    {
        if (MasterVolumeSlider) GI->SetMasterVolume(MasterVolumeSlider->GetValue());
        if (MusicVolumeSlider) GI->SetMusicVolume(MusicVolumeSlider->GetValue());
        if (SFXVolumeSlider) GI->SetSFXVolume(SFXVolumeSlider->GetValue());
        if (MouseSensitivitySlider) GI->SetMouseSensitivity(
            MouseSensitivitySlider->GetValue() * 5.0f);
        GI->SaveGame();
    }

    // Apply graphics settings
    UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings();
    if (UserSettings)
    {
        UserSettings->SetOverallScalabilityLevel(QualityLevel);
        UserSettings->SetFullscreenMode(bFullscreen ?
            EWindowMode::Fullscreen : EWindowMode::Windowed);
        UserSettings->SetVSyncEnabled(bVSync);
        UserSettings->ApplySettings(true);
        UserSettings->SaveSettings();
    }
}
