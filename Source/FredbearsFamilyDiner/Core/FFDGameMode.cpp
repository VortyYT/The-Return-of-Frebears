// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "FFDGameMode.h"
#include "FFDGameState.h"
#include "FredbearsFamilyDiner/Player/FFDPlayerCharacter.h"
#include "FredbearsFamilyDiner/Animatronics/AnimatronicBase.h"
#include "Kismet/GameplayStatics.h"

AFFDGameMode::AFFDGameMode()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickGroup = TG_PrePhysics;

    DefaultPawnClass = AFFDPlayerCharacter::StaticClass();
    GameStateClass = AFFDGameState::StaticClass();
}

void AFFDGameMode::BeginPlay()
{
    Super::BeginPlay();

    PlayerCharacterRef = Cast<AFFDPlayerCharacter>(
        UGameplayStatics::GetPlayerCharacter(this, 0));

    InitializeRandomSeed();

    UE_LOG(LogFFD, Log, TEXT("GameMode initialized. Random seed: %d"), RandomSeed);
}

void AFFDGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bNightActive && CurrentPhase == EGamePhase::Playing)
    {
        UpdateNightTime(DeltaTime);
        CheckNightCompletion();
    }
}

void AFFDGameMode::StartNight(EDifficulty InDifficulty, int32 NightNumber)
{
    CurrentDifficulty = InDifficulty;
    CurrentNight = NightNumber;
    CurrentHour = FFDConstants::NightStartHour;
    NightElapsedTime = 0.0f;
    bNightActive = true;
    CurrentPhase = EGamePhase::Playing;

    ActivateAnimatronics();

    OnNightStarted.Broadcast();
    OnHourChanged.Broadcast(CurrentHour);

    UE_LOG(LogFFD, Log, TEXT("Night %d started. Difficulty: %d"), CurrentNight,
        static_cast<int32>(CurrentDifficulty));
}

void AFFDGameMode::EndNight(bool bPlayerSurvived)
{
    bNightActive = false;
    DeactivateAnimatronics();

    if (bPlayerSurvived)
    {
        CurrentPhase = EGamePhase::Victory;
        OnNightCompleted.Broadcast();
        UE_LOG(LogFFD, Log, TEXT("Night %d completed! Player survived."), CurrentNight);
    }
    else
    {
        CurrentPhase = EGamePhase::GameOver;
        UE_LOG(LogFFD, Log, TEXT("Night %d failed. Player died."), CurrentNight);
    }
}

float AFFDGameMode::GetNightProgress() const
{
    if (NightDuration <= 0.0f) return 0.0f;
    return FMath::Clamp(NightElapsedTime / NightDuration, 0.0f, 1.0f);
}

float AFFDGameMode::GetDifficultyMultiplier() const
{
    switch (CurrentDifficulty)
    {
    case EDifficulty::Easy:      return FFDConstants::EasyMultiplier;
    case EDifficulty::Normal:    return FFDConstants::NormalMultiplier;
    case EDifficulty::Hard:      return FFDConstants::HardMultiplier;
    case EDifficulty::Nightmare: return FFDConstants::NightmareMultiplier;
    default:                     return FFDConstants::NormalMultiplier;
    }
}

void AFFDGameMode::TriggerPlayerDeath(AAnimatronicBase* Killer)
{
    if (!bNightActive) return;

    UE_LOG(LogFFD, Warning, TEXT("Player killed by: %s"),
        Killer ? *Killer->GetName() : TEXT("Unknown"));

    OnPlayerDeath.Broadcast();
    EndNight(false);
}

void AFFDGameMode::SetGamePaused(bool bPaused)
{
    UGameplayStatics::SetGamePaused(this, bPaused);
    CurrentPhase = bPaused ? EGamePhase::Paused : EGamePhase::Playing;
}

void AFFDGameMode::UpdateNightTime(float DeltaTime)
{
    NightElapsedTime += DeltaTime;

    // Calculate the new hour based on elapsed time
    const int32 HoursElapsed = FMath::FloorToInt(
        NightElapsedTime / FFDConstants::SecondsPerInGameHour);
    const int32 NewHour = (FFDConstants::NightStartHour + HoursElapsed) % 12;
    const int32 DisplayHour = (NewHour == 0) ? 12 : NewHour;

    if (DisplayHour != CurrentHour)
    {
        CurrentHour = DisplayHour;
        OnHourChanged.Broadcast(CurrentHour);
        UE_LOG(LogFFD, Log, TEXT("Hour changed to: %d AM"), CurrentHour);
    }
}

void AFFDGameMode::CheckNightCompletion()
{
    if (GetNightProgress() >= 1.0f)
    {
        CurrentHour = FFDConstants::NightEndHour;
        OnHourChanged.Broadcast(CurrentHour);
        EndNight(true);
    }
}

void AFFDGameMode::InitializeRandomSeed()
{
    RandomSeed = FMath::Rand();
    FMath::SRandInit(RandomSeed);
    UE_LOG(LogFFD, Log, TEXT("Random seed initialized: %d"), RandomSeed);
}

void AFFDGameMode::ActivateAnimatronics()
{
    const float DiffMult = GetDifficultyMultiplier();

    for (AAnimatronicBase* Animatronic : Animatronics)
    {
        if (Animatronic)
        {
            Animatronic->ActivateAnimatronic(DiffMult, CurrentNight);
        }
    }

    UE_LOG(LogFFDAI, Log, TEXT("Activated %d animatronics with difficulty multiplier %.2f"),
        Animatronics.Num(), DiffMult);
}

void AFFDGameMode::DeactivateAnimatronics()
{
    for (AAnimatronicBase* Animatronic : Animatronics)
    {
        if (Animatronic)
        {
            Animatronic->DeactivateAnimatronic();
        }
    }
}

void AFFDGameMode::RegisterAnimatronic(AAnimatronicBase* Animatronic)
{
    if (Animatronic && !Animatronics.Contains(Animatronic))
    {
        Animatronics.Add(Animatronic);
        UE_LOG(LogFFDAI, Log, TEXT("Registered animatronic: %s"), *Animatronic->GetName());
    }
}
