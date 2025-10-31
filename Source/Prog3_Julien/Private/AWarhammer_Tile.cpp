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

	if (!Parent)
	{
		FString DebugMessage = FString::Printf(TEXT("Tile has no Parent Actor"));
		MyFunctionList::DebugPrint(DebugMessage);
		return;
	}

	if (GetActorLocation().Z < Parent->GetActorLocation().Z)
	{
		SetMaterial(0);
	}
	else if (GetActorLocation().Z == Parent->GetActorLocation().Z)
	{
		SetMaterial(1);
	}
	else
	{
		SetMaterial(2);
	}
}

void AWarhammer_Tile::SetRiver(float ZPos)
{
	FVector ActorLocation = GetActorLocation();
	SetActorLocation(FVector(ActorLocation.X, ActorLocation.Y, ZPos));

	if (!RiverMaterial) 
	{
		FString DebugMessage = FString::Printf(TEXT("Please Add a River Material"));
		MyFunctionList::DebugPrint(DebugMessage);
	}
	TileMesh->SetMaterial(0, RiverMaterial);
}

void AWarhammer_Tile::SetMaterial(uint8 index)
{
	if (MaterialsUsed.Num() == 0)
	{
		FString DebugMessage = FString::Printf(TEXT("Please Add at least 1 Material to Tile"));
		MyFunctionList::DebugPrint(DebugMessage);
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
