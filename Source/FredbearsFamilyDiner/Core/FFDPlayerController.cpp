// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "FFDPlayerController.h"
#include "FFDGameMode.h"
#include "FredbearsFamilyDiner/Player/FFDPlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AFFDPlayerController::AFFDPlayerController()
{
}

void AFFDPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Add input mapping context
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (DefaultMappingContext)
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

void AFFDPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent* EnhancedInput =
        CastChecked<UEnhancedInputComponent>(InputComponent);

    if (MoveAction)
    {
        EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered,
            this, &AFFDPlayerController::OnMove);
    }

    if (LookAction)
    {
        EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered,
            this, &AFFDPlayerController::OnLook);
    }

    if (SprintAction)
    {
        EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started,
            this, &AFFDPlayerController::OnSprintStarted);
        EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed,
            this, &AFFDPlayerController::OnSprintCompleted);
    }

    if (CrouchAction)
    {
        EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Started,
            this, &AFFDPlayerController::OnCrouchStarted);
        EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Completed,
            this, &AFFDPlayerController::OnCrouchCompleted);
    }

    if (InteractAction)
    {
        EnhancedInput->BindAction(InteractAction, ETriggerEvent::Started,
            this, &AFFDPlayerController::OnInteract);
    }

    if (FlashlightAction)
    {
        EnhancedInput->BindAction(FlashlightAction, ETriggerEvent::Started,
            this, &AFFDPlayerController::OnFlashlightToggle);
    }

    if (CameraToggleAction)
    {
        EnhancedInput->BindAction(CameraToggleAction, ETriggerEvent::Started,
            this, &AFFDPlayerController::OnCameraToggle);
    }

    if (PauseAction)
    {
        EnhancedInput->BindAction(PauseAction, ETriggerEvent::Started,
            this, &AFFDPlayerController::OnPause);
    }
}

void AFFDPlayerController::OnMove(const FInputActionValue& Value)
{
    if (AFFDPlayerCharacter* Character = Cast<AFFDPlayerCharacter>(GetPawn()))
    {
        const FVector2D MoveValue = Value.Get<FVector2D>();
        Character->HandleMoveInput(MoveValue);
    }
}

void AFFDPlayerController::OnLook(const FInputActionValue& Value)
{
    if (AFFDPlayerCharacter* Character = Cast<AFFDPlayerCharacter>(GetPawn()))
    {
        const FVector2D LookValue = Value.Get<FVector2D>();
        Character->HandleLookInput(LookValue);
    }
}

void AFFDPlayerController::OnSprintStarted()
{
    if (AFFDPlayerCharacter* Character = Cast<AFFDPlayerCharacter>(GetPawn()))
    {
        Character->StartSprint();
    }
}

void AFFDPlayerController::OnSprintCompleted()
{
    if (AFFDPlayerCharacter* Character = Cast<AFFDPlayerCharacter>(GetPawn()))
    {
        Character->StopSprint();
    }
}

void AFFDPlayerController::OnCrouchStarted()
{
    if (AFFDPlayerCharacter* Character = Cast<AFFDPlayerCharacter>(GetPawn()))
    {
        Character->StartCrouch();
    }
}

void AFFDPlayerController::OnCrouchCompleted()
{
    if (AFFDPlayerCharacter* Character = Cast<AFFDPlayerCharacter>(GetPawn()))
    {
        Character->StopCrouch();
    }
}

void AFFDPlayerController::OnInteract()
{
    if (AFFDPlayerCharacter* Character = Cast<AFFDPlayerCharacter>(GetPawn()))
    {
        Character->TryInteract();
    }
}

void AFFDPlayerController::OnFlashlightToggle()
{
    if (AFFDPlayerCharacter* Character = Cast<AFFDPlayerCharacter>(GetPawn()))
    {
        Character->ToggleFlashlight();
    }
}

void AFFDPlayerController::OnCameraToggle()
{
    if (AFFDPlayerCharacter* Character = Cast<AFFDPlayerCharacter>(GetPawn()))
    {
        Character->ToggleCameraSystem();
    }
}

void AFFDPlayerController::OnPause()
{
    if (AFFDGameMode* GameMode = Cast<AFFDGameMode>(GetWorld()->GetAuthGameMode()))
    {
        const bool bCurrentlyPaused =
            GameMode->GetCurrentPhase() == EGamePhase::Paused;
        GameMode->SetGamePaused(!bCurrentlyPaused);
    }
}
