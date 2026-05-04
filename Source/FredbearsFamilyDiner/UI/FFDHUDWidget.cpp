// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "FFDHUDWidget.h"
#include "FredbearsFamilyDiner/Core/FFDGameMode.h"
#include "FredbearsFamilyDiner/Player/FFDPlayerCharacter.h"
#include "FredbearsFamilyDiner/Player/FFDFlashlightComponent.h"
#include "FredbearsFamilyDiner/Player/FFDStaminaComponent.h"
#include "FredbearsFamilyDiner/Player/FFDInteractionComponent.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"

void UFFDHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Hide overlays initially
    if (DeathOverlay) DeathOverlay->SetVisibility(ESlateVisibility::Collapsed);
    if (VictoryOverlay) VictoryOverlay->SetVisibility(ESlateVisibility::Collapsed);
    if (InteractionPromptText) InteractionPromptText->SetVisibility(ESlateVisibility::Collapsed);

    // Initial power display
    UpdatePower(100.0f);
    UpdatePowerUsage(1);
}

void UFFDHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // Update from player components
    AFFDPlayerCharacter* Player = Cast<AFFDPlayerCharacter>(
        UGameplayStatics::GetPlayerCharacter(this, 0));
    if (!Player) return;

    // Stamina
    if (UFFDStaminaComponent* Stamina = Player->GetStamina())
    {
        const float StaminaPct = Stamina->GetStaminaPercent();
        UpdateStamina(StaminaPct);

        // Only show stamina bar when relevant
        if (StaminaBar)
        {
            const bool bShowStamina = Stamina->IsDraining() || StaminaPct < 100.0f;
            StaminaBar->SetVisibility(bShowStamina ?
                ESlateVisibility::Visible : ESlateVisibility::Collapsed);
        }
    }

    // Battery
    if (UFFDFlashlightComponent* Flashlight = Player->GetFlashlight())
    {
        UpdateBattery(Flashlight->GetBatteryPercent());

        if (FlashlightIcon)
        {
            FlashlightIcon->SetVisibility(Flashlight->IsFlashlightOn() ?
                ESlateVisibility::Visible : ESlateVisibility::Collapsed);
        }
    }

    // Interaction prompt
    if (UFFDInteractionComponent* Interaction = Player->FindComponentByClass<UFFDInteractionComponent>())
    {
        if (Interaction->HasValidTarget())
        {
            ShowInteractionPrompt(Interaction->GetInteractionPrompt());
        }
        else
        {
            HideInteractionPrompt();
        }
    }
}

void UFFDHUDWidget::ShowInteractionPrompt(const FText& Prompt)
{
    if (InteractionPromptText)
    {
        InteractionPromptText->SetText(
            FText::Format(NSLOCTEXT("HUD", "InteractPrompt", "[E] {0}"),
                Prompt));
        InteractionPromptText->SetVisibility(ESlateVisibility::Visible);
    }
}

void UFFDHUDWidget::HideInteractionPrompt()
{
    if (InteractionPromptText)
    {
        InteractionPromptText->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UFFDHUDWidget::ShowDeathScreen()
{
    if (DeathOverlay)
    {
        DeathOverlay->SetVisibility(ESlateVisibility::Visible);
    }
}

void UFFDHUDWidget::ShowVictoryScreen()
{
    if (VictoryOverlay)
    {
        VictoryOverlay->SetVisibility(ESlateVisibility::Visible);
    }

    if (VictoryTimeText)
    {
        VictoryTimeText->SetText(FText::FromString(TEXT("6 AM")));
    }
}

void UFFDHUDWidget::UpdateTime(int32 Hour)
{
    if (TimeText)
    {
        const FString TimeString = (Hour == 12)
            ? TEXT("12 AM")
            : FString::Printf(TEXT("%d AM"), Hour);
        TimeText->SetText(FText::FromString(TimeString));
    }
}

void UFFDHUDWidget::UpdatePower(float PowerPercent)
{
    if (PowerBar)
    {
        PowerBar->SetPercent(PowerPercent / 100.0f);

        // Color: green -> yellow -> red
        FLinearColor BarColor;
        if (PowerPercent > 50.0f)
        {
            BarColor = FLinearColor::LerpUsingHSV(
                FLinearColor(1.0f, 1.0f, 0.0f),
                FLinearColor(0.0f, 1.0f, 0.0f),
                (PowerPercent - 50.0f) / 50.0f);
        }
        else
        {
            BarColor = FLinearColor::LerpUsingHSV(
                FLinearColor(1.0f, 0.0f, 0.0f),
                FLinearColor(1.0f, 1.0f, 0.0f),
                PowerPercent / 50.0f);
        }
        PowerBar->SetFillColorAndOpacity(BarColor);
    }

    if (PowerText)
    {
        PowerText->SetText(FText::FromString(
            FString::Printf(TEXT("Power: %d%%"), FMath::RoundToInt(PowerPercent))));
    }
}

void UFFDHUDWidget::UpdateStamina(float StaminaPercent)
{
    if (StaminaBar)
    {
        StaminaBar->SetPercent(StaminaPercent / 100.0f);
    }
}

void UFFDHUDWidget::UpdateBattery(float BatteryPercent)
{
    if (BatteryBar)
    {
        BatteryBar->SetPercent(BatteryPercent / 100.0f);
    }
}

void UFFDHUDWidget::UpdatePowerUsage(int32 UsageLevel)
{
    if (UsageLevelText)
    {
        FString UsageStr = TEXT("Usage: ");
        for (int32 i = 0; i < UsageLevel; ++i)
        {
            UsageStr += TEXT("|");
        }
        UsageLevelText->SetText(FText::FromString(UsageStr));
    }
}
