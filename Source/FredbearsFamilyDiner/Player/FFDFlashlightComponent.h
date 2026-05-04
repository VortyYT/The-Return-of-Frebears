// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FredbearsFamilyDiner/FredbearsFamilyDiner.h"
#include "FFDFlashlightComponent.generated.h"

class USpotLightComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBatteryChanged, float, NewBatteryLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBatteryDepleted);

/**
 * Flashlight with battery drain, flickering at low battery,
 * and automatic shutoff when depleted.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FREDBEARSFAMILYDINER_API UFFDFlashlightComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UFFDFlashlightComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Flashlight")
    void ToggleFlashlight();

    UFUNCTION(BlueprintCallable, Category = "Flashlight")
    void SetFlashlightOn(bool bOn);

    UFUNCTION(BlueprintPure, Category = "Flashlight")
    bool IsFlashlightOn() const { return bIsOn; }

    UFUNCTION(BlueprintPure, Category = "Flashlight")
    float GetBatteryLevel() const { return CurrentBattery; }

    UFUNCTION(BlueprintPure, Category = "Flashlight")
    float GetBatteryPercent() const;

    UFUNCTION(BlueprintCallable, Category = "Flashlight")
    void AddBattery(float Amount);

    UPROPERTY(BlueprintAssignable, Category = "Flashlight|Events")
    FOnBatteryChanged OnBatteryChanged;

    UPROPERTY(BlueprintAssignable, Category = "Flashlight|Events")
    FOnBatteryDepleted OnBatteryDepleted;

protected:
    void DrainBattery(float DeltaTime);
    void UpdateFlicker(float DeltaTime);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flashlight")
    TObjectPtr<USpotLightComponent> SpotLight;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flashlight|Config")
    float MaxBattery = FFDConstants::FlashlightMaxBattery;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flashlight|Config")
    float DrainRate = FFDConstants::FlashlightDrainRate;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flashlight|Config")
    float FlickerThreshold = 20.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flashlight|Config")
    float LightIntensity = 8000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flashlight|Config")
    float LightAttenuationRadius = 2000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flashlight|Config")
    float InnerConeAngle = 15.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Flashlight|Config")
    float OuterConeAngle = 35.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flashlight|State")
    float CurrentBattery;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flashlight|State")
    bool bIsOn = false;

    float FlickerTimer = 0.0f;
    float NextFlickerTime = 0.0f;
};
