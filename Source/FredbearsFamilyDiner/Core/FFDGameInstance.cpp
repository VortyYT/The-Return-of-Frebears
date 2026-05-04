// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "FFDGameInstance.h"
#include "FFDSaveGame.h"
#include "Kismet/GameplayStatics.h"

const FString UFFDGameInstance::SaveSlotName = TEXT("FredbearsSave");
const int32 UFFDGameInstance::SaveUserIndex = 0;

UFFDGameInstance::UFFDGameInstance()
{
}

void UFFDGameInstance::Init()
{
    Super::Init();

    // Attempt to load existing save data
    if (!LoadGame())
    {
        // Create fresh save data if none exists
        CurrentSaveData = Cast<UFFDSaveGame>(
            UGameplayStatics::CreateSaveGameObject(UFFDSaveGame::StaticClass()));
        UE_LOG(LogFFDSave, Log, TEXT("Created new save data"));
    }
}

bool UFFDGameInstance::SaveGame()
{
    if (!CurrentSaveData)
    {
        UE_LOG(LogFFDSave, Error, TEXT("Cannot save: SaveData is null"));
        return false;
    }

    const bool bSuccess = UGameplayStatics::SaveGameToSlot(
        CurrentSaveData, SaveSlotName, SaveUserIndex);

    UE_LOG(LogFFDSave, Log, TEXT("Save game %s"), bSuccess ? TEXT("succeeded") : TEXT("FAILED"));
    return bSuccess;
}

bool UFFDGameInstance::LoadGame()
{
    if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex))
    {
        return false;
    }

    CurrentSaveData = Cast<UFFDSaveGame>(
        UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex));

    if (CurrentSaveData)
    {
        // Restore settings from save data
        MasterVolume = CurrentSaveData->MasterVolume;
        MusicVolume = CurrentSaveData->MusicVolume;
        SFXVolume = CurrentSaveData->SFXVolume;
        MouseSensitivity = CurrentSaveData->MouseSensitivity;

        UE_LOG(LogFFDSave, Log, TEXT("Loaded save data. Highest night: %d"),
            CurrentSaveData->HighestNightCompleted);
        return true;
    }

    UE_LOG(LogFFDSave, Error, TEXT("Failed to cast loaded save data"));
    return false;
}

bool UFFDGameInstance::DeleteSave()
{
    const bool bDeleted = UGameplayStatics::DeleteGameInSlot(SaveSlotName, SaveUserIndex);

    if (bDeleted)
    {
        CurrentSaveData = Cast<UFFDSaveGame>(
            UGameplayStatics::CreateSaveGameObject(UFFDSaveGame::StaticClass()));
        UE_LOG(LogFFDSave, Log, TEXT("Save data deleted and reset"));
    }

    return bDeleted;
}

bool UFFDGameInstance::DoesSaveExist() const
{
    return UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveUserIndex);
}

void UFFDGameInstance::SetMasterVolume(float Volume)
{
    MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
    if (CurrentSaveData) CurrentSaveData->MasterVolume = MasterVolume;
}

void UFFDGameInstance::SetMusicVolume(float Volume)
{
    MusicVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
    if (CurrentSaveData) CurrentSaveData->MusicVolume = MusicVolume;
}

void UFFDGameInstance::SetSFXVolume(float Volume)
{
    SFXVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
    if (CurrentSaveData) CurrentSaveData->SFXVolume = SFXVolume;
}

void UFFDGameInstance::SetMouseSensitivity(float Sensitivity)
{
    MouseSensitivity = FMath::Clamp(Sensitivity, 0.1f, 5.0f);
    if (CurrentSaveData) CurrentSaveData->MouseSensitivity = MouseSensitivity;
}

int32 UFFDGameInstance::GetHighestNightCompleted() const
{
    return CurrentSaveData ? CurrentSaveData->HighestNightCompleted : 0;
}

bool UFFDGameInstance::IsExtrasUnlocked() const
{
    // Extras unlocked after completing night 5
    return GetHighestNightCompleted() >= 5;
}

void UFFDGameInstance::UnlockNight(int32 NightNumber)
{
    if (CurrentSaveData && NightNumber > CurrentSaveData->HighestNightCompleted)
    {
        CurrentSaveData->HighestNightCompleted = NightNumber;
        SaveGame();
        UE_LOG(LogFFDSave, Log, TEXT("Unlocked night %d"), NightNumber);
    }
}
