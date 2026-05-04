// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FredbearsFamilyDiner/FredbearsFamilyDiner.h"
#include "AudioManager.generated.h"

class UAudioComponent;

/**
 * Centralized audio manager for ambient horror soundscape.
 * Handles:
 * - Layered ambient audio (base + tension layers)
 * - 3D spatial sound events (footsteps, whispers, metal clangs)
 * - Dynamic music intensity based on threat level
 * - Random sound events for atmosphere
 */
UCLASS()
class FREDBEARSFAMILYDINER_API AAudioManager : public AActor
{
    GENERATED_BODY()

public:
    AAudioManager();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // --- Ambient ---

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetTensionLevel(float Level);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetMusicIntensity(float Intensity);

    // --- Sound Events ---

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayRandomAmbientEvent();

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlaySoundAtLocation(USoundBase* Sound, const FVector& Location,
        float VolumeMultiplier = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayJumpscare(USoundBase* Sound);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void PlayUISound(USoundBase* Sound);

    // --- Volume Control ---

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetMasterVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetMusicVolume(float Volume);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetSFXVolume(float Volume);

protected:
    void UpdateAmbientLayers(float DeltaTime);
    void TriggerRandomEvent(float DeltaTime);

    // --- Audio Components ---

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio|Components")
    TObjectPtr<UAudioComponent> BaseAmbientComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio|Components")
    TObjectPtr<UAudioComponent> TensionLayerComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio|Components")
    TObjectPtr<UAudioComponent> MusicComponent;

    // --- Sounds ---

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Sounds")
    TObjectPtr<USoundBase> BaseAmbientSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Sounds")
    TObjectPtr<USoundBase> TensionAmbientSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Sounds")
    TObjectPtr<USoundBase> LowTensionMusic;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Sounds")
    TObjectPtr<USoundBase> HighTensionMusic;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Random Events")
    TArray<USoundBase*> RandomAmbientSounds;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Random Events")
    TArray<USoundBase*> WhisperSounds;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Random Events")
    TArray<USoundBase*> MetalSounds;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Random Events")
    TArray<USoundBase*> DoorSlamSounds;

    // --- Config ---

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Config")
    float RandomEventIntervalMin = 10.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Config")
    float RandomEventIntervalMax = 45.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio|Config")
    float RandomEventRange = 2000.0f;

    // --- State ---

    float CurrentTensionLevel = 0.0f;
    float CurrentMusicIntensity = 0.0f;
    float RandomEventTimer = 0.0f;
    float NextRandomEventTime = 15.0f;

    float MasterVolumeMultiplier = 1.0f;
    float MusicVolumeMultiplier = 0.8f;
    float SFXVolumeMultiplier = 1.0f;
};
