// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SecurityCamera.generated.h"

class USceneCaptureComponent2D;
class UTextureRenderTarget2D;

/**
 * A security camera placed in the diner. Captures a scene render
 * that is displayed on the camera system UI. Can detect animatronics
 * in its view.
 */
UCLASS()
class FREDBEARSFAMILYDINER_API ASecurityCamera : public AActor
{
    GENERATED_BODY()

public:
    ASecurityCamera();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "Camera")
    void SetCameraActive(bool bActive);

    UFUNCTION(BlueprintPure, Category = "Camera")
    bool IsCameraActive() const { return bIsActive; }

    UFUNCTION(BlueprintPure, Category = "Camera")
    FText GetCameraName() const { return CameraName; }

    UFUNCTION(BlueprintPure, Category = "Camera")
    UTextureRenderTarget2D* GetRenderTarget() const { return RenderTarget; }

    UFUNCTION(BlueprintPure, Category = "Camera")
    FName GetCameraID() const { return CameraID; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USceneCaptureComponent2D> SceneCapture;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera|Config")
    TObjectPtr<UTextureRenderTarget2D> RenderTarget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera|Config")
    FName CameraID;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera|Config")
    FText CameraName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera|Config")
    float CaptureResolution = 512.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|State")
    bool bIsActive = false;
};
