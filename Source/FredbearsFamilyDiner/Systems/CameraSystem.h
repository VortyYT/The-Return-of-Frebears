// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FredbearsFamilyDiner/FredbearsFamilyDiner.h"
#include "CameraSystem.generated.h"

class ASecurityCamera;
class APowerSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCameraSwitched, int32, OldIndex, int32, NewIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCameraSystemToggled_System, bool, bIsActive);

/**
 * Security camera system allowing the player to monitor the diner.
 * Consumes power while active. Shows static/interference when
 * animatronics are nearby.
 */
UCLASS()
class FREDBEARSFAMILYDINER_API ACameraSystem : public AActor
{
    GENERATED_BODY()

public:
    ACameraSystem();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Camera")
    void ActivateCameras();

    UFUNCTION(BlueprintCallable, Category = "Camera")
    void DeactivateCameras();

    UFUNCTION(BlueprintCallable, Category = "Camera")
    void SwitchToCamera(int32 CameraIndex);

    UFUNCTION(BlueprintCallable, Category = "Camera")
    void NextCamera();

    UFUNCTION(BlueprintCallable, Category = "Camera")
    void PreviousCamera();

    UFUNCTION(BlueprintPure, Category = "Camera")
    bool IsActive() const { return bIsActive; }

    UFUNCTION(BlueprintPure, Category = "Camera")
    int32 GetCurrentCameraIndex() const { return CurrentCameraIndex; }

    UFUNCTION(BlueprintPure, Category = "Camera")
    ASecurityCamera* GetCurrentCamera() const;

    UFUNCTION(BlueprintPure, Category = "Camera")
    int32 GetCameraCount() const { return Cameras.Num(); }

    UFUNCTION(BlueprintPure, Category = "Camera")
    float GetStaticIntensity() const { return CurrentStaticIntensity; }

    UPROPERTY(BlueprintAssignable, Category = "Camera|Events")
    FOnCameraSwitched OnCameraSwitched;

    UPROPERTY(BlueprintAssignable, Category = "Camera|Events")
    FOnCameraSystemToggled_System OnCameraSystemToggled;

protected:
    void UpdateStaticIntensity();
    void RegisterWithPowerSystem();

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Camera|Config")
    TArray<ASecurityCamera*> Cameras;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Camera|Config")
    TObjectPtr<APowerSystem> PowerSystemRef;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera|Config")
    float AnimatronicStaticRange = 500.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|State")
    int32 CurrentCameraIndex = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|State")
    bool bIsActive = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|State")
    float CurrentStaticIntensity = 0.0f;
};
