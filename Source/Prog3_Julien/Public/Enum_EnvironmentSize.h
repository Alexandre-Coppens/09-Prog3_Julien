// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enum_EnvironmentSize.generated.h"

UENUM(BlueprintType)
enum class EEnvironmentSize : uint8
{
	Near		UMETA(DisplayName = "Near"),
	Far			UMETA(DisplayName = "Far")
};