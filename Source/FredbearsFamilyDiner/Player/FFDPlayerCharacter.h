// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FredbearsFamilyDiner/FredbearsFamilyDiner.h"
#include "FFDPlayerCharacter.generated.h"

class UCameraComponent;
class UFFDFlashlightComponent;
class UFFDStaminaComponent;
class UFFDInteractionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFlashlightToggled, bool, bIsOn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCameraSystemToggled, bool, bIsViewing);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerCrouching, bool, bIsCrouching);

/**
 * First-person player character for Fredbear's Family Diner.
 * Handles movement, flashlight, interaction, and noise generation.
 */
UCLASS()
class FREDBEARSFAMILYDINER_API AFFDPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AFFDPlayerCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // --- Input Handlers (called by PlayerController) ---

    void HandleMoveInput(const FVector2D& MoveValue);
    void HandleLookInput(const FVector2D& LookValue);
    void StartSprint();
    void StopSprint();
    void StartCrouch();
    void StopCrouch();
    void TryInteract();
    void ToggleFlashlight();
    void ToggleCameraSystem();

    // --- State Queries ---

    UFUNCTION(BlueprintPure, Category = "Player")
    bool IsSprinting() const { return bIsSprinting; }

    UFUNCTION(BlueprintPure, Category = "Player")
    bool IsPlayerCrouching() const { return bIsCrouching; }

    UFUNCTION(BlueprintPure, Category = "Player")
    bool IsViewingCameras() const { return bViewingCameras; }

    UFUNCTION(BlueprintPure, Category = "Player")
    bool IsAlive() const { return bIsAlive; }

    UFUNCTION(BlueprintPure, Category = "Player")
    float GetNoiseLevel() const { return CurrentNoiseLevel; }

    UFUNCTION(BlueprintPure, Category = "Player")
    UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }

    UFUNCTION(BlueprintPure, Category = "Player")
    UFFDFlashlightComponent* GetFlashlight() const { return FlashlightComponent; }

    UFUNCTION(BlueprintPure, Category = "Player")
    UFFDStaminaComponent* GetStamina() const { return StaminaComponent; }

    /** Kill the player (called by animatronics on attack) */
    UFUNCTION(BlueprintCallable, Category = "Player")
    void KillPlayer();

    // --- Delegates ---

    UPROPERTY(BlueprintAssignable, Category = "Player|Events")
    FOnFlashlightToggled OnFlashlightToggled;

    UPROPERTY(BlueprintAssignable, Category = "Player|Events")
    FOnCameraSystemToggled OnCameraSystemToggled;

    UPROPERTY(BlueprintAssignable, Category = "Player|Events")
    FOnPlayerCrouching OnPlayerCrouching;

protected:
    /** Update the noise the player makes based on movement state */
    void UpdateNoiseLevel();

    /** Perform a ray-cast to find interactable objects */
    void PerformInteractionTrace();

    // --- Components ---

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UCameraComponent> FirstPersonCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UFFDFlashlightComponent> FlashlightComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UFFDStaminaComponent> StaminaComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UFFDInteractionComponent> InteractionComponent;

    // --- Config ---

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Config")
    float WalkSpeed = FFDConstants::DefaultWalkSpeed;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Config")
    float SprintSpeed = FFDConstants::DefaultSprintSpeed;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Config")
    float CrouchSpeed = FFDConstants::DefaultCrouchSpeed;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Config")
    float LookSensitivity = 1.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Config")
    float InteractionRange = 250.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Noise")
    float WalkNoiseLevel = 0.3f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Noise")
    float SprintNoiseLevel = 0.8f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Noise")
    float CrouchNoiseLevel = 0.1f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player|Noise")
    float IdleNoiseLevel = 0.0f;

    // --- Runtime State ---

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|State")
    bool bIsSprinting = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|State")
    bool bIsCrouching = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|State")
    bool bViewingCameras = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|State")
    bool bIsAlive = true;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|State")
    float CurrentNoiseLevel = 0.0f;

    UPROPERTY()
    TObjectPtr<AActor> CurrentInteractTarget;
};
