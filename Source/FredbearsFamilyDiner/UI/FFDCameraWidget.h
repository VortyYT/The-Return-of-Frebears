// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FFDCameraWidget.generated.h"

class UImage;
class UTextBlock;
class UButton;
class UMaterialInstanceDynamic;
class ACameraSystem;

/**
 * Camera view overlay showing security feed with static effect,
 * camera name, and navigation buttons. Displays when the player
 * toggles the camera system.
 */
UCLASS()
class FREDBEARSFAMILYDINER_API UFFDCameraWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Camera UI")
    void SetCameraSystem(ACameraSystem* InCameraSystem);

    UFUNCTION(BlueprintCallable, Category = "Camera UI")
    void ShowCameraView();

    UFUNCTION(BlueprintCallable, Category = "Camera UI")
    void HideCameraView();

protected:
    UFUNCTION()
    void OnNextCameraClicked();

    UFUNCTION()
    void OnPrevCameraClicked();

    void UpdateStaticEffect(float StaticIntensity);
    void UpdateCameraFeed();

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> CameraFeedImage;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> StaticOverlay;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> CameraNameText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> RecordingIndicator;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> NextCameraButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> PrevCameraButton;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera UI|Config")
    TObjectPtr<UMaterialInterface> StaticMaterial;

    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> StaticMaterialInstance;

    UPROPERTY()
    TObjectPtr<ACameraSystem> CameraSystemRef;

    float RecordingBlinkTimer = 0.0f;
};
