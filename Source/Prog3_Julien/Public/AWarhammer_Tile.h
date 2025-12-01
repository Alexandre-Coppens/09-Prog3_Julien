// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enum_TileType.h"

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* DecorationMesh;

	// Variables

	//Set Here the different Materials (Max: 3)
	UPROPERTY(EditAnywhere, Category = Default)
	TArray<UMaterialInterface*> MaterialsUsed;

	//Set Here the River Material 
	UPROPERTY(EditAnywhere, Category = Default)
	UMaterialInterface* RiverMaterial;

	UPROPERTY(EditAnywhere, Category = Default)
	UMaterialInterface* PathMaterial;

	UPROPERTY(EditAnywhere, Category = Default)
	UMaterialInterface* ForestMaterial;

	UPROPERTY(EditAnywhere, Category = Default)
	UMaterialInterface* CityMaterial;

	UPROPERTY(EditAnywhere, Category = Default)
	TArray<UMaterialInterface*> DebugMaterials;

	UPROPERTY()
	ETileType TileType;

	UPROPERTY()
	bool HasBeenChecked;

	UPROPERTY()
	int8 BorderDist;

private:
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void InitTile();

	UFUNCTION()
	void SetRiver();

	UFUNCTION()
	void SetPath();

	UFUNCTION()
	void SetForest(float height);

	UFUNCTION()
	void SetCity(float height);

	UFUNCTION()
	void DebugShowTile(int8 skin);

private:
	UFUNCTION()
	void SetMaterial(uint8 index);
	
};
