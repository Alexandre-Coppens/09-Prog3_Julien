// Fill out your copyright notice in the Description page of Project Settings.


#include "AWarhammer_Tile.h"
#include "MyFunctionList.h"

// Sets default values
AWarhammer_Tile::AWarhammer_Tile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	TileMesh->SetupAttachment(Root);

	TileType = ETileType::None;
}

// Called when the game starts or when spawned
void AWarhammer_Tile::BeginPlay()
{
	Super::BeginPlay();
	PrimaryActorTick.bCanEverTick = false;
}

// Called every frame
void AWarhammer_Tile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWarhammer_Tile::InitTile()
{
	AActor* Parent = GetAttachParentActor();

	TileMesh->SetRelativeLocation(FVector(0, 0, TileMesh->GetRelativeScale3D().Z * 50.0f));

	if (!Parent)
	{
		FString DebugMessage = FString::Printf(TEXT("Tile has no Parent Actor"));
		MyFunctionList::DebugPrint(DebugMessage, FColor::Red);
		return;
	}

	FVector ActorScale = GetActorScale();

	if (ActorScale.Z == 1.5f * ActorScale.X)
	{
		SetMaterial(0);
		TileType = ETileType::Low;
	}
	else if (ActorScale.Z == 2.0f * ActorScale.X)
	{
		SetMaterial(1);
		TileType = ETileType::Medium;
	}
	else
	{
		SetMaterial(2);
		TileType = ETileType::High;
	}
}

void AWarhammer_Tile::SetRiver()
{
	FVector ActorScale = GetActorScale();
	TileMesh->SetWorldScale3D(FVector(ActorScale.X, ActorScale.Y, ActorScale.X));
	TileMesh->SetRelativeLocation(FVector(0, 0, TileMesh->GetRelativeScale3D().Z * 50.0f));

	if (!RiverMaterial) 
	{
		FString DebugMessage = FString::Printf(TEXT("Please Add a River Material"));
		MyFunctionList::DebugPrint(DebugMessage, FColor::Red);
	}
	TileMesh->SetMaterial(0, RiverMaterial);
	TileType = ETileType::River;
}
void AWarhammer_Tile::SetPath()
{
	FVector ActorScale = GetActorScale();
	TileMesh->SetWorldScale3D(FVector(ActorScale.X, ActorScale.Y, ActorScale.Z - ActorScale.X * 0.25f));
	TileMesh->SetRelativeLocation(FVector(0, 0, TileMesh->GetRelativeScale3D().Z * 50.0f));

	if (!PathMaterial)
	{
		FString DebugMessage = FString::Printf(TEXT("Please Add a Path Material"));
		MyFunctionList::DebugPrint(DebugMessage, FColor::Red);
	}
	TileMesh->SetMaterial(0, PathMaterial);
	TileType = ETileType::Path;
}

void AWarhammer_Tile::DebugShowTile()
{
	TileMesh->SetMaterial(0, DebugMaterial);
}

void AWarhammer_Tile::SetMaterial(uint8 index)
{
	if (MaterialsUsed.Num() == 0)
	{
		FString DebugMessage = FString::Printf(TEXT("Please Add at least 1 Material to Tile"));
		MyFunctionList::DebugPrint(DebugMessage, FColor::Red);
		return;
	}

	if (index > 2) 
	{
		SetMaterial(2);
		return;
	}

	if (index + 1 > MaterialsUsed.Num())
	{
		SetMaterial(MaterialsUsed.Num() - 1);
		return;
	}

	TileMesh->SetMaterial(0, MaterialsUsed[index]);
}