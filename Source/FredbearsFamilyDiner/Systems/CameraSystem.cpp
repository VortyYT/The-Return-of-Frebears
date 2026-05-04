// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "CameraSystem.h"
#include "FredbearsFamilyDiner/Environment/SecurityCamera.h"
#include "FredbearsFamilyDiner/Animatronics/AnimatronicBase.h"
#include "FredbearsFamilyDiner/Core/FFDGameMode.h"
#include "PowerSystem.h"
#include "Kismet/GameplayStatics.h"

ACameraSystem::ACameraSystem()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ACameraSystem::BeginPlay()
{
    Super::BeginPlay();
    RegisterWithPowerSystem();
}

void ACameraSystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsActive)
    {
        UpdateStaticIntensity();
    }
}

void ACameraSystem::ActivateCameras()
{
    bIsActive = true;
    OnCameraSystemToggled.Broadcast(true);

    if (PowerSystemRef)
    {
        PowerSystemRef->SetConsumerActive(FName(TEXT("CameraSystem")), true);
    }

    // Activate the current camera
    if (ASecurityCamera* Camera = GetCurrentCamera())
    {
        Camera->SetCameraActive(true);
    }

    UE_LOG(LogFFD, Log, TEXT("Camera system activated"));
}

void ACameraSystem::DeactivateCameras()
{
    // Deactivate current camera
    if (ASecurityCamera* Camera = GetCurrentCamera())
    {
        Camera->SetCameraActive(false);
    }

    bIsActive = false;
    OnCameraSystemToggled.Broadcast(false);

    if (PowerSystemRef)
    {
        PowerSystemRef->SetConsumerActive(FName(TEXT("CameraSystem")), false);
    }
}

void ACameraSystem::SwitchToCamera(int32 CameraIndex)
{
    if (CameraIndex < 0 || CameraIndex >= Cameras.Num()) return;
    if (CameraIndex == CurrentCameraIndex) return;

    // Deactivate old camera
    if (ASecurityCamera* OldCamera = GetCurrentCamera())
    {
        OldCamera->SetCameraActive(false);
    }

    const int32 OldIndex = CurrentCameraIndex;
    CurrentCameraIndex = CameraIndex;

    // Activate new camera
    if (ASecurityCamera* NewCamera = GetCurrentCamera())
    {
        NewCamera->SetCameraActive(true);
    }

    OnCameraSwitched.Broadcast(OldIndex, CurrentCameraIndex);

    // Increment camera check stat
    if (AFFDGameState* GameState = Cast<AFFDGameState>(
        UGameplayStatics::GetGameState(this)))
    {
        GameState->CameraChecks++;
    }
}

void ACameraSystem::NextCamera()
{
    const int32 NextIndex = (CurrentCameraIndex + 1) % FMath::Max(Cameras.Num(), 1);
    SwitchToCamera(NextIndex);
}

void ACameraSystem::PreviousCamera()
{
    const int32 PrevIndex = (CurrentCameraIndex - 1 + Cameras.Num()) %
        FMath::Max(Cameras.Num(), 1);
    SwitchToCamera(PrevIndex);
}

ASecurityCamera* ACameraSystem::GetCurrentCamera() const
{
    if (Cameras.IsValidIndex(CurrentCameraIndex))
    {
        return Cameras[CurrentCameraIndex];
    }
    return nullptr;
}

void ACameraSystem::UpdateStaticIntensity()
{
    ASecurityCamera* Camera = GetCurrentCamera();
    if (!Camera) return;

    float MaxStatic = 0.0f;

    // Check proximity of animatronics to current camera
    AFFDGameMode* GameMode = Cast<AFFDGameMode>(
        UGameplayStatics::GetGameMode(this));
    if (!GameMode) return;

    for (const AAnimatronicBase* Animatronic : GameMode->GetAnimatronics())
    {
        if (!Animatronic || !Animatronic->IsActive()) continue;

        const float Distance = FVector::Dist(
            Camera->GetActorLocation(), Animatronic->GetActorLocation());

        if (Distance < AnimatronicStaticRange)
        {
            const float StaticAmount = 1.0f - (Distance / AnimatronicStaticRange);
            MaxStatic = FMath::Max(MaxStatic, StaticAmount);
        }
    }

    // Add random static noise
    MaxStatic += FMath::FRand() * 0.05f;
    CurrentStaticIntensity = FMath::Clamp(MaxStatic, 0.0f, 1.0f);
}

void ACameraSystem::RegisterWithPowerSystem()
{
    if (PowerSystemRef)
    {
        PowerSystemRef->AddPowerConsumer(
            FName(TEXT("CameraSystem")),
            FFDConstants::CameraDrainPerSecond);
    }
}
