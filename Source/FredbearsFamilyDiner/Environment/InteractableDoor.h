// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FFDInteractableInterface.h"
#include "InteractableDoor.generated.h"

class UStaticMeshComponent;
class UAudioComponent;
class APowerSystem;

UENUM(BlueprintType)
enum class EDoorState : uint8
{
    Open,
    Closed,
    Opening,
    Closing,
    Locked
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoorStateChanged, EDoorState, NewState);

/**
 * Interactive door that can be opened/closed manually.
 * Consumes power when closed (security doors).
 * Blocks animatronic movement when closed.
 */
UCLASS()
class FREDBEARSFAMILYDINER_API AInteractableDoor : public AActor, public IFFDInteractableInterface
{
    GENERATED_BODY()

public:
    AInteractableDoor();

    virtual void Tick(float DeltaTime) override;

    // --- IFFDInteractableInterface ---
    virtual void OnInteract_Implementation(AActor* Interactor) override;
    virtual FText GetInteractionPrompt_Implementation() const override;
    virtual bool CanBeInteracted_Implementation() const override;

    // --- Door Controls ---

    UFUNCTION(BlueprintCallable, Category = "Door")
    void OpenDoor();

    UFUNCTION(BlueprintCallable, Category = "Door")
    void CloseDoor();

    UFUNCTION(BlueprintCallable, Category = "Door")
    void ToggleDoor();

    UFUNCTION(BlueprintCallable, Category = "Door")
    void SlamDoor();

    UFUNCTION(BlueprintCallable, Category = "Door")
    void LockDoor();

    UFUNCTION(BlueprintCallable, Category = "Door")
    void UnlockDoor();

    UFUNCTION(BlueprintPure, Category = "Door")
    EDoorState GetDoorState() const { return CurrentState; }

    UFUNCTION(BlueprintPure, Category = "Door")
    bool IsOpen() const { return CurrentState == EDoorState::Open; }

    UFUNCTION(BlueprintPure, Category = "Door")
    bool IsClosed() const { return CurrentState == EDoorState::Closed; }

    /** Whether this door blocks animatronics when closed */
    UFUNCTION(BlueprintPure, Category = "Door")
    bool IsSecurityDoor() const { return bIsSecurityDoor; }

    UPROPERTY(BlueprintAssignable, Category = "Door|Events")
    FOnDoorStateChanged OnDoorStateChanged;

protected:
    void UpdateDoorAnimation(float DeltaTime);
    void RegisterWithPowerSystem();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> DoorMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> FrameMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UAudioComponent> DoorAudioComponent;

    // --- Config ---

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door|Config")
    bool bIsSecurityDoor = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door|Config")
    float OpenAngle = 90.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door|Config")
    float DoorSpeed = 3.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door|Config")
    float SlamSpeed = 15.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door|Config")
    FName PowerConsumerID;

    // --- Audio ---

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door|Audio")
    TObjectPtr<USoundBase> OpenSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door|Audio")
    TObjectPtr<USoundBase> CloseSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door|Audio")
    TObjectPtr<USoundBase> SlamSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door|Audio")
    TObjectPtr<USoundBase> LockedSound;

    // --- References ---

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Door|References")
    TObjectPtr<APowerSystem> PowerSystemRef;

    // --- State ---

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|State")
    EDoorState CurrentState = EDoorState::Closed;

    float CurrentAngle = 0.0f;
    float TargetAngle = 0.0f;
    float CurrentSpeed = 0.0f;
};
