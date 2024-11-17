// Copyright, Wistchze 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraSystemComponent.generated.h"

class USpringArmComponent;

/** To determine which position is the camera at? */
UENUM(BlueprintType)
enum class ECameraPosition : uint8
{
	LeftShoulder,
	Center,
	RightShoulder
};

/**
 * A component to handle the camera, Spring Arm and Camera Component is still needed.
 * This system has features:
 * 1. Able to move shoulder, later it should be automatically moved.
 * 2. Zooming in and out.
 * 3. Smoothly interpolate when collision happens. (UNCHECKED)
 * 
 * NOTE:
 * 1. Spring Arm Offset should be 0! Else we gonna set it to 0 anyway.
 * 2. Disable camera lag and collision test on the spring arm.
 *
 * Those are gonna be handled on this system.
 */
UCLASS(ClassGroup=(WistchzeFramework), meta=(BlueprintSpawnableComponent), HideCategories=(ComponentReplication,Activation,Sockets,ComponentTick,Replication,Navigation))
class WISTCHZELOCOMOTIONSYSTEM_API UCameraSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCameraSystemComponent();

	// ====== Events ===== //
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	// ===== References ===== //

	/** For the camera, we just care about the spring arm. */
	TWeakObjectPtr<USpringArmComponent> SpringArm;

private:
	// ===== Camera Position ===== //

	/** Offset from the CURRENT SPRING ARM location (should be center) to the shoulder. */
	UPROPERTY(EditAnywhere, Category=CameraPosition)
	float ShoulderOffset = 100.f;

	/** In which position are we? */
	UPROPERTY(EditAnywhere, Category=CameraPosition)
	ECameraPosition CameraPosition = ECameraPosition::RightShoulder;

	/** Smoothly shifting the camera towards the target offset. */
	void ShiftCamera(const float DeltaTime) const;

	FORCEINLINE float GetCameraDirection() const
	{
		return CameraPosition == ECameraPosition::LeftShoulder ? -1.f :
			   CameraPosition == ECameraPosition::RightShoulder ? 1.f : 0.f;
	}

	FORCEINLINE float GetTargetOffset() const
	{
		return GetCameraDirection() * ShoulderOffset;
	}

public:
	UFUNCTION(BlueprintCallable, Category=CameraPosition)
	void ChangeCameraPosition();

private:
	// ====== Camera Zooming ===== //
	
	/**
	 * How close the camera to the head by the percentage when zooming in.
	 * For the maximum zoom out, the maximum is amount of TargetArmLength from actual the Spring Arm Component.
	 */
	UPROPERTY(EditAnywhere, Category=CameraZooming, meta=(ClampMin=0,ClampMax=1))
	float MaxZoomIn = .75;

	/** The value will be obtained from the actual Spring Arm Component's TargetArmLength */
	float ZoomOutLength = 0.f;

	/** Calculating the result of target arm length from the MaxZoomIn percentage */
	float ZoomInLength = 0.f;

	UPROPERTY(EditAnywhere, Category=CameraZooming)
	float ZoomSensitivity = .5f;

	/** Lower means zoom out, higher means zoom in */
	float CurrentZoomAlpha = 0.f;

	/** Target of the actual target arm length */
	float NewArmLength = 0.f;

	/** Apply with interpliation to smooth it out */
	void InterpCameraZoom(const float DeltaTime) const;
	
public:
	UFUNCTION(BlueprintCallable, Category=CameraZooming)
	void ZoomCamera(const float Input);
};
