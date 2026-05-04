// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FredbearsFamilyDiner/FredbearsFamilyDiner.h"
#include "RandomEventSystem.generated.h"

class AAudioManager;
class APowerSystem;

UENUM(BlueprintType)
enum class ERandomEventType : uint8
{
    LightFlicker    UMETA(DisplayName = "Light Flicker"),
    DoorSlam        UMETA(DisplayName = "Door Slam"),
    PowerSurge      UMETA(DisplayName = "Power Surge"),
    PhoneRing       UMETA(DisplayName = "Phone Ring"),
    ShadowAppear    UMETA(DisplayName = "Shadow Appear"),
    CameraStatic    UMETA(DisplayName = "Camera Static"),
    FootstepsNearby UMETA(DisplayName = "Footsteps Nearby"),
    VentRattle      UMETA(DisplayName = "Vent Rattle")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRandomEvent, ERandomEventType, EventType);

/**
 * Triggers random environmental events for atmosphere and tension.
 * Events become more frequent as the night progresses and threat
 * level increases.
 */
UCLASS()
class FREDBEARSFAMILYDINER_API ARandomEventSystem : public AActor
{
    GENERATED_BODY()

public:
    ARandomEventSystem();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Events")
    void TriggerEvent(ERandomEventType EventType);

    UFUNCTION(BlueprintCallable, Category = "Events")
    void SetEventFrequencyMultiplier(float Multiplier);

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnRandomEvent OnRandomEvent;

protected:
    void ScheduleNextEvent();
    void ExecuteEvent(ERandomEventType EventType);

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Events|References")
    TObjectPtr<AAudioManager> AudioManagerRef;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Events|References")
    TObjectPtr<APowerSystem> PowerSystemRef;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Events|Config")
    float BaseIntervalMin = 15.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Events|Config")
    float BaseIntervalMax = 60.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Events|Config")
    TArray<ERandomEventType> EnabledEventTypes;

    /** Actors in the level that can flicker (lights) */
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Events|References")
    TArray<AActor*> FlickerableLights;

    /** Doors that can randomly slam */
    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Events|References")
    TArray<AActor*> SlammableDoors;

    float EventTimer = 0.0f;
    float NextEventTime = 20.0f;
    float FrequencyMultiplier = 1.0f;
};
