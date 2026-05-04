// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "FFDFlashlightComponent.h"
#include "Components/SpotLightComponent.h"

UFFDFlashlightComponent::UFFDFlashlightComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
    SpotLight->SetupAttachment(this);
    SpotLight->SetRelativeLocation(FVector(20.0f, 10.0f, -5.0f));
    SpotLight->SetIntensity(LightIntensity);
    SpotLight->SetAttenuationRadius(LightAttenuationRadius);
    SpotLight->SetInnerConeAngle(InnerConeAngle);
    SpotLight->SetOuterConeAngle(OuterConeAngle);
    SpotLight->SetVisibility(false);
    SpotLight->CastShadows = true;

    CurrentBattery = MaxBattery;
}

void UFFDFlashlightComponent::BeginPlay()
{
    Super::BeginPlay();
    CurrentBattery = MaxBattery;
}

void UFFDFlashlightComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bIsOn)
    {
        DrainBattery(DeltaTime);

        if (CurrentBattery <= FlickerThreshold && CurrentBattery > 0.0f)
        {
            UpdateFlicker(DeltaTime);
        }
    }
}

void UFFDFlashlightComponent::ToggleFlashlight()
{
    SetFlashlightOn(!bIsOn);
}

void UFFDFlashlightComponent::SetFlashlightOn(bool bOn)
{
    if (bOn && CurrentBattery <= 0.0f)
    {
        return; // Can't turn on with no battery
    }

    bIsOn = bOn;

    if (SpotLight)
    {
        SpotLight->SetVisibility(bIsOn);
        if (bIsOn)
        {
            SpotLight->SetIntensity(LightIntensity);
        }
    }

    UE_LOG(LogFFD, Verbose, TEXT("Flashlight %s (Battery: %.1f%%)"),
        bIsOn ? TEXT("ON") : TEXT("OFF"), GetBatteryPercent());
}

float UFFDFlashlightComponent::GetBatteryPercent() const
{
    return (MaxBattery > 0.0f) ? (CurrentBattery / MaxBattery) * 100.0f : 0.0f;
}

void UFFDFlashlightComponent::AddBattery(float Amount)
{
    CurrentBattery = FMath::Clamp(CurrentBattery + Amount, 0.0f, MaxBattery);
    OnBatteryChanged.Broadcast(CurrentBattery);
}

void UFFDFlashlightComponent::DrainBattery(float DeltaTime)
{
    const float PrevBattery = CurrentBattery;
    CurrentBattery = FMath::Max(CurrentBattery - (DrainRate * DeltaTime), 0.0f);

    if (CurrentBattery != PrevBattery)
    {
        OnBatteryChanged.Broadcast(CurrentBattery);
    }

    if (CurrentBattery <= 0.0f)
    {
        SetFlashlightOn(false);
        OnBatteryDepleted.Broadcast();
        UE_LOG(LogFFD, Log, TEXT("Flashlight battery depleted"));
    }
}

void UFFDFlashlightComponent::UpdateFlicker(float DeltaTime)
{
    FlickerTimer += DeltaTime;

    if (FlickerTimer >= NextFlickerTime)
    {
        FlickerTimer = 0.0f;

        // Flicker more frequently as battery gets lower
        const float BatteryRatio = CurrentBattery / FlickerThreshold;
        NextFlickerTime = FMath::RandRange(0.05f, 0.3f * BatteryRatio + 0.05f);

        if (SpotLight)
        {
            const bool bFlickerOff = FMath::FRand() < (1.0f - BatteryRatio) * 0.4f;
            SpotLight->SetIntensity(bFlickerOff ? 0.0f :
                LightIntensity * FMath::RandRange(0.5f, 1.0f));
        }
    }
}
