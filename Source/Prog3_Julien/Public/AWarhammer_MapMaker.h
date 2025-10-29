#pragma once

#include "AWarhammer_Tile.h"
#include "Enum_EnvironmentSize.h"

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


	/* Variables */

	//Set the number of Tiles on the map
	UPROPERTY(EditAnywhere, Category = Default)
	FIntVector2 MapSize;

	//Change the Scale of the map
	//Example: Near = HouseScale / Far = CityScale
	UPROPERTY(EditAnywhere, Category = Default)
	EEnvironmentSize MapScale;

	//Set A Tile Size in Meters
	UPROPERTY(EditAnywhere, Category = Default)
	float TileScale;

	//Set What is the type of Tile Used
	UPROPERTY(EditAnywhere, Category = Default)
	TSubclassOf<AWarhammer_Tile> TileClass;

	//Set the Frequency of the Noice
	UPROPERTY(EditAnywhere, Category = Default)
	float NoiseFrequency;

private:
	UPROPERTY()
	TArray<AWarhammer_Tile*> TileArray;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Create new Map
	UFUNCTION(CallInEditor, Category = Default)
	void BuildMap();

	//Remove Tiles referenced in this Actor
	UFUNCTION(CallInEditor, Category = Default)
	void DestroyAllTiles();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	UFUNCTION()
	void Resize();

	UFUNCTION()
	void CreateBaseMap();

	UFUNCTION()
	uint8 GetHeightElevation(float X, float Y, float Frequency);
};
