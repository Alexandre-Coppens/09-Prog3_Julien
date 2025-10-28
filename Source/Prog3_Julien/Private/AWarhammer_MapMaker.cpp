// Fill out your copyright notice in the Description page of Project Settings.


#include "AWarhammer_MapMaker.h"

#include "Components/StaticMeshComponent.h"

// Sets default values
AWarhammer_MapMaker::AWarhammer_MapMaker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    MapSizeDebug = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    MapSizeDebug->SetupAttachment(Root);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("Engine/Common/BasicShapes/Cube"));
    if (CubeMesh.Succeeded()) {
        MapSizeDebug->SetStaticMesh(CubeMesh.Object);
    }
    
    TileScale = 0.1f;
    MapSize = FIntVector2(20,10);
}

// Called when the game starts or when spawned
void AWarhammer_MapMaker::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWarhammer_MapMaker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Check if value been modified in editor
#if WITH_EDITOR
void AWarhammer_MapMaker::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    FName PropertyName = (PropertyChangedEvent.Property != nullptr)
        ? PropertyChangedEvent.Property->GetFName()
        : NAME_None;

    if (PropertyName == GET_MEMBER_NAME_CHECKED(FIntVector2, X) || PropertyName == GET_MEMBER_NAME_CHECKED(FIntVector2, Y))
    {
        Resize();
    }
}
#endif

void AWarhammer_MapMaker::BuildMap()
{
    DestroyAllTiles();
    CreateBaseMap();
}

void AWarhammer_MapMaker::DestroyAllTiles()
{
    for (AActor* Tile : TileArray)
    {
        if (Tile)
        {
            Tile->Destroy();
        }
    }
    TileArray.Empty();
}

void AWarhammer_MapMaker::Resize() 
{
    FVector newSize{MapSize.X * TileScale * 0.1f, MapSize.Y * TileScale * 0.1f, 5 * 0.1f };
    MapSizeDebug->SetWorldScale3D(newSize);
    BuildMap();
}

void AWarhammer_MapMaker::CreateBaseMap()
{
    if (!TileClass->IsValidLowLevelFast())
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Tile Class is not VALID!"));
        return;
    }

    FVector ActorLocation = GetActorLocation();
    FVector CurrentPosition{ ActorLocation.X - (MapSize.X * 0.5f) * TileScale * 100, ActorLocation.Y - (MapSize.Y * 0.5f) * TileScale * 100, 0 };
    
    FVector Location{ 0, 0, 0 };
    FRotator Rotation = FRotator::ZeroRotator;
    FVector Scale{ TileScale, TileScale, 1 };
    FActorSpawnParameters SpawnInfo;
    
    for (int i = 0; i < MapSize.X; i++) {
        for (int j = 0; j < MapSize.Y; j++) {
            Location = FVector( CurrentPosition.X + TileScale * i * 100 + TileScale * 0.5f * 100,
                                CurrentPosition.Y + TileScale * j * 100 + TileScale * 0.5f * 100,
                                ActorLocation.Z);

            AWarhammer_Tile* NewTile = GetWorld()->SpawnActor<AWarhammer_Tile>(TileClass, Location, Rotation, SpawnInfo);
            
            if (NewTile)
            {
                NewTile->SetActorScale3D(Scale);
                TileArray.Add(NewTile);
                NewTile->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
            }
        }
    }
}
