// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BP_Warhammer_MapMaker.generated.h"

UCLASS()
class PROG3_JULIEN_API ABP_Warhammer_MapMaker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABP_Warhammer_MapMaker();

	// Components
	UPROPERTY()
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MapSizeDebug;

	// Variables
	UPROPERTY(EditAnywhere, Category = Default)
	float Scale;

	UPROPERTY(EditAnywhere, Category = Default)
	FIntVector2 MapSize;

private:

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Make the function callable from Blueprints
	UFUNCTION(CallInEditor, Category = Default)
	void MyEditorFunction();

	//virtual void OnConstruction(const FTransform& Transform) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
