// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "FFDMainMenuWidget.h"
#include "FredbearsFamilyDiner/Core/FFDGameInstance.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UFFDMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind button clicks
    if (PlayButton) PlayButton->OnClicked.AddDynamic(this, &UFFDMainMenuWidget::OnPlayClicked);
    if (SettingsButton) SettingsButton->OnClicked.AddDynamic(this, &UFFDMainMenuWidget::OnSettingsClicked);
    if (ExtrasButton) ExtrasButton->OnClicked.AddDynamic(this, &UFFDMainMenuWidget::OnExtrasClicked);
    if (QuitButton) QuitButton->OnClicked.AddDynamic(this, &UFFDMainMenuWidget::OnQuitClicked);
    if (StartNightButton) StartNightButton->OnClicked.AddDynamic(this, &UFFDMainMenuWidget::OnStartNightClicked);
    if (BackButton) BackButton->OnClicked.AddDynamic(this, &UFFDMainMenuWidget::OnBackClicked);

    // Difficulty buttons
    if (EasyButton) EasyButton->OnClicked.AddDynamic(this, &UFFDMainMenuWidget::OnPlayClicked);
    if (NormalButton) NormalButton->OnClicked.AddDynamic(this, &UFFDMainMenuWidget::OnPlayClicked);
    if (HardButton) HardButton->OnClicked.AddDynamic(this, &UFFDMainMenuWidget::OnPlayClicked);
    if (NightmareButton) NightmareButton->OnClicked.AddDynamic(this, &UFFDMainMenuWidget::OnPlayClicked);

    // Check if extras are unlocked
    if (UFFDGameInstance* GI = Cast<UFFDGameInstance>(GetGameInstance()))
    {
        if (ExtrasButton)
        {
            ExtrasButton->SetIsEnabled(GI->IsExtrasUnlocked());
        }
    }

    // Show main menu page
    if (MenuSwitcher)
    {
        MenuSwitcher->SetActiveWidgetIndex(0);
    }
}

void UFFDMainMenuWidget::OnPlayClicked()
{
    // Switch to night select page
    if (MenuSwitcher)
    {
        MenuSwitcher->SetActiveWidgetIndex(1);
    }
}

void UFFDMainMenuWidget::OnSettingsClicked()
{
    // Switch to settings page
    if (MenuSwitcher)
    {
        MenuSwitcher->SetActiveWidgetIndex(2);
    }
}

void UFFDMainMenuWidget::OnExtrasClicked()
{
    // Switch to extras page
    if (MenuSwitcher)
    {
        MenuSwitcher->SetActiveWidgetIndex(3);
    }
}

void UFFDMainMenuWidget::OnQuitClicked()
{
    UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
}

void UFFDMainMenuWidget::OnStartNightClicked()
{
    if (UFFDGameInstance* GI = Cast<UFFDGameInstance>(GetGameInstance()))
    {
        GI->SelectedDifficulty = SelectedDifficulty;
        GI->SelectedNight = SelectedNight;
    }

    // Load the game level
    UGameplayStatics::OpenLevel(this, FName(TEXT("DinerMap")));
}

void UFFDMainMenuWidget::OnBackClicked()
{
    if (MenuSwitcher)
    {
        MenuSwitcher->SetActiveWidgetIndex(0);
    }
}

void UFFDMainMenuWidget::OnDifficultyChanged(EDifficulty NewDifficulty)
{
    SelectedDifficulty = NewDifficulty;
}
