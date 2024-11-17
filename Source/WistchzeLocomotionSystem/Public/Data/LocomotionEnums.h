// Copyright, Wistchze 2024

#pragma once

#include "LocomotionEnums.generated.h"

UENUM(BlueprintType)
enum class EAnalogStickBehavior : uint8
{
	FixedSpeed_SingleGait,
	FixedSpeed_WalkRun,
	VariableSpeed_SingleGait,
	VariableSpeed_WalkRun,
};

UENUM(BlueprintType)
enum class ECameraStyle : uint8
{
	Close,
	Balanced,
	Far
};

UENUM(BlueprintType)
enum class EGait : uint8
{
	Walk,
	Run,
	Sprint
};
