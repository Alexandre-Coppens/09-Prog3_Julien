// Fill out your copyright notice in the Description page of Project Settings.

#include "MyFunctionList.h"

void MyFunctionList::DebugPrint(FString DebugMessage, FColor Color)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, Color, DebugMessage);
    }
    UE_LOG(LogTemp, Log, TEXT("%s"), *DebugMessage);
}