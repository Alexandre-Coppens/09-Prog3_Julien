// Fill out your copyright notice in the Description page of Project Settings.


#include "AWarhammer_Tile.h"

// Sets default values
AWarhammer_Tile::AWarhammer_Tile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	MapSizeDebug = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MapSizeDebug->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void AWarhammer_Tile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWarhammer_Tile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

