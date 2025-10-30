// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AWarhammer_Tile.generated.h"

UCLASS()
class PROG3_JULIEN_API AWarhammer_Tile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWarhammer_Tile();

	// Components
	UPROPERTY()
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* TileMesh;

	// Variables

	//Set Here the different Materials (Max: 3)
	UPROPERTY(EditAnywhere, Category = Default)
	TArray<UMaterialInterface*> MaterialsUsed;

private:

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void InitTile();

private:
	UFUNCTION()
	void SetMaterial(uint8 index);

	UFUNCTION()
	void CreateRiver();

};
