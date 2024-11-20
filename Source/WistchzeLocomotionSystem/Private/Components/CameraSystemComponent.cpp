// Copyright, WistchzeL 2024

#include "Components/CameraSystemComponent.h"

#include "GameFramework/SpringArmComponent.h"

UCameraSystemComponent::UCameraSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Only needed to tick in client-side, enable tick on client only
	PrimaryComponentTick.bAllowTickOnDedicatedServer = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	// Making sure that this won't be replicated, it's really not needed to be
	SetIsReplicatedByDefault(false);
}

// ===== Events ===== //

void UCameraSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// Check is it on client-side?
	if (!Cast<APawn>(GetOwner())->IsLocallyControlled()) return;
	
	// Init References
	SpringArm = GetOwner()->GetComponentByClass<USpringArmComponent>();
	SpringArm->AddTickPrerequisiteComponent(this);

	// Zooming feature
	ZoomOutLength = SpringArm->TargetArmLength;
	NewArmLength = SpringArm->TargetArmLength;
	ZoomInLength = ZoomOutLength - ZoomOutLength * MaxZoomIn; // Calculating the result of target arm length from the MaxZoomIn percentage

	// Camera Shifting
	SpringArm->SocketOffset.Y = GetTargetOffset();
}

void UCameraSystemComponent::TickComponent(const float DeltaTime, const ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ShiftCamera(DeltaTime);
	InterpCameraZoom(DeltaTime);
}

void UCameraSystemComponent::ActivateFeature(const ECameraFeature Key)
{
	const uint8 Feature = 1 << static_cast<uint8>(Key);
	bEnableTickFeature |= Feature;

	SetComponentTickEnabled(true);
	SpringArm->SetComponentTickEnabled(true);
}

void UCameraSystemComponent::DeactivateFeature(const ECameraFeature Key)
{
	const uint8 Feature = 1 << static_cast<uint8>(Key);
	bEnableTickFeature &= ~Feature;

	// Check all features
	if (!bEnableTickFeature)
	{
		SetComponentTickEnabled(false);
		SpringArm->SetComponentTickEnabled(false);
	}
}

// ===== Camera Move ===== //

void UCameraSystemComponent::ShiftCamera(const float DeltaTime)
{
	if (!IsFeatureActive(ECameraFeature::CameraPosition)) return;
	
	const float CurrentOffset = SpringArm->SocketOffset.Y;
	const float TargetOffset = GetTargetOffset();
	const float NewOffset = FMath::FInterpTo(CurrentOffset, TargetOffset, DeltaTime, 3.f);

	SpringArm->SocketOffset.Y = NewOffset;

	if (FMath::IsNearlyEqual(NewOffset, TargetOffset, .25f)) DeactivateFeature(ECameraFeature::CameraPosition);
}

void UCameraSystemComponent::ChangeCameraPosition()
{
	ActivateFeature(ECameraFeature::CameraPosition);
	
	const uint8 CurrentPosition = static_cast<uint8>(CameraPosition);
	const uint8 NewPosition = (CurrentPosition + 1) % 3;

	CameraPosition = static_cast<ECameraPosition>(NewPosition);
}

// ===== Camera Zooming ===== //

void UCameraSystemComponent::InterpCameraZoom(const float DeltaTime)
{
	if (!IsFeatureActive(ECameraFeature::CameraZooming)) return;
	
	// Apply with Interpolation
	SpringArm->TargetArmLength = FMath::FInterpTo(
		SpringArm->TargetArmLength,
		NewArmLength,
		DeltaTime,
		2.f
	);

	if (FMath::IsNearlyEqual(SpringArm->TargetArmLength, NewArmLength, .25f)) DeactivateFeature(ECameraFeature::CameraZooming);
}

void UCameraSystemComponent::ZoomCamera(const float Input)
{
	ActivateFeature(ECameraFeature::CameraZooming);
	
	// Getting the new alpha first
	CurrentZoomAlpha = FMath::Clamp(CurrentZoomAlpha + Input * .05f * ZoomSensitivity, .0f, MaxZoomIn);

	// Getting the actual TargetArmLength with Lerp and the calculated Alpha
	NewArmLength = FMath::Lerp(ZoomOutLength, ZoomInLength, CurrentZoomAlpha);
}

void UCameraSystemComponent::ZoomByPercentage(const float Percentage)
{
	ActivateFeature(ECameraFeature::CameraZooming);

	CurrentZoomAlpha = FMath::Clamp(Percentage, .0f, MaxZoomIn);
	NewArmLength = FMath::Lerp(ZoomOutLength, ZoomInLength, CurrentZoomAlpha);
}
