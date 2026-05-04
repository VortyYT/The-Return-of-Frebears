// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "InteractableDoor.h"
#include "FredbearsFamilyDiner/Systems/PowerSystem.h"
#include "FredbearsFamilyDiner/Core/FFDGameState.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

AInteractableDoor::AInteractableDoor()
{
    PrimaryActorTick.bCanEverTick = true;

    // Door frame (static)
    FrameMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrameMesh"));
    SetRootComponent(FrameMesh);

    // Door panel (animates)
    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    DoorMesh->SetupAttachment(FrameMesh);

    // Audio
    DoorAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("DoorAudio"));
    DoorAudioComponent->SetupAttachment(FrameMesh);
    DoorAudioComponent->bAutoActivate = false;

    PowerConsumerID = FName(TEXT("Door_Default"));
}

void AInteractableDoor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateDoorAnimation(DeltaTime);
}

void AInteractableDoor::OnInteract_Implementation(AActor* Interactor)
{
    ToggleDoor();

    // Update door usage stat
    if (AFFDGameState* GameState = Cast<AFFDGameState>(
        UGameplayStatics::GetGameState(this)))
    {
        GameState->DoorsUsed++;
    }
}

FText AInteractableDoor::GetInteractionPrompt_Implementation() const
{
    switch (CurrentState)
    {
    case EDoorState::Open:
    case EDoorState::Opening:
        return FText::FromString(TEXT("Close Door"));
    case EDoorState::Closed:
    case EDoorState::Closing:
        return FText::FromString(TEXT("Open Door"));
    case EDoorState::Locked:
        return FText::FromString(TEXT("Door Locked"));
    default:
        return FText::FromString(TEXT("Door"));
    }
}

bool AInteractableDoor::CanBeInteracted_Implementation() const
{
    return CurrentState != EDoorState::Locked;
}

void AInteractableDoor::OpenDoor()
{
    if (CurrentState == EDoorState::Locked) return;

    CurrentState = EDoorState::Opening;
    TargetAngle = OpenAngle;
    CurrentSpeed = DoorSpeed;

    if (OpenSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());
    }

    // Security doors stop consuming power when open
    if (bIsSecurityDoor && PowerSystemRef)
    {
        PowerSystemRef->SetConsumerActive(PowerConsumerID, false);
    }

    OnDoorStateChanged.Broadcast(CurrentState);
}

void AInteractableDoor::CloseDoor()
{
    if (CurrentState == EDoorState::Locked) return;

    CurrentState = EDoorState::Closing;
    TargetAngle = 0.0f;
    CurrentSpeed = DoorSpeed;

    if (CloseSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, CloseSound, GetActorLocation());
    }

    // Security doors consume power when closed
    if (bIsSecurityDoor && PowerSystemRef)
    {
        PowerSystemRef->SetConsumerActive(PowerConsumerID, true);
    }

    OnDoorStateChanged.Broadcast(CurrentState);
}

void AInteractableDoor::ToggleDoor()
{
    if (CurrentState == EDoorState::Open || CurrentState == EDoorState::Opening)
    {
        CloseDoor();
    }
    else
    {
        OpenDoor();
    }
}

void AInteractableDoor::SlamDoor()
{
    // Forced close at high speed (used by random events)
    CurrentState = EDoorState::Closing;
    TargetAngle = 0.0f;
    CurrentSpeed = SlamSpeed;

    if (SlamSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, SlamSound, GetActorLocation());
    }

    OnDoorStateChanged.Broadcast(CurrentState);
}

void AInteractableDoor::LockDoor()
{
    if (CurrentState == EDoorState::Open || CurrentState == EDoorState::Opening)
    {
        CloseDoor();
    }

    CurrentState = EDoorState::Locked;

    if (LockedSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, LockedSound, GetActorLocation());
    }

    OnDoorStateChanged.Broadcast(CurrentState);
}

void AInteractableDoor::UnlockDoor()
{
    if (CurrentState == EDoorState::Locked)
    {
        CurrentState = EDoorState::Closed;
        OnDoorStateChanged.Broadcast(CurrentState);
    }
}

void AInteractableDoor::UpdateDoorAnimation(float DeltaTime)
{
    if (CurrentState != EDoorState::Opening && CurrentState != EDoorState::Closing)
    {
        return;
    }

    const float PrevAngle = CurrentAngle;
    CurrentAngle = FMath::FInterpTo(CurrentAngle, TargetAngle, DeltaTime, CurrentSpeed);

    // Apply rotation to door mesh
    if (DoorMesh)
    {
        DoorMesh->SetRelativeRotation(FRotator(0.0f, CurrentAngle, 0.0f));
    }

    // Check if animation is complete
    if (FMath::IsNearlyEqual(CurrentAngle, TargetAngle, 0.5f))
    {
        CurrentAngle = TargetAngle;

        if (CurrentState == EDoorState::Opening)
        {
            CurrentState = EDoorState::Open;
        }
        else if (CurrentState == EDoorState::Closing)
        {
            CurrentState = EDoorState::Closed;
        }

        OnDoorStateChanged.Broadcast(CurrentState);
    }
}

void AInteractableDoor::RegisterWithPowerSystem()
{
    if (bIsSecurityDoor && PowerSystemRef)
    {
        PowerSystemRef->AddPowerConsumer(PowerConsumerID,
            FFDConstants::DoorDrainPerSecond);
    }
}
