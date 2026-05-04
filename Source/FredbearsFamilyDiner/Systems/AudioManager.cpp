// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "AudioManager.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

AAudioManager::AAudioManager()
{
    PrimaryActorTick.bCanEverTick = true;

    BaseAmbientComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("BaseAmbient"));
    BaseAmbientComponent->SetupAttachment(GetRootComponent());
    BaseAmbientComponent->bAutoActivate = false;
    BaseAmbientComponent->bIsUISound = true;

    TensionLayerComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("TensionLayer"));
    TensionLayerComponent->SetupAttachment(GetRootComponent());
    TensionLayerComponent->bAutoActivate = false;
    TensionLayerComponent->bIsUISound = true;

    MusicComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Music"));
    MusicComponent->SetupAttachment(GetRootComponent());
    MusicComponent->bAutoActivate = false;
    MusicComponent->bIsUISound = true;
}

void AAudioManager::BeginPlay()
{
    Super::BeginPlay();

    // Start base ambient sound
    if (BaseAmbientSound && BaseAmbientComponent)
    {
        BaseAmbientComponent->SetSound(BaseAmbientSound);
        BaseAmbientComponent->Play();
        BaseAmbientComponent->SetVolumeMultiplier(0.5f * MasterVolumeMultiplier);
    }

    NextRandomEventTime = FMath::RandRange(RandomEventIntervalMin, RandomEventIntervalMax);
}

void AAudioManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateAmbientLayers(DeltaTime);
    TriggerRandomEvent(DeltaTime);
}

void AAudioManager::SetTensionLevel(float Level)
{
    CurrentTensionLevel = FMath::Clamp(Level, 0.0f, 1.0f);
}

void AAudioManager::SetMusicIntensity(float Intensity)
{
    CurrentMusicIntensity = FMath::Clamp(Intensity, 0.0f, 1.0f);
}

void AAudioManager::PlayRandomAmbientEvent()
{
    if (RandomAmbientSounds.Num() == 0) return;

    // Pick a random sound
    const int32 SoundIndex = FMath::RandRange(0, RandomAmbientSounds.Num() - 1);
    USoundBase* Sound = RandomAmbientSounds[SoundIndex];
    if (!Sound) return;

    // Play at a random location near the player
    APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
    if (!Player) return;

    const FVector PlayerLoc = Player->GetActorLocation();
    const FVector RandomOffset = FMath::VRand() * FMath::RandRange(200.0f, RandomEventRange);
    const FVector SoundLocation = PlayerLoc + RandomOffset;

    PlaySoundAtLocation(Sound, SoundLocation, SFXVolumeMultiplier);

    UE_LOG(LogFFDAudio, Verbose, TEXT("Random ambient event triggered"));
}

void AAudioManager::PlaySoundAtLocation(USoundBase* Sound, const FVector& Location,
    float VolumeMultiplier)
{
    if (!Sound) return;

    UGameplayStatics::PlaySoundAtLocation(
        this, Sound, Location,
        VolumeMultiplier * MasterVolumeMultiplier * SFXVolumeMultiplier);
}

void AAudioManager::PlayJumpscare(USoundBase* Sound)
{
    if (!Sound) return;

    UGameplayStatics::PlaySound2D(this, Sound,
        MasterVolumeMultiplier * SFXVolumeMultiplier * 1.5f);
}

void AAudioManager::PlayUISound(USoundBase* Sound)
{
    if (!Sound) return;

    UGameplayStatics::PlaySound2D(this, Sound,
        MasterVolumeMultiplier * SFXVolumeMultiplier * 0.8f);
}

void AAudioManager::SetMasterVolume(float Volume)
{
    MasterVolumeMultiplier = FMath::Clamp(Volume, 0.0f, 1.0f);
}

void AAudioManager::SetMusicVolume(float Volume)
{
    MusicVolumeMultiplier = FMath::Clamp(Volume, 0.0f, 1.0f);
}

void AAudioManager::SetSFXVolume(float Volume)
{
    SFXVolumeMultiplier = FMath::Clamp(Volume, 0.0f, 1.0f);
}

void AAudioManager::UpdateAmbientLayers(float DeltaTime)
{
    // Tension layer fades in/out based on tension level
    if (TensionLayerComponent)
    {
        if (CurrentTensionLevel > 0.1f && !TensionLayerComponent->IsPlaying()
            && TensionAmbientSound)
        {
            TensionLayerComponent->SetSound(TensionAmbientSound);
            TensionLayerComponent->Play();
        }

        const float TargetVolume = CurrentTensionLevel * MasterVolumeMultiplier *
            MusicVolumeMultiplier;
        const float CurrentVolume = TensionLayerComponent->VolumeMultiplier;
        TensionLayerComponent->SetVolumeMultiplier(
            FMath::FInterpTo(CurrentVolume, TargetVolume, DeltaTime, 2.0f));

        if (CurrentTensionLevel <= 0.05f && TensionLayerComponent->IsPlaying())
        {
            TensionLayerComponent->FadeOut(1.0f, 0.0f);
        }
    }

    // Music crossfade between low and high tension
    if (MusicComponent)
    {
        USoundBase* TargetMusic = (CurrentMusicIntensity > 0.5f)
            ? HighTensionMusic : LowTensionMusic;

        if (TargetMusic && MusicComponent->Sound != TargetMusic)
        {
            MusicComponent->FadeOut(2.0f, 0.0f);
            MusicComponent->SetSound(TargetMusic);
            MusicComponent->FadeIn(2.0f, MusicVolumeMultiplier * MasterVolumeMultiplier);
        }

        MusicComponent->SetVolumeMultiplier(
            MusicVolumeMultiplier * MasterVolumeMultiplier *
            FMath::Max(0.3f, CurrentMusicIntensity));
    }
}

void AAudioManager::TriggerRandomEvent(float DeltaTime)
{
    RandomEventTimer += DeltaTime;

    if (RandomEventTimer >= NextRandomEventTime)
    {
        RandomEventTimer = 0.0f;

        // Higher tension = more frequent events
        const float FrequencyMult = 1.0f - (CurrentTensionLevel * 0.5f);
        NextRandomEventTime = FMath::RandRange(
            RandomEventIntervalMin * FrequencyMult,
            RandomEventIntervalMax * FrequencyMult);

        // Choose event type
        const float Roll = FMath::FRand();
        APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
        if (!Player) return;

        const FVector PlayerLoc = Player->GetActorLocation();

        if (Roll < 0.3f && WhisperSounds.Num() > 0)
        {
            // Whisper near the player
            const int32 Idx = FMath::RandRange(0, WhisperSounds.Num() - 1);
            if (WhisperSounds[Idx])
            {
                const FVector Offset = FMath::VRand() * FMath::RandRange(100.0f, 400.0f);
                PlaySoundAtLocation(WhisperSounds[Idx], PlayerLoc + Offset, 0.4f);
            }
        }
        else if (Roll < 0.6f && MetalSounds.Num() > 0)
        {
            // Metal clang in the distance
            const int32 Idx = FMath::RandRange(0, MetalSounds.Num() - 1);
            if (MetalSounds[Idx])
            {
                const FVector Offset = FMath::VRand() * FMath::RandRange(500.0f, 1500.0f);
                PlaySoundAtLocation(MetalSounds[Idx], PlayerLoc + Offset, 0.6f);
            }
        }
        else if (Roll < 0.8f && DoorSlamSounds.Num() > 0)
        {
            // Door slam
            const int32 Idx = FMath::RandRange(0, DoorSlamSounds.Num() - 1);
            if (DoorSlamSounds[Idx])
            {
                const FVector Offset = FMath::VRand() * FMath::RandRange(300.0f, 1000.0f);
                PlaySoundAtLocation(DoorSlamSounds[Idx], PlayerLoc + Offset, 0.7f);
            }
        }
        else
        {
            PlayRandomAmbientEvent();
        }
    }
}
