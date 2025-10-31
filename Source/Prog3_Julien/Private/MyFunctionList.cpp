// Fill out your copyright notice in the Description page of Project Settings.

#include "MyFunctionList.h"

void MyFunctionList::DebugPrint(FString DebugMessage)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, DebugMessage);
    }
    UE_LOG(LogTemp, Log, TEXT("%s"), *DebugMessage);
}