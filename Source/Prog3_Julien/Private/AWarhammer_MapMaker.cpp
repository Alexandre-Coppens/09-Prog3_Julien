// Fill out your copyright notice in the Description page of Project Settings.


#include "AWarhammer_MapMaker.h"
#include "MyFunctionList.h"

#include "Kismet/GameplayStatics.h"
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
    EmptyTileList();
    CreateBaseMap();
    for (int i = 0; i < RiverNumber; i++)
    {
        CreateRiver();
    }
    for (int i = 0; i < PathNumber; i++)
    {
        CreatePath();
    }
}

void AWarhammer_MapMaker::EmptyTileList()
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

    FString DebugMessage = FString::Printf(TEXT("Empty execution took %f seconds (%f ms)"), DebugTotalTime, DebugTotalTime * 1000.0);
    MyFunctionList::DebugPrint(DebugMessage, FColor::Yellow);
}

void AWarhammer_MapMaker::DestroyAllTiles()
{
    double DebugStartTime = FPlatformTime::Seconds();

    TArray<AActor*> DeleteList;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWarhammer_Tile::StaticClass(), DeleteList);

    for (AActor* Tile : DeleteList)
    {
        Tile->Destroy();
    }
    TileArray.Empty();

    double DebugEndTime = FPlatformTime::Seconds();
    double DebugTotalTime = DebugEndTime - DebugStartTime;

    FString DebugMessage = FString::Printf(TEXT("Destroy execution took %f seconds (%f ms)"), DebugTotalTime, DebugTotalTime * 1000.0);
    MyFunctionList::DebugPrint(DebugMessage, FColor::Yellow);
}

void AWarhammer_MapMaker::Resize() 
{
    MapSize.X = FMath::Clamp(MapSize.X, 1, 50);
    MapSize.Y = FMath::Clamp(MapSize.Y, 1, 50);
    FVector newSize{MapSize.X * TileScale, MapSize.Y * TileScale, 3 };
    MapSizeDebug->SetWorldScale3D(newSize);
    BuildMap();
}

void AWarhammer_MapMaker::CreateBaseMap()
{
    MeterTileScale = TileScale * 100;
    if (!TileClass->IsValidLowLevelFast())
    {
        FString DebugMessage = FString::Printf(TEXT("Tile Class is not VALID!"));
        MyFunctionList::DebugPrint(DebugMessage, FColor::Red);
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
    FVector CurrentPosition{ ActorLocation.X - (MapSize.X * 0.5f) * MeterTileScale, ActorLocation.Y - (MapSize.Y * 0.5f) * MeterTileScale, 0 };
    
    FVector Location{ 0, 0, 0 };
    FRotator Rotation = FRotator::ZeroRotator;
    FVector Scale{ TileScale, TileScale, TileScale };
    FActorSpawnParameters SpawnInfo;
    
    double DebugStartTime = FPlatformTime::Seconds();

    for (int i = 0; i < MapSize.X; i++) {
        FRowArray row;
        for (int j = 0; j < MapSize.Y; j++) {
            Location = FVector( CurrentPosition.X + MeterTileScale * i + MeterTileScale * 0.5f,
                                CurrentPosition.Y + MeterTileScale * j + MeterTileScale * 0.5f,
                                ActorLocation.Z - MapSizeDebug->GetRelativeScale3D().Z * MeterTileScale * 0.5f);

            Scale.Z = TileScale + TileScale * GetHeightElevation(PerlinStartPos.X + Location.X, PerlinStartPos.Y + Location.Y, PerlinDist) * 0.5f;

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
    MyFunctionList::DebugPrint(DebugMessage, FColor::Yellow);
}

uint8 AWarhammer_MapMaker::GetHeightElevation(float X, float Y, float Frequency)
{
    float noise = FMath::PerlinNoise2D(FVector2D(X * Frequency, Y * Frequency));
    noise = (noise + 1) * 0.5f;
    if (noise < 0.475f) return 1;
    if (noise < 0.60f) return 2;
    return 3;
}

void AWarhammer_MapMaker::CreateRiver() 
{
    uint8 RiverX = ceilf(MapSize.X * 0.5f) + roundf(FMath::RandRange(MapSize.X * -0.125f, MapSize.X * 0.125f));
    uint8 RiverY = 0;

    TileArray[RiverX].RowArray[RiverY]->SetRiver();

    uint8 random;
    bool RiverEnded = false;
    for (int i = 0; i <= 1000 && !RiverEnded; i++)
    {
        random = roundf(FMath::RandRange(0,3));
        switch (random)
        {
        case 0:
            if (RiverX > 0) RiverX--;
            break;

        case 1:
            if (RiverX < MapSize.X - 1) RiverX++;
            break;

        default:
            RiverY++;
            break;
        }

        TileArray[RiverX].RowArray[RiverY]->SetRiver();

        if (RiverY == MapSize.Y - 1) RiverEnded = true;
    }
}

void AWarhammer_MapMaker::CreatePath()
{
    uint8 RiverX = ceilf(roundf(FMath::RandRange(MapSize.X * 0.1f, MapSize.X * 0.9f)));
    uint8 RiverY = 0;

    TileArray[RiverY].RowArray[RiverX]->SetPath();

    uint8 random;
    bool RiverEnded = false;
    for (int i = 0; i <= 1000 && !RiverEnded; i++)
    {
        random = roundf(FMath::RandRange(0, 7));
        switch (random)
        {
        case 0:
            if (RiverX > 0) RiverX--;
            break;

        case 1:
            if (RiverX < MapSize.X - 1) RiverX++;
            break;

        default:
            RiverY++;
            break;
        }

        TileArray[RiverY].RowArray[RiverX]->SetPath();

        if (RiverY == MapSize.X - 1) RiverEnded = true;
    }
}