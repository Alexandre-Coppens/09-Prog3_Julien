// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enum_TileType.generated.h"

UENUM(BlueprintType)
enum class ETileType : uint8
{
	None			UMETA(DisplayName = "None"),
	Low				UMETA(DisplayName = "Low"),
	Medium			UMETA(DisplayName = "Medium"),
	High			UMETA(DisplayName = "High"),
	River			UMETA(DisplayName = "River"),
	Path			UMETA(DisplayName = "Path")
};