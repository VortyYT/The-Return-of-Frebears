// Copyright (c) 2024 Fredbear's Family Diner Project. All rights reserved.

#include "SecurityCamera.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"

ASecurityCamera::ASecurityCamera()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
    SetRootComponent(SceneCapture);

    SceneCapture->bCaptureEveryFrame = false;
    SceneCapture->bCaptureOnMovement = false;
    SceneCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
}

void ASecurityCamera::BeginPlay()
{
    Super::BeginPlay();

    // Create render target if not assigned
    if (!RenderTarget)
    {
        RenderTarget = NewObject<UTextureRenderTarget2D>(this);
        RenderTarget->InitAutoFormat(
            static_cast<uint32>(CaptureResolution),
            static_cast<uint32>(CaptureResolution));
        RenderTarget->UpdateResourceImmediate(true);
    }

    if (SceneCapture)
    {
        SceneCapture->TextureTarget = RenderTarget;
    }
}

void ASecurityCamera::SetCameraActive(bool bActive)
{
    bIsActive = bActive;

    if (SceneCapture)
    {
        SceneCapture->bCaptureEveryFrame = bActive;

        if (bActive)
        {
            SceneCapture->CaptureScene();
        }
    }
}
