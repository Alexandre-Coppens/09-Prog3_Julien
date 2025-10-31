// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AWarhammer_Tile.h"

#include "FRowArray.generated.h"

USTRUCT(BlueprintType)
struct FRowArray
{
    GENERATED_BODY()

public:
    UPROPERTY()
    TArray<AWarhammer_Tile*> RowArray;
};