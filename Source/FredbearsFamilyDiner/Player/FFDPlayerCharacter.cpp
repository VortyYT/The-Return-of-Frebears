// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "FFDPlayerCharacter.h"
#include "FFDFlashlightComponent.h"
#include "FFDStaminaComponent.h"
#include "FFDInteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FredbearsFamilyDiner/Core/FFDGameMode.h"
#include "Kismet/GameplayStatics.h"

AFFDPlayerCharacter::AFFDPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // First-person camera attached to the capsule
    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(GetRootComponent());
    FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));
    FirstPersonCamera->bUsePawnControlRotation = true;

    // Flashlight component
    FlashlightComponent = CreateDefaultSubobject<UFFDFlashlightComponent>(TEXT("Flashlight"));
    FlashlightComponent->SetupAttachment(FirstPersonCamera);

    // Stamina component
    StaminaComponent = CreateDefaultSubobject<UFFDStaminaComponent>(TEXT("Stamina"));

    // Interaction component
    InteractionComponent = CreateDefaultSubobject<UFFDInteractionComponent>(TEXT("Interaction"));

    // Character movement defaults
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (MoveComp)
    {
        MoveComp->MaxWalkSpeed = WalkSpeed;
        MoveComp->MaxWalkSpeedCrouched = CrouchSpeed;
        MoveComp->NavAgentProps.bCanCrouch = true;
        MoveComp->SetCrouchedHalfHeight(44.0f);
    }
}

void AFFDPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = WalkSpeed;
    }
}

void AFFDPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsAlive) return;

    UpdateNoiseLevel();
    PerformInteractionTrace();
}

void AFFDPlayerCharacter::HandleMoveInput(const FVector2D& MoveValue)
{
    if (!bIsAlive || bViewingCameras) return;

    const FRotator Rotation = GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDir, MoveValue.Y);
    AddMovementInput(RightDir, MoveValue.X);
}

void AFFDPlayerCharacter::HandleLookInput(const FVector2D& LookValue)
{
    if (!bIsAlive || bViewingCameras) return;

    AddControllerYawInput(LookValue.X * LookSensitivity);
    AddControllerPitchInput(-LookValue.Y * LookSensitivity);
}

void AFFDPlayerCharacter::StartSprint()
{
    if (!bIsAlive || bIsCrouching || bViewingCameras) return;
    if (!StaminaComponent || StaminaComponent->IsExhausted()) return;

    bIsSprinting = true;
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = SprintSpeed;
    }
}

void AFFDPlayerCharacter::StopSprint()
{
    bIsSprinting = false;
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = WalkSpeed;
    }
}

void AFFDPlayerCharacter::StartCrouch()
{
    if (!bIsAlive || bViewingCameras) return;

    StopSprint();
    bIsCrouching = true;
    Crouch();
    OnPlayerCrouching.Broadcast(true);
}

void AFFDPlayerCharacter::StopCrouch()
{
    bIsCrouching = false;
    UnCrouch();
    OnPlayerCrouching.Broadcast(false);
}

void AFFDPlayerCharacter::TryInteract()
{
    if (!bIsAlive) return;

    if (InteractionComponent)
    {
        InteractionComponent->TryInteract();
    }
}

void AFFDPlayerCharacter::ToggleFlashlight()
{
    if (!bIsAlive) return;

    if (FlashlightComponent)
    {
        FlashlightComponent->ToggleFlashlight();
        OnFlashlightToggled.Broadcast(FlashlightComponent->IsFlashlightOn());
    }
}

void AFFDPlayerCharacter::ToggleCameraSystem()
{
    if (!bIsAlive) return;

    bViewingCameras = !bViewingCameras;

    if (bViewingCameras)
    {
        StopSprint();
    }

    OnCameraSystemToggled.Broadcast(bViewingCameras);
}

void AFFDPlayerCharacter::KillPlayer()
{
    if (!bIsAlive) return;

    bIsAlive = false;
    bIsSprinting = false;
    bViewingCameras = false;

    // Disable movement
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->DisableMovement();
    }

    // Notify game mode
    if (AFFDGameMode* GameMode = Cast<AFFDGameMode>(
        UGameplayStatics::GetGameMode(this)))
    {
        GameMode->TriggerPlayerDeath(nullptr);
    }

    UE_LOG(LogFFD, Warning, TEXT("Player has been killed"));
}

void AFFDPlayerCharacter::UpdateNoiseLevel()
{
    const float Speed = GetVelocity().Size();

    if (Speed < 10.0f)
    {
        CurrentNoiseLevel = IdleNoiseLevel;
    }
    else if (bIsCrouching)
    {
        CurrentNoiseLevel = CrouchNoiseLevel;
    }
    else if (bIsSprinting)
    {
        CurrentNoiseLevel = SprintNoiseLevel;
    }
    else
    {
        CurrentNoiseLevel = WalkNoiseLevel;
    }
}

void AFFDPlayerCharacter::PerformInteractionTrace()
{
    if (!FirstPersonCamera) return;

    FVector TraceStart = FirstPersonCamera->GetComponentLocation();
    FVector TraceEnd = TraceStart +
        (FirstPersonCamera->GetForwardVector() * InteractionRange);

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult, TraceStart, TraceEnd, ECC_Visibility, Params);

    CurrentInteractTarget = bHit ? HitResult.GetActor() : nullptr;

    if (InteractionComponent)
    {
        InteractionComponent->SetCurrentTarget(CurrentInteractTarget);
    }
}
