// Copyright, Wistchze 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/LocomotionEnums.h"
#include "Data/LocomotionStructs.h"
#include "LocomotionComponent.generated.h"

class UGameplayCameraComponent;

UCLASS(ClassGroup=(WistchzeFramework), meta=(BlueprintSpawnableComponent))
class WISTCHZELOCOMOTIONSYSTEM_API ULocomotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULocomotionComponent();

	// ===== Events ===== //
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	/** Called after possessed by or anytime when the controller is changed. */
	UFUNCTION()
	void PossessedBy(APawn* Pawn, AController* OldController, AController* NewController);
	
private:
	// ===== References ===== //

	/** Player References */
	TWeakObjectPtr<APlayerController> PlayerController;
	
	/** Character References */
	TWeakObjectPtr<ACharacter> Character;
	
	/** 
	 * Called in begin play, Init the references and prepare (what should it do next) the references here.
	 * NOTE: Not all references going to be initialized here such as PlayerController, etc.!
	 */
	void InitReferences();

private:
	// ===== Input ====== //

	EAnalogStickBehavior MovementStickMode = EAnalogStickBehavior::FixedSpeed_SingleGait;

	UPROPERTY(EditAnywhere, Category=Input)
	float AnalogMoveThreshold = .7f;

	FCharacterInputState InputState;

	FVector2D GetMovementInputScale(const FVector2D& Input) const;
	
private:
	// ===== Movement ===== //

	UPROPERTY(EditAnywhere, Category=Movement)
	TSoftObjectPtr<UCurveFloat> StrafeSpeedMapCurve;

	EGait Gait = EGait::Run;

	bool bWasLastOnGround = false;
	bool bJustLanded = false;
	bool bUsingAttributeBasedRootMotion = false;

	FVector LandVelocity {};

	UPROPERTY(EditAnywhere, Category=Movement)
	FVector WalkSpeed { 200.f, 180.f, 150.f };

	UPROPERTY(EditAnywhere, Category=Movement)
	FVector RunSpeed { 500.f, 350.f, 300.f };

	UPROPERTY(EditAnywhere, Category=Movement)
	FVector SprintSpeed { 700.f };

	UPROPERTY(EditAnywhere, Category=Movement)
	FVector CrouchSpeed { 225.f, 200.f, 180.f };

	FVector LastVelocity = FVector::ZeroVector;
};
