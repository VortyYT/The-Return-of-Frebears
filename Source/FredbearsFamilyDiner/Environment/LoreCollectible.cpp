// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "LoreCollectible.h"
#include "FredbearsFamilyDiner/Core/FFDGameState.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ALoreCollectible::ALoreCollectible()
{
    PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);
    Mesh->SetCollisionProfileName(TEXT("OverlapAll"));
}

void ALoreCollectible::BeginPlay()
{
    Super::BeginPlay();

    // Check if already collected in save data
    if (AFFDGameState* GameState = Cast<AFFDGameState>(
        UGameplayStatics::GetGameState(this)))
    {
        if (GameState->HasCollectedLoreItem(LoreID))
        {
            bCollected = true;
            // Keep the visual but mark as collected
        }
    }
}

void ALoreCollectible::OnInteract_Implementation(AActor* Interactor)
{
    if (bCollected) return;

    bCollected = true;

    // Register with game state
    if (AFFDGameState* GameState = Cast<AFFDGameState>(
        UGameplayStatics::GetGameState(this)))
    {
        GameState->CollectLoreItem(LoreID);
    }

    // Play collect sound
    if (CollectSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, CollectSound, GetActorLocation());
    }

    // Play audio recording if this is a recording type
    if (LoreType == ELoreType::Recording && AudioRecording)
    {
        UGameplayStatics::PlaySound2D(this, AudioRecording);
    }

    UE_LOG(LogFFD, Log, TEXT("Collected lore: %s - %s"),
        *LoreID.ToString(), *LoreTitle.ToString());
}

FText ALoreCollectible::GetInteractionPrompt_Implementation() const
{
    if (bCollected)
    {
        return FText::FromString(TEXT("(Already Read)"));
    }

    switch (LoreType)
    {
    case ELoreType::Poster:     return FText::FromString(TEXT("Read Poster"));
    case ELoreType::Newspaper:  return FText::FromString(TEXT("Read Newspaper"));
    case ELoreType::Note:       return FText::FromString(TEXT("Read Note"));
    case ELoreType::Recording:  return FText::FromString(TEXT("Play Recording"));
    case ELoreType::Photo:      return FText::FromString(TEXT("Examine Photo"));
    case ELoreType::HiddenItem: return FText::FromString(TEXT("Pick Up"));
    default:                    return FText::FromString(TEXT("Examine"));
    }
}

bool ALoreCollectible::CanBeInteracted_Implementation() const
{
    return !bCollected;
}
