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

enum class ECameraFeature : uint8
{
	CameraPosition = 1,
	CameraZooming
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

	/**
	 * Should this component ticking? If all features are disabled, then this should be 0.
	 * We should modify this variable using bitwise operator.
	 *
	 * Each function has their own key defined in ECameraFeature.
	 * Enable/disable by using the key to modify the binary using bitwise operator.
	 */
	uint8 bEnableTickFeature = 0;
	
	/**
	 * Never directly set the feature variable like "bShiftCamera", use this function instead.
	 * It also enables the component tick.
	 */
	void ActivateFeature(const ECameraFeature Key);

	/** Same as activate, here if ALL features are disabled so turn off the component tick too. */
	void DeactivateFeature(const ECameraFeature Key);

	FORCEINLINE bool IsFeatureActive(const ECameraFeature Key) const
	{
		const uint8 Feature = 1 << static_cast<uint8>(Key);
		
		return bEnableTickFeature & Feature;
	}
	
private:
	// ===== References ===== //

	/** For the camera, we just care about the spring arm. */
	TWeakObjectPtr<USpringArmComponent> SpringArm;

private:
	// ===== Camera Position (Key 1) ===== //
	
	/** Offset from the CURRENT SPRING ARM location (should be center) to the shoulder. */
	UPROPERTY(EditAnywhere, Category=CameraPosition)
	float ShoulderOffset = 80.f;

	/** In which position are we? */
	UPROPERTY(EditAnywhere, Category=CameraPosition)
	ECameraPosition CameraPosition = ECameraPosition::RightShoulder;

	/** Smoothly shifting the camera towards the target offset. */
	void ShiftCamera(const float DeltaTime);

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
	// ====== Camera Zooming (Key 2) ===== //
	
	/**
	 * How close the camera to the head by the percentage when zooming in.
	 * For the maximum zoom out, the maximum is amount of TargetArmLength from actual the Spring Arm Component.
	 */
	UPROPERTY(EditAnywhere, Category=CameraZooming, meta=(ClampMin=0,ClampMax=1))
	float MaxZoomIn = .75;

	/** The value will be obtained from the actual Spring Arm Component's TargetArmLength. */
	float ZoomOutLength = 0.f;

	/** Calculating the result of target arm length from the MaxZoomIn percentage. */
	float ZoomInLength = 0.f;

	UPROPERTY(EditAnywhere, Category=CameraZooming)
	float ZoomSensitivity = 1.f;

	/** Lower means zoom out, higher means zoom in. */
	float CurrentZoomAlpha = 0.f;

	/** Target of the actual target arm length. */
	float NewArmLength = 0.f;

	/** Apply with interpolation to smooth it out. */
	void InterpCameraZoom(const float DeltaTime);
	
public:
	UFUNCTION(BlueprintCallable, Category=CameraZooming)
	void ZoomCamera(const float Input);

	/** This can be used for aiming purpose */
	UFUNCTION(BlueprintCallable, Category=CameraZooming)
	void ZoomByPercentage(const float Percentage);
};
