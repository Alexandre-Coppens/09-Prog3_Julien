// Fill out your copyright notice in the Description page of Project Settings.


#include "AWarhammer_MapMaker.h"
#include "MyFunctionList.h"

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
    
    TileScale = 0.1f;
    MapSize = FIntVector2(20,10);
    NoiseFrequency = 1;
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
    CreateRiver();
}

void AWarhammer_MapMaker::DestroyAllTiles()
{
    double DebugStartTime = FPlatformTime::Seconds();

    for (FRowArray Row : TileArray)
    {
        for (AActor* Tile : Row.RowArray)
        {
            if (Tile)
            {
                Tile->Destroy();
            }
        }
    }
    TileArray.Empty();

    double DebugEndTime = FPlatformTime::Seconds();
    double DebugTotalTime = DebugEndTime - DebugStartTime;

    FString DebugMessage = FString::Printf(TEXT("Destroy execution took %f seconds (%f ms)"), DebugTotalTime, DebugTotalTime * 1000.0);
    MyFunctionList::DebugPrint(DebugMessage);
}

void AWarhammer_MapMaker::Resize() 
{
    MapSize.X = FMath::Clamp(MapSize.X, 1, 50);
    MapSize.Y = FMath::Clamp(MapSize.Y, 1, 50);
    FVector newSize{MapSize.X * TileScale, MapSize.Y * TileScale, 2 };
    MapSizeDebug->SetWorldScale3D(newSize);
    BuildMap();
}

void AWarhammer_MapMaker::CreateBaseMap()
{
    if (!TileClass->IsValidLowLevelFast())
    {
        FString DebugMessage = FString::Printf(TEXT("Tile Class is not VALID!"));
        MyFunctionList::DebugPrint(DebugMessage);
    }

    FVector2D PerlinStartPos{ FMath::RandRange(0.0, 99999.0), FMath::RandRange(0.0, 99999.0)};

    float PerlinDist;
    switch (MapScale)
    {
    case EEnvironmentSize::Near:
        PerlinDist = 0.00025f;
        break;
    case EEnvironmentSize::Far:
        PerlinDist = 0.00075f;
        break;
    }

    FVector ActorLocation = GetActorLocation();
    FVector CurrentPosition{ ActorLocation.X - (MapSize.X * 0.5f) * TileScale * 100, ActorLocation.Y - (MapSize.Y * 0.5f) * TileScale * 100, 0 };
    
    FVector Location{ 0, 0, 0 };
    FRotator Rotation = FRotator::ZeroRotator;
    FVector Scale{ TileScale, TileScale, TileScale };
    FActorSpawnParameters SpawnInfo;
    
    double DebugStartTime = FPlatformTime::Seconds();

    for (int i = 0; i < MapSize.X; i++) {
        FRowArray row;
        for (int j = 0; j < MapSize.Y; j++) {
            Location = FVector( CurrentPosition.X + TileScale * i * 100 + TileScale * 0.5f * 100,
                                CurrentPosition.Y + TileScale * j * 100 + TileScale * 0.5f * 100,
                                ActorLocation.Z - TileScale * 50);

            Location.Z += GetHeightElevation(PerlinStartPos.X + Location.X, PerlinStartPos.Y + Location.Y, PerlinDist) * TileScale * 50;

            AWarhammer_Tile* NewTile = GetWorld()->SpawnActor<AWarhammer_Tile>(TileClass, Location, Rotation, SpawnInfo);
            
            if (NewTile)
            {
                NewTile->SetActorScale3D(Scale);
                row.RowArray.Add(NewTile);
                NewTile->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
                NewTile->InitTile();
            }
            UE_LOG(LogTemp, Log, TEXT("Tile n° %i of %i"), i * MapSize.X + j, MapSize.X * MapSize.Y);
        }
        TileArray.Add(row);
    }

    double DebugEndTime = FPlatformTime::Seconds();
    double DebugTotalTime = DebugEndTime - DebugStartTime;

    FString DebugMessage = FString::Printf(TEXT("Build execution took %f seconds (%f ms)"), DebugTotalTime, DebugTotalTime * 1000.0);
    MyFunctionList::DebugPrint(DebugMessage);
}

uint8 AWarhammer_MapMaker::GetHeightElevation(float X, float Y, float Frequency)
{
    float noise = FMath::PerlinNoise2D(FVector2D(X * Frequency, Y * Frequency));
    noise = (noise + 1) * 0.5f;
    if (noise < 0.475f) return 0;
    if (noise < 0.60f) return 1;
    return 2;
}

void AWarhammer_MapMaker::CreateRiver() 
{
    uint8 RiverX = ceilf(MapSize.X * 0.5f) + roundf(FMath::RandRange(MapSize.X * -0.125f, MapSize.X * 0.125f));
    uint8 RiverY = 0;
    float RiverZ = GetActorLocation().Z - TileScale * 100;

    for (int i = 0; i < MapSize.Y; i++)
    {
        TileArray[RiverX].RowArray[i]->SetRiver(RiverZ - TileScale * 50 * i);
    }
}