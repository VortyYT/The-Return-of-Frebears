// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "FlickerLight.h"
#include "Components/PointLightComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

AFlickerLight::AFlickerLight()
{
    PrimaryActorTick.bCanEverTick = true;

    LightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
    SetRootComponent(LightComponent);
    LightComponent->SetIntensity(BaseIntensity);
    LightComponent->SetLightColor(LightColor);
    LightComponent->SetAttenuationRadius(AttenuationRadius);
    LightComponent->CastShadows = true;

    BuzzAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("BuzzAudio"));
    BuzzAudioComponent->SetupAttachment(LightComponent);
    BuzzAudioComponent->bAutoActivate = false;
    BuzzAudioComponent->SetVolumeMultiplier(0.15f);
}

void AFlickerLight::BeginPlay()
{
    Super::BeginPlay();

    if (BuzzSound && BuzzAudioComponent)
    {
        BuzzAudioComponent->SetSound(BuzzSound);
        BuzzAudioComponent->Play();
    }

    RandomFlickerTimer = FMath::RandRange(0.0f, RandomFlickerInterval);
}

void AFlickerLight::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsFlickering)
    {
        UpdateFlicker(DeltaTime);
    }
    else if (bRandomFlicker && bLightOn)
    {
        UpdateRandomFlicker(DeltaTime);
    }
}

void AFlickerLight::TriggerFlicker(float Duration)
{
    bIsFlickering = true;
    FlickerDuration = Duration;
    FlickerTimer = 0.0f;
    FlickerPhase = 0.0f;

    if (FlickerSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, FlickerSound, GetActorLocation(), 0.5f);
    }
}

void AFlickerLight::SetLightEnabled(bool bEnabled)
{
    bLightOn = bEnabled;
    bIsFlickering = false;

    if (LightComponent)
    {
        LightComponent->SetVisibility(bEnabled);
        LightComponent->SetIntensity(bEnabled ? BaseIntensity : 0.0f);
    }
}

void AFlickerLight::UpdateFlicker(float DeltaTime)
{
    FlickerTimer += DeltaTime;
    FlickerPhase += DeltaTime * FMath::RandRange(10.0f, 30.0f);

    if (FlickerTimer >= FlickerDuration)
    {
        bIsFlickering = false;
        if (LightComponent && bLightOn)
        {
            LightComponent->SetIntensity(BaseIntensity);
            LightComponent->SetVisibility(true);
        }
        return;
    }

    if (LightComponent)
    {
        // Create erratic flicker pattern
        const float FlickerValue = FMath::Sin(FlickerPhase) *
            FMath::Sin(FlickerPhase * 3.7f) * FMath::Sin(FlickerPhase * 7.3f);
        const float Normalized = (FlickerValue + 1.0f) * 0.5f;

        // Bias toward off for more dramatic effect
        const bool bFlickerOn = Normalized > 0.3f;
        LightComponent->SetVisibility(bFlickerOn);
        LightComponent->SetIntensity(bFlickerOn ?
            BaseIntensity * FMath::RandRange(0.4f, 1.0f) : 0.0f);
    }
}

void AFlickerLight::UpdateRandomFlicker(float DeltaTime)
{
    RandomFlickerTimer += DeltaTime;

    if (RandomFlickerTimer >= RandomFlickerInterval)
    {
        RandomFlickerTimer = 0.0f;

        if (FMath::FRand() < 0.3f)
        {
            TriggerFlicker(FMath::RandRange(0.2f, 1.5f));
        }

        // Randomize next interval
        RandomFlickerInterval = FMath::RandRange(15.0f, 60.0f);
    }
}
