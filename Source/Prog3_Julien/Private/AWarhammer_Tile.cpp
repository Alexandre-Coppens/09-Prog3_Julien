// Fill out your copyright notice in the Description page of Project Settings.


#include "AWarhammer_Tile.h"

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
		if (GEngine)
		{
			FString DebugMessage = FString::Printf(TEXT("Tile has no Parent Actor"));
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, DebugMessage);
		}
		UE_LOG(LogTemp, Log, TEXT("Tile has no Parent Actor"));

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

void AWarhammer_Tile::SetMaterial(uint8 index)
{
	if (MaterialsUsed.Num() == 0)
	{
		if (GEngine)
		{
			FString DebugMessage = FString::Printf(TEXT("Please Add at least 1 Material to Tile"));
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, DebugMessage);
		}
		UE_LOG(LogTemp, Log, TEXT("Please Add at least 1 Material to Tile"));

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

void AWarhammer_Tile::CreateRiver()
{
}

