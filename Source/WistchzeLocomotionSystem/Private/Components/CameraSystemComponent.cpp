// Copyright, Wistchze 2024

#include "Components/CameraSystemComponent.h"

#include "GameFramework/SpringArmComponent.h"

UCameraSystemComponent::UCameraSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Making sure that this won't be replicated, it's really not needed to be
	SetIsReplicatedByDefault(false);
}

// ===== Events ===== //

void UCameraSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// Init References
	SpringArm = GetOwner()->GetComponentByClass<USpringArmComponent>();
	SpringArm->AddTickPrerequisiteComponent(this);

	ZoomOutLength = SpringArm->TargetArmLength;
	NewArmLength = SpringArm->TargetArmLength;

	// Start with the selected camera position
	SpringArm->SocketOffset.Y = GetTargetOffset();

	// Calculating the result of target arm length from the MaxZoomIn percentage
	ZoomInLength = ZoomOutLength - ZoomOutLength * MaxZoomIn;
}

void UCameraSystemComponent::TickComponent(const float DeltaTime, const ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ShiftCamera(DeltaTime);
	InterpCameraZoom(DeltaTime);
}

// ===== Camera Move ===== //

void UCameraSystemComponent::ShiftCamera(const float DeltaTime) const
{
	const float CurrentOffset = SpringArm->SocketOffset.Y;
	const float NewOffset = FMath::FInterpTo(CurrentOffset, GetTargetOffset(), DeltaTime, 3.f);

	SpringArm->SocketOffset.Y = NewOffset;
}

void UCameraSystemComponent::ChangeCameraPosition()
{
	const uint8 CurrentPosition = static_cast<uint8>(CameraPosition);
	const uint8 NewPosition = (CurrentPosition + 1) % 3;

	CameraPosition = static_cast<ECameraPosition>(NewPosition);
}

// ===== Camera Zooming ===== //

void UCameraSystemComponent::InterpCameraZoom(const float DeltaTime) const
{
	// Apply with Interpolation
	SpringArm->TargetArmLength = FMath::FInterpTo(
		SpringArm->TargetArmLength,
		NewArmLength,
		DeltaTime,
		3.f
	);
}

void UCameraSystemComponent::ZoomCamera(const float Input)
{
	// Getting the new alpha first
	CurrentZoomAlpha = FMath::Clamp(CurrentZoomAlpha + Input * .1f * ZoomSensitivity, .0f, MaxZoomIn);

	// Getting the actual TargetArmLength with Lerp and the calculated Alpha
	NewArmLength = FMath::Lerp(ZoomOutLength, ZoomInLength, CurrentZoomAlpha);
}
