// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "FFDExtrasWidget.h"
#include "FredbearsFamilyDiner/Core/FFDGameInstance.h"
#include "FredbearsFamilyDiner/Core/FFDSaveGame.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/WidgetSwitcher.h"

void UFFDExtrasWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (LoreButton) LoreButton->OnClicked.AddDynamic(this, &UFFDExtrasWidget::OnLoreClicked);
    if (StatsButton) StatsButton->OnClicked.AddDynamic(this, &UFFDExtrasWidget::OnStatsClicked);
    if (AnimatronicsButton) AnimatronicsButton->OnClicked.AddDynamic(
        this, &UFFDExtrasWidget::OnAnimatronicsClicked);
    if (BackButton) BackButton->OnClicked.AddDynamic(this, &UFFDExtrasWidget::OnBackClicked);

    RefreshContent();
}

void UFFDExtrasWidget::RefreshContent()
{
    PopulateLoreEntries();
    PopulateStatistics();
    PopulateAnimatronicProfiles();

    if (ContentSwitcher)
    {
        ContentSwitcher->SetActiveWidgetIndex(0);
    }
}

void UFFDExtrasWidget::OnLoreClicked()
{
    if (ContentSwitcher) ContentSwitcher->SetActiveWidgetIndex(0);
}

void UFFDExtrasWidget::OnStatsClicked()
{
    if (ContentSwitcher) ContentSwitcher->SetActiveWidgetIndex(1);
}

void UFFDExtrasWidget::OnAnimatronicsClicked()
{
    if (ContentSwitcher) ContentSwitcher->SetActiveWidgetIndex(2);
}

void UFFDExtrasWidget::OnBackClicked()
{
    RemoveFromParent();
}

void UFFDExtrasWidget::PopulateLoreEntries()
{
    if (!LoreScrollBox) return;
    LoreScrollBox->ClearChildren();

    UFFDGameInstance* GI = Cast<UFFDGameInstance>(GetGameInstance());
    if (!GI || !GI->GetSaveData()) return;

    const UFFDSaveGame* SaveData = GI->GetSaveData();

    // Create text entries for each collected lore item
    for (const FName& LoreID : SaveData->CollectedLoreItems)
    {
        UTextBlock* Entry = NewObject<UTextBlock>(this);
        Entry->SetText(FText::FromName(LoreID));
        Entry->SetAutoWrapText(true);
        LoreScrollBox->AddChild(Entry);
    }

    if (SaveData->CollectedLoreItems.Num() == 0)
    {
        UTextBlock* EmptyText = NewObject<UTextBlock>(this);
        EmptyText->SetText(FText::FromString(TEXT("No lore items collected yet.")));
        LoreScrollBox->AddChild(EmptyText);
    }
}

void UFFDExtrasWidget::PopulateStatistics()
{
    if (!StatsScrollBox) return;
    StatsScrollBox->ClearChildren();

    UFFDGameInstance* GI = Cast<UFFDGameInstance>(GetGameInstance());
    if (!GI || !GI->GetSaveData()) return;

    const UFFDSaveGame* SaveData = GI->GetSaveData();

    auto AddStat = [this](const FString& Label, const FString& Value)
    {
        UTextBlock* Entry = NewObject<UTextBlock>(this);
        Entry->SetText(FText::FromString(FString::Printf(TEXT("%s: %s"), *Label, *Value)));
        StatsScrollBox->AddChild(Entry);
    };

    AddStat(TEXT("Highest Night Completed"), FString::FromInt(SaveData->HighestNightCompleted));
    AddStat(TEXT("Total Deaths"), FString::FromInt(SaveData->TotalDeaths));
    AddStat(TEXT("Total Jumpscares"), FString::FromInt(SaveData->TotalJumpscares));
    AddStat(TEXT("Nights Attempted"), FString::FromInt(SaveData->TotalNightsAttempted));
    AddStat(TEXT("Nights Completed"), FString::FromInt(SaveData->TotalNightsCompleted));
    AddStat(TEXT("Total Play Time"),
        FString::Printf(TEXT("%.1f hours"), SaveData->TotalPlayTime / 3600.0f));
    AddStat(TEXT("Lore Items Found"),
        FString::FromInt(SaveData->CollectedLoreItems.Num()));
}

void UFFDExtrasWidget::PopulateAnimatronicProfiles()
{
    if (!AnimatronicsScrollBox) return;
    AnimatronicsScrollBox->ClearChildren();

    // Static profiles for each animatronic
    struct FProfileEntry
    {
        FString Name;
        FString Description;
    };

    const TArray<FProfileEntry> Profiles = {
        { TEXT("Fredbear"),
          TEXT("The star of Fredbear's Family Diner. A golden bear animatronic "
               "that performs on the main stage. Known for his unsettling smile "
               "and tendency to wander after hours.") },
        { TEXT("Spring Bonnie"),
          TEXT("Fredbear's companion on stage. A golden rabbit animatronic with "
               "a spring-lock suit mechanism. Fond of using the vent systems "
               "to move around undetected.") },
        { TEXT("Golden Foxy"),
          TEXT("A mysterious broken animatronic found in the storage room. "
               "Origin unknown. Capable of terrifying rush attacks down the "
               "hallways. Grows more aggressive as the diner's power drops.") }
    };

    for (const auto& Profile : Profiles)
    {
        UTextBlock* NameText = NewObject<UTextBlock>(this);
        NameText->SetText(FText::FromString(Profile.Name));
        AnimatronicsScrollBox->AddChild(NameText);

        UTextBlock* DescText = NewObject<UTextBlock>(this);
        DescText->SetText(FText::FromString(Profile.Description));
        DescText->SetAutoWrapText(true);
        AnimatronicsScrollBox->AddChild(DescText);
    }
}
