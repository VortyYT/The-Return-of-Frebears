// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlickerLight.generated.h"

class UPointLightComponent;
class UAudioComponent;

/**
 * A light fixture that can flicker randomly or on demand.
 * Used for atmospheric horror and random event integration.
 */
UCLASS()
class FREDBEARSFAMILYDINER_API AFlickerLight : public AActor
{
    GENERATED_BODY()

public:
    AFlickerLight();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    /** Trigger a flicker for the specified duration */
    UFUNCTION(BlueprintCallable, Category = "Light")
    void TriggerFlicker(float Duration);

    /** Permanently turn the light on or off */
    UFUNCTION(BlueprintCallable, Category = "Light")
    void SetLightEnabled(bool bEnabled);

    UFUNCTION(BlueprintPure, Category = "Light")
    bool IsFlickering() const { return bIsFlickering; }

    UFUNCTION(BlueprintPure, Category = "Light")
    bool IsLightOn() const { return bLightOn; }

protected:
    void UpdateFlicker(float DeltaTime);
    void UpdateRandomFlicker(float DeltaTime);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UPointLightComponent> LightComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UAudioComponent> BuzzAudioComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Light|Config")
    float BaseIntensity = 5000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Light|Config")
    FLinearColor LightColor = FLinearColor(1.0f, 0.95f, 0.8f);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Light|Config")
    float AttenuationRadius = 800.0f;

    /** Whether this light randomly flickers on its own */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Light|Config")
    bool bRandomFlicker = true;

    /** Average time between random flickers */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Light|Config")
    float RandomFlickerInterval = 30.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Light|Audio")
    TObjectPtr<USoundBase> FlickerSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Light|Audio")
    TObjectPtr<USoundBase> BuzzSound;

    // --- State ---

    bool bLightOn = true;
    bool bIsFlickering = false;
    float FlickerDuration = 0.0f;
    float FlickerTimer = 0.0f;
    float FlickerPhase = 0.0f;
    float RandomFlickerTimer = 0.0f;
};
