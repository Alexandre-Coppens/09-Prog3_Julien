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
    MapSize.X = FMath::Clamp(MapSize.X, 1, 50);
    MapSize.Y = FMath::Clamp(MapSize.Y, 1, 50);
    FVector newSize{MapSize.X * TileScale, MapSize.Y * TileScale, 5 };
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
    FVector Scale{ TileScale, TileScale, 1 };
    FActorSpawnParameters SpawnInfo;
    
    double DebugStartTime = FPlatformTime::Seconds();

    for (int i = 0; i < MapSize.X; i++) {
        for (int j = 0; j < MapSize.Y; j++) {
            Location = FVector( CurrentPosition.X + TileScale * i * 100 + TileScale * 0.5f * 100,
                                CurrentPosition.Y + TileScale * j * 100 + TileScale * 0.5f * 100,
                                ActorLocation.Z - TileScale * 50);

            Location.Z += GetHeightElevation(PerlinStartPos.X + Location.X, PerlinStartPos.Y + Location.Y, PerlinDist) * TileScale * 50;

            AWarhammer_Tile* NewTile = GetWorld()->SpawnActor<AWarhammer_Tile>(TileClass, Location, Rotation, SpawnInfo);
            
            if (NewTile)
            {
                NewTile->SetActorScale3D(Scale);
                TileArray.Add(NewTile);
                NewTile->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
            }

            if (GEngine)
            {
                FString DebugMessage = FString::Printf(TEXT("Tile n° %i of %i"), i * MapSize.X + j, MapSize.X * MapSize.Y);
                GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, DebugMessage);
            }
            UE_LOG(LogTemp, Log, TEXT("Tile n° %i of %i"), i * MapSize.X + j, MapSize.X + MapSize.Y);
        }
    }

    double DebugEndTime = FPlatformTime::Seconds();
    double DebugTotalTime = DebugEndTime - DebugStartTime;
    if (GEngine)
    {
        FString DebugMessage = FString::Printf(TEXT("Execution took %f seconds (%f ms)"), DebugTotalTime, DebugTotalTime * 1000.0);
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, DebugMessage);
    }
    UE_LOG(LogTemp, Log, TEXT("Execution took %f seconds (%f ms)"), DebugTotalTime, DebugTotalTime * 1000.0);
}

uint8 AWarhammer_MapMaker::GetHeightElevation(float X, float Y, float Frequency)
{
    float noise = FMath::PerlinNoise2D(FVector2D(X * Frequency, Y * Frequency)) + 0.5f;
    noise = FMath::Clamp(noise, 0, 1);
    if (noise < 0.33f) return 0;
    if (noise < 0.66f) return 1;
    return 2;
}
