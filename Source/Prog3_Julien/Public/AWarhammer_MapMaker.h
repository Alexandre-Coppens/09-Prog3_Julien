#pragma once

#include "AWarhammer_Tile.h"
//#include "Enum_EnvironmentScale.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AWarhammer_MapMaker.generated.h"

UCLASS()
class PROG3_JULIEN_API AWarhammer_MapMaker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWarhammer_MapMaker();

	// Components
	UPROPERTY()
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MapSizeDebug;


	// Variables
	UPROPERTY(EditAnywhere, Category = Default)
	FIntVector2 MapSize;

	//UPROPERTY(EditAnywhere, Category = Default)
	//Enum_EnvironmentScale MapScale;

	UPROPERTY(EditAnywhere, Category = Default)
	float TileScale;

	UPROPERTY(EditAnywhere, Category = Default)
	TSubclassOf<AWarhammer_Tile> TileClass;

private:
	UPROPERTY()
	TArray<AWarhammer_Tile*> TileArray;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Make the function callable from Blueprints
	UFUNCTION(CallInEditor, Category = Default)
	void BuildMap();

	UFUNCTION(CallInEditor, Category = Default)
	void DestroyAllTiles();

	//virtual void OnConstruction(const FTransform& Transform) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	UFUNCTION()
	void Resize();

	UFUNCTION()
	void CreateBaseMap();
};
