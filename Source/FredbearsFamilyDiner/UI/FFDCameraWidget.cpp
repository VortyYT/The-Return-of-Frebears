// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "FFDCameraWidget.h"
#include "FredbearsFamilyDiner/Systems/CameraSystem.h"
#include "FredbearsFamilyDiner/Environment/SecurityCamera.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/TextureRenderTarget2D.h"

void UFFDCameraWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (NextCameraButton) NextCameraButton->OnClicked.AddDynamic(
        this, &UFFDCameraWidget::OnNextCameraClicked);
    if (PrevCameraButton) PrevCameraButton->OnClicked.AddDynamic(
        this, &UFFDCameraWidget::OnPrevCameraClicked);

    // Create static material instance
    if (StaticMaterial)
    {
        StaticMaterialInstance = UMaterialInstanceDynamic::Create(StaticMaterial, this);
        if (StaticOverlay)
        {
            StaticOverlay->SetBrushFromMaterial(StaticMaterialInstance);
        }
    }

    SetVisibility(ESlateVisibility::Collapsed);
}

void UFFDCameraWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!CameraSystemRef || !CameraSystemRef->IsActive()) return;

    UpdateCameraFeed();
    UpdateStaticEffect(CameraSystemRef->GetStaticIntensity());

    // Blink recording indicator
    RecordingBlinkTimer += InDeltaTime;
    if (RecordingIndicator)
    {
        const bool bShow = FMath::Fmod(RecordingBlinkTimer, 1.0f) < 0.7f;
        RecordingIndicator->SetVisibility(bShow ?
            ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
}

void UFFDCameraWidget::SetCameraSystem(ACameraSystem* InCameraSystem)
{
    CameraSystemRef = InCameraSystem;
}

void UFFDCameraWidget::ShowCameraView()
{
    SetVisibility(ESlateVisibility::Visible);
    RecordingBlinkTimer = 0.0f;
}

void UFFDCameraWidget::HideCameraView()
{
    SetVisibility(ESlateVisibility::Collapsed);
}

void UFFDCameraWidget::OnNextCameraClicked()
{
    if (CameraSystemRef)
    {
        CameraSystemRef->NextCamera();
    }
}

void UFFDCameraWidget::OnPrevCameraClicked()
{
    if (CameraSystemRef)
    {
        CameraSystemRef->PreviousCamera();
    }
}

void UFFDCameraWidget::UpdateStaticEffect(float StaticIntensity)
{
    if (StaticMaterialInstance)
    {
        StaticMaterialInstance->SetScalarParameterValue(
            FName(TEXT("StaticIntensity")), StaticIntensity);
        StaticMaterialInstance->SetScalarParameterValue(
            FName(TEXT("Time")), GetWorld()->GetTimeSeconds());
    }

    if (StaticOverlay)
    {
        StaticOverlay->SetRenderOpacity(StaticIntensity * 0.8f);
    }
}

void UFFDCameraWidget::UpdateCameraFeed()
{
    if (!CameraSystemRef) return;

    ASecurityCamera* Camera = CameraSystemRef->GetCurrentCamera();
    if (!Camera) return;

    // Update camera name
    if (CameraNameText)
    {
        CameraNameText->SetText(FText::Format(
            NSLOCTEXT("Camera", "CamName", "CAM {0} - {1}"),
            FText::AsNumber(CameraSystemRef->GetCurrentCameraIndex() + 1),
            Camera->GetCameraName()));
    }

    // Update camera feed texture
    if (CameraFeedImage && Camera->GetRenderTarget())
    {
        CameraFeedImage->SetBrushFromMaterial(nullptr);
        // Note: In a full implementation, create a dynamic material with the
        // render target as a texture parameter
    }
}
