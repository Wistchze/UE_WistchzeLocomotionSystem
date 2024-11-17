// Copyright, Wistchze 2024

#include "Components/LocomotionComponent.h"

#include "GameFramework/Character.h"

ULocomotionComponent::ULocomotionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Init Default Assets
	StrafeSpeedMapCurve = FSoftObjectPath { TEXT("/AnonLocomotion/Data/Curves/CF_StrafeSpeedMap.CF_StrafeSpeedMap") };
}

// ===== Events ===== //

void ULocomotionComponent::BeginPlay()
{
	Super::BeginPlay();

	InitReferences();
}

void ULocomotionComponent::TickComponent(const float DeltaTime, const ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void ULocomotionComponent::PossessedBy(APawn*, AController*, AController* NewController)
{
	// Only called on the client side
	if (!Character->IsLocallyControlled()) return;

	PlayerController = Cast<APlayerController>(NewController);
}

// ===== References ===== //

void ULocomotionComponent::InitReferences()
{
	Character = Cast<ACharacter>(GetOwner());
	
	// Bind to character's PossessedBy with this delegate
	Character->ReceiveControllerChangedDelegate.AddDynamic(this, &ThisClass::PossessedBy);
}

// ===== Input ===== //

FVector2D ULocomotionComponent::GetMovementInputScale(const FVector2D& Input) const
{
	switch (MovementStickMode)
	{
		case EAnalogStickBehavior::VariableSpeed_SingleGait:
		case EAnalogStickBehavior::VariableSpeed_WalkRun:
			return Input;

		case EAnalogStickBehavior::FixedSpeed_SingleGait:
		case EAnalogStickBehavior::FixedSpeed_WalkRun:	
			return Input.GetSafeNormal();
	}

	return FVector2D::ZeroVector;
}
