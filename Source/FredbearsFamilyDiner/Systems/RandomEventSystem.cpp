// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "RandomEventSystem.h"
#include "AudioManager.h"
#include "PowerSystem.h"
#include "FredbearsFamilyDiner/Environment/FlickerLight.h"
#include "FredbearsFamilyDiner/Environment/InteractableDoor.h"
#include "Kismet/GameplayStatics.h"

ARandomEventSystem::ARandomEventSystem()
{
    PrimaryActorTick.bCanEverTick = true;

    // Default enabled events
    EnabledEventTypes = {
        ERandomEventType::LightFlicker,
        ERandomEventType::DoorSlam,
        ERandomEventType::PowerSurge,
        ERandomEventType::FootstepsNearby,
        ERandomEventType::VentRattle,
        ERandomEventType::CameraStatic
    };
}

void ARandomEventSystem::BeginPlay()
{
    Super::BeginPlay();
    ScheduleNextEvent();
}

void ARandomEventSystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    EventTimer += DeltaTime;

    if (EventTimer >= NextEventTime)
    {
        // Pick a random event from enabled types
        if (EnabledEventTypes.Num() > 0)
        {
            const int32 Index = FMath::RandRange(0, EnabledEventTypes.Num() - 1);
            ExecuteEvent(EnabledEventTypes[Index]);
        }

        ScheduleNextEvent();
    }
}

void ARandomEventSystem::TriggerEvent(ERandomEventType EventType)
{
    ExecuteEvent(EventType);
}

void ARandomEventSystem::SetEventFrequencyMultiplier(float Multiplier)
{
    FrequencyMultiplier = FMath::Clamp(Multiplier, 0.1f, 5.0f);
}

void ARandomEventSystem::ScheduleNextEvent()
{
    EventTimer = 0.0f;
    NextEventTime = FMath::RandRange(BaseIntervalMin, BaseIntervalMax) / FrequencyMultiplier;
}

void ARandomEventSystem::ExecuteEvent(ERandomEventType EventType)
{
    OnRandomEvent.Broadcast(EventType);

    switch (EventType)
    {
    case ERandomEventType::LightFlicker:
    {
        // Make random lights flicker
        if (FlickerableLights.Num() > 0)
        {
            const int32 NumToFlicker = FMath::RandRange(1,
                FMath::Min(3, FlickerableLights.Num()));

            for (int32 i = 0; i < NumToFlicker; ++i)
            {
                const int32 Idx = FMath::RandRange(0, FlickerableLights.Num() - 1);
                if (AFlickerLight* Light = Cast<AFlickerLight>(FlickerableLights[Idx]))
                {
                    Light->TriggerFlicker(FMath::RandRange(1.0f, 4.0f));
                }
            }
        }
        UE_LOG(LogFFD, Verbose, TEXT("Random event: Light flicker"));
        break;
    }

    case ERandomEventType::DoorSlam:
    {
        if (SlammableDoors.Num() > 0)
        {
            const int32 Idx = FMath::RandRange(0, SlammableDoors.Num() - 1);
            if (AInteractableDoor* Door = Cast<AInteractableDoor>(SlammableDoors[Idx]))
            {
                Door->SlamDoor();
            }
        }
        UE_LOG(LogFFD, Verbose, TEXT("Random event: Door slam"));
        break;
    }

    case ERandomEventType::PowerSurge:
    {
        // Brief power fluctuation
        if (PowerSystemRef)
        {
            const float DrainAmount = FMath::RandRange(1.0f, 5.0f);
            PowerSystemRef->RestorePower(-DrainAmount);
        }

        // Flicker all lights briefly
        for (AActor* LightActor : FlickerableLights)
        {
            if (AFlickerLight* Light = Cast<AFlickerLight>(LightActor))
            {
                Light->TriggerFlicker(0.5f);
            }
        }
        UE_LOG(LogFFD, Verbose, TEXT("Random event: Power surge"));
        break;
    }

    case ERandomEventType::FootstepsNearby:
    {
        if (AudioManagerRef)
        {
            AudioManagerRef->PlayRandomAmbientEvent();
        }
        UE_LOG(LogFFD, Verbose, TEXT("Random event: Footsteps nearby"));
        break;
    }

    case ERandomEventType::VentRattle:
    {
        if (AudioManagerRef)
        {
            AudioManagerRef->PlayRandomAmbientEvent();
        }
        UE_LOG(LogFFD, Verbose, TEXT("Random event: Vent rattle"));
        break;
    }

    case ERandomEventType::CameraStatic:
    {
        UE_LOG(LogFFD, Verbose, TEXT("Random event: Camera static burst"));
        break;
    }

    default:
        break;
    }
}
