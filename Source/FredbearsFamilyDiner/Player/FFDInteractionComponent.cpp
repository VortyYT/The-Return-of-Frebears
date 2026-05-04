// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "FFDInteractionComponent.h"
#include "FredbearsFamilyDiner/Environment/FFDInteractableInterface.h"

UFFDInteractionComponent::UFFDInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UFFDInteractionComponent::SetCurrentTarget(AActor* NewTarget)
{
    // Only accept targets that implement the interactable interface
    if (NewTarget && !NewTarget->GetClass()->ImplementsInterface(
        UFFDInteractableInterface::StaticClass()))
    {
        NewTarget = nullptr;
    }

    if (CurrentTarget != NewTarget)
    {
        CurrentTarget = NewTarget;
        OnInteractionTargetChanged.Broadcast(CurrentTarget);
    }
}

void UFFDInteractionComponent::TryInteract()
{
    if (!HasValidTarget()) return;

    if (IFFDInteractableInterface* Interactable =
        Cast<IFFDInteractableInterface>(CurrentTarget))
    {
        Interactable->OnInteract(GetOwner());
        UE_LOG(LogTemp, Verbose, TEXT("Interacted with: %s"), *CurrentTarget->GetName());
    }
}

bool UFFDInteractionComponent::HasValidTarget() const
{
    return CurrentTarget != nullptr &&
        IsValid(CurrentTarget) &&
        CurrentTarget->GetClass()->ImplementsInterface(
            UFFDInteractableInterface::StaticClass());
}

FText UFFDInteractionComponent::GetInteractionPrompt() const
{
    if (!HasValidTarget()) return FText::GetEmpty();

    if (const IFFDInteractableInterface* Interactable =
        Cast<IFFDInteractableInterface>(CurrentTarget))
    {
        return Interactable->GetInteractionPrompt();
    }

    return FText::FromString(TEXT("Interact"));
}
