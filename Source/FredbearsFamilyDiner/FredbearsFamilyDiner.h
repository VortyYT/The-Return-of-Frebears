// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"

// Log categories for the game
DECLARE_LOG_CATEGORY_EXTERN(LogFFD, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogFFDAI, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogFFDPower, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogFFDAudio, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogFFDSave, Log, All);

// Game-wide constants
namespace FFDConstants
{
    // Time system
    constexpr float NightDurationSeconds = 480.0f;       // 8 real minutes = one in-game night
    constexpr int32 NightStartHour = 12;                 // 12 AM
    constexpr int32 NightEndHour = 6;                    // 6 AM
    constexpr float SecondsPerInGameHour = NightDurationSeconds / 6.0f;

    // Power system
    constexpr float MaxPower = 100.0f;
    constexpr float BasePowerDrainPerSecond = 0.08f;
    constexpr float FlashlightDrainPerSecond = 0.15f;
    constexpr float CameraDrainPerSecond = 0.12f;
    constexpr float DoorDrainPerSecond = 0.10f;

    // Player
    constexpr float DefaultWalkSpeed = 300.0f;
    constexpr float DefaultSprintSpeed = 550.0f;
    constexpr float DefaultCrouchSpeed = 150.0f;
    constexpr float MaxStamina = 100.0f;
    constexpr float StaminaDrainPerSecond = 20.0f;
    constexpr float StaminaRegenPerSecond = 10.0f;
    constexpr float FlashlightMaxBattery = 100.0f;
    constexpr float FlashlightDrainRate = 5.0f;

    // AI
    constexpr float AIBaseDetectionRange = 1500.0f;
    constexpr float AIHearingRange = 2000.0f;
    constexpr float AIAggroTimeout = 15.0f;
    constexpr float AIPatrolWaitMin = 3.0f;
    constexpr float AIPatrolWaitMax = 10.0f;

    // Difficulty multipliers
    constexpr float EasyMultiplier = 0.7f;
    constexpr float NormalMultiplier = 1.0f;
    constexpr float HardMultiplier = 1.4f;
    constexpr float NightmareMultiplier = 2.0f;
}

// Enums used across the project
UENUM(BlueprintType)
enum class EAnimatronicState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Patrolling  UMETA(DisplayName = "Patrolling"),
    Hunting     UMETA(DisplayName = "Hunting"),
    Stalking    UMETA(DisplayName = "Stalking"),
    Aggressive  UMETA(DisplayName = "Aggressive"),
    Retreating  UMETA(DisplayName = "Retreating"),
    Deactivated UMETA(DisplayName = "Deactivated")
};

UENUM(BlueprintType)
enum class EDifficulty : uint8
{
    Easy      UMETA(DisplayName = "Easy"),
    Normal    UMETA(DisplayName = "Normal"),
    Hard      UMETA(DisplayName = "Hard"),
    Nightmare UMETA(DisplayName = "Nightmare")
};

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
    MainMenu    UMETA(DisplayName = "Main Menu"),
    NightIntro  UMETA(DisplayName = "Night Intro"),
    Playing     UMETA(DisplayName = "Playing"),
    GameOver    UMETA(DisplayName = "Game Over"),
    Victory     UMETA(DisplayName = "Victory"),
    Paused      UMETA(DisplayName = "Paused")
};
