// Fill out your copyright notice in the Description page of Project Settings.


#include "AWarhammer_MapMaker.h"
#include "MyFunctionList.h"
#include "Enum_TileType.h"

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
    SetDecoration();
}

void AWarhammer_MapMaker::EmptyTileList()
{
    double DebugStartTime = FPlatformTime::Seconds();

    for (FTileRowArray Row : TileArray)
    {
        for (AActor* Tile : Row.TileRowArray)
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

AWarhammer_Tile* AWarhammer_MapMaker::GetTileAt(int32 pos)
{
    return TileArray[pos / MapSize.X].TileRowArray[pos % MapSize.X];
}

bool AWarhammer_MapMaker::IsTileInTileCenter(int32 pos)
{
    for (FPosArray i : TileCenters)
    {
        for (int32 j : i.PosArray)
        {
            if (j == pos) return true;
        }
    }
    return false;
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

    for (int i = 0; i < MapSize.Y; i++) {
        FTileRowArray row;
        for (int j = 0; j < MapSize.X; j++) {
            Location = FVector( CurrentPosition.X + MeterTileScale * j + MeterTileScale * 0.5f,
                                CurrentPosition.Y + MeterTileScale * i + MeterTileScale * 0.5f,
                                ActorLocation.Z - MapSizeDebug->GetRelativeScale3D().Z * MeterTileScale * 0.5f);

            Scale.Z = TileScale + TileScale * GetHeightElevation(PerlinStartPos.X + Location.X, PerlinStartPos.Y + Location.Y, PerlinDist) * 0.5f;

            AWarhammer_Tile* NewTile = GetWorld()->SpawnActor<AWarhammer_Tile>(TileClass, Location, Rotation, SpawnInfo);
            
            if (NewTile)
            {
                NewTile->SetActorScale3D(Scale);
                row.TileRowArray.Add(NewTile);
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

//River is Top -> Bottom
void AWarhammer_MapMaker::CreateRiver() 
{
    uint8 RiverX = ceilf(MapSize.X * 0.5f) + roundf(FMath::RandRange(MapSize.X * -0.125f, MapSize.X * 0.125f));
    uint8 RiverY = 0;

    TileArray[RiverY].TileRowArray[RiverX]->SetRiver();

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

        TileArray[RiverY].TileRowArray[RiverX]->SetRiver();

        if (RiverY == MapSize.Y - 1) RiverEnded = true;
    }
}

void AWarhammer_MapMaker::CreatePath()
{
    FString DebugMessage;

    DebugMessage = FString::Printf(TEXT("Init All Positions"));
    MyFunctionList::DebugPrint(DebugMessage, FColor::Yellow);
    //Init all positions
    TArray<int32> positionArray;
    TArray<int32> secondaryArray;
    for (int i = 0; i < (MapSize.X * MapSize.Y); i++)
    {
        positionArray.Add(i);
    }
    secondaryArray = positionArray;

    DebugMessage = FString::Printf(TEXT("Find All Borders"));
    MyFunctionList::DebugPrint(DebugMessage, FColor::Yellow);
    //Find all borders
    TileCenters.Empty();
    FPosArray borderList;
    FPosArray SideList;
    for (int i = 0; i < positionArray.Num(); i++)
    {
        DebugMessage = FString::Printf(TEXT("Border check n° %i"), i);
        MyFunctionList::DebugPrint(DebugMessage, FColor::Green);
        if (IsTileOnBorder(i))
        {
            borderList.PosArray.Add(i);
            positionArray[i] = -1;
            GetTileAt(i)->BorderDist = 1;
        }
    }
    TileCenters.Add(borderList);
     
    DebugMessage = FString::Printf(TEXT("Expand Numbers"));
    MyFunctionList::DebugPrint(DebugMessage, FColor::Yellow);
    //Expand numbers
    int tileLeft = positionArray.Num() - TileCenters[0].PosArray.Num();
    int currentBorder = 2;
    ExpandingSize = 0;
    TArray<int32> tempArray;
    bestTiles.Empty();
    while (tileLeft != 0)
    {
        borderList.PosArray.Empty();
        for (int32 i : positionArray)
        {
            if (i != -1)
            {
                DebugMessage = FString::Printf(TEXT("Loop n° %i | Position = %i"), currentBorder, i);
                MyFunctionList::DebugPrint(DebugMessage, FColor::Cyan);
                if (IsNextTileNumbered(i))
                {
                    tempArray.Add(i);
                    positionArray[i] = -1;
                    GetTileAt(i)->BorderDist = currentBorder;
                    tileLeft--;
                }
            }
        }
        for (int32 j : tempArray)
        {
            borderList.PosArray.Add(j);
            ExpandingSize++;
        }
        TileCenters.Add(borderList);
        tempArray.Empty();
        currentBorder++;
    }

    DebugMessage = FString::Printf(TEXT("Show Best Positions"));
    MyFunctionList::DebugPrint(DebugMessage, FColor::Yellow);
    //Show Best Positions From best to worst
    int8 cluster = 0;
    for (int k = TileCenters.Num() - 1; k != 3; k--) {
        for (int32 checkPos : TileCenters[k].PosArray)
        {
            if (not GetTileAt(checkPos)->HasBeenChecked)
            {
                CheckTileAround(checkPos, k, 6, cluster);
            }
        }
        cluster += 1;
    }

    //Get Random Tile in the best Paths (1 per clusters )
    TArray<int32> bestPathArray;
    bestPathArray.Add(FMath::RandRange(0, MapSize.Y - 1) * MapSize.X);
    for (int i = 0; i < bestTiles.Num(); i++)
    {
        if (bestTiles[i].PosArray.Num() != 0)
        {
            bestPathArray.Add(bestTiles[i].PosArray[FMath::RandRange(0, bestTiles[i].PosArray.Num() - 1)]);
        }
    }
    bestPathArray.Add(FMath::RandRange(0, MapSize.Y - 1) * MapSize.X + MapSize.X -1);

    //Create Path with fast voxel algorithm
    float tMaxX, tMaxY, tDeltaX, tDeltaY;
    int stepX, stepY, X, Y, endX, endY, dX, dY;
    int tries;
    for (int j = 0; j < bestPathArray.Num() - 1; j++)
    {
        X = bestPathArray[j] % MapSize.X;
        Y = bestPathArray[j] / MapSize.X;
        endX = bestPathArray[j+1] % MapSize.X;
        endY = bestPathArray[j+1] / MapSize.X;
        
        TileArray[Y].TileRowArray[X]->SetPath();

        if (endX > X)stepX = 1;
        else if (endX < X) stepX = -1;
        else stepX = 0;

        if (endY > Y)stepY = 1;
        else if (endY < Y) stepY = -1;
        else stepY = 0;

        dX = endX - X;
        dY = endY - Y;

        if (dX < 0) { X = endX; Y = endY; }

        tDeltaX = dX == 0 ? 0 : abs(1.0f / dX);
        tDeltaY = dY==0 ? 0 : abs(1.0f/ dY);

        tMaxX = tDeltaX;
        tMaxY = tDeltaY;

        tries = 0;
        while ((X != endX || Y != endY) && tries < 500) {
            if (tMaxX < tMaxY) {
                tMaxX = tMaxX + tDeltaX;
                X = X + stepX;
            }
            else {
                tMaxY = tMaxY + tDeltaY;
                Y = Y + stepY;
            }
            tries++;
            TileArray[Y].TileRowArray[X]->SetPath();
        }
    }
}

bool AWarhammer_MapMaker::IsTileOnBorder(int32 tilePlace)
{
    ETileType tileType = GetTileAt(tilePlace)->TileType;

    int row = tilePlace / MapSize.X;
    int col = tilePlace % MapSize.X;

    //Try up
    if (row > 0)
    {
        if (GetTileAt(tilePlace  - MapSize.X)->TileType != tileType)
        {
            return true;
        }
    }

    //Try down
    if (row < MapSize.Y - 1)
    {
        if (GetTileAt(tilePlace + MapSize.X)->TileType != tileType)
        {
            return true;
        }
    }

    //Try left
    if (col > 0)
    {
        if (GetTileAt(tilePlace - 1)->TileType != tileType)
        {
            return true;
        }
    }

    //Try right
    if (col < MapSize.X - 1)
    {
        if (GetTileAt(tilePlace + 1)->TileType != tileType)
        {
            return true;
        }
    }

    return false;
}

bool AWarhammer_MapMaker::IsNextTileNumbered(int32 tilePlace)
{
    int row = tilePlace / MapSize.X;
    int col = tilePlace % MapSize.X;
    int i = tilePlace;

    //Try up
    if (row > 0)
    {
        if (IsTileInTileCenter(i - MapSize.X))
        {
            return true;
        }
    }
    else
    {
        return true;
    }

    //Try down
    if (row < MapSize.Y - 1)
    {
        if (IsTileInTileCenter(i + MapSize.X))
        {

            return true;
        }
    }
    else
    {
        return true;
    }

    //Try left
    if (col > 0)
    {
        if (IsTileInTileCenter(i - 1))
        {
            return true;
        }
    }
    else
    {
        return true;
    }

    //Try right
    if (col < MapSize.X - 1)
    {
        if (IsTileInTileCenter(i + 1))
        {
            return true;
        }
    }
    else
    {
        return true;
    }

    return false;
}


void AWarhammer_MapMaker::CheckTileAround(int32 tilePlace, int8 best, int8 repetitionLeft, int8 currentCluster)
{
    AWarhammer_Tile* tile = GetTileAt(tilePlace);

    if (tile->HasBeenChecked) return;

    FString DebugMessage = FString::Printf(TEXT("Border Dist = %i"), tile->BorderDist);
    MyFunctionList::DebugPrint(DebugMessage, FColor::Yellow);

    int row = tilePlace / MapSize.X;
    int col = tilePlace % MapSize.X;
    int i = tilePlace;

    tile->HasBeenChecked = true;

    if (tile->BorderDist == best )
    {
        while (bestTiles.Num() <= currentCluster)
        {
            bestTiles.Add(FPosArray());
        }
        bestTiles[currentCluster].PosArray.Add(tilePlace);
    }

    if (repetitionLeft == 0) return;

    //Try up
    if (row > 0)
    {
        CheckTileAround(i - MapSize.X, best, repetitionLeft - 1, currentCluster);
    }

    //Try down
    if (row < MapSize.Y - 1)
    {
        CheckTileAround(i + MapSize.X, best, repetitionLeft - 1, currentCluster);
    }

    //Try left
    if (col > 0)
    {
        CheckTileAround(i - 1, best, repetitionLeft - 1, currentCluster);
    }

    //Try right
    if (col < MapSize.X - 1)
    {
        CheckTileAround(i + 1, best, repetitionLeft - 1, currentCluster);
    }

    return;
}

void AWarhammer_MapMaker::SetDecoration()
{
    FString DebugMessage = FString::Printf(TEXT("Putting Decorations"));
    MyFunctionList::DebugPrint(DebugMessage, FColor::Yellow);

    int tilePlace;
    int row;
    int col;
    AWarhammer_Tile* tileReference = nullptr;
    DebugMessage = FString::Printf(TEXT("Creating Forest"));
    MyFunctionList::DebugPrint(DebugMessage, FColor::Yellow);
    for (int i = 0; i < ForestClumps; i++)
    {
        tileReference = nullptr;

        while (tileReference == nullptr)
        {
            row = FMath::RandRange(0, (int)(MapSize.X * 0.5f));
            col = FMath::RandRange(0, MapSize.Y - 1);
            tilePlace = col * MapSize.X + row;
            DebugMessage = FString::Printf(TEXT("DECORATION::Col = %i; Row = %i"), col, row);
            MyFunctionList::DebugPrint(DebugMessage, FColor::Yellow);
            tileReference = GetTileAt(tilePlace);
            if (tileReference->TileType != ETileType::Low && tileReference->TileType != ETileType::Medium && tileReference->TileType != ETileType::High)
            {
                tileReference = nullptr;
            } 
        }

        SetForestAt(tilePlace, FMath::FRandRange(1.0f, 3.0f));
    }

    DebugMessage = FString::Printf(TEXT("Creating City"));
    MyFunctionList::DebugPrint(DebugMessage, FColor::Yellow);
    for (int i = 0; i < CityClumps; i++)
    {
        tileReference = nullptr;

        while (tileReference == nullptr)
        {
            row = FMath::RandRange(0, (int)(MapSize.X * 0.5f));
            col = FMath::RandRange(1, (int)(MapSize.Y));
            tilePlace = col * MapSize.X - row;
            tileReference = GetTileAt(tilePlace);
            if (tileReference->TileType != ETileType::Low && tileReference->TileType != ETileType::Medium && tileReference->TileType != ETileType::High)
            {
                tileReference = nullptr;
            }
        }

        SetCityAt(tilePlace, FMath::FRandRange(1.5f, 4.5f), 0);
    }
}

void AWarhammer_MapMaker::SetForestAt(int32 tilePlace, float height)
{
    AWarhammer_Tile* tile = GetTileAt(tilePlace);
    ETileType tileType = tile->TileType;

    int row = tilePlace / MapSize.X;
    int col = tilePlace % MapSize.X;

    FString DebugMessage = FString::Printf(TEXT("DECORATION::Col = %i; Row = %i"), col, row);
    MyFunctionList::DebugPrint(DebugMessage, FColor::Yellow);

    if (tileType != ETileType::Low && tileType != ETileType::Medium && tileType != ETileType::High)
    {
        return;
    }

    tile->SetForest(height);
    float rand;

    //Try up
    if (row > 0)
    {
        rand = FMath::FRand();
        if (rand > RandomStep) SetForestAt(tilePlace - MapSize.X, height);
    }

    //Try down
    if (row < MapSize.Y - 1)
    {
        rand = FMath::FRand();
        if (rand > RandomStep) SetForestAt(tilePlace + MapSize.X, height);
    }

    //Try left
    if (col > 0)
    {
        rand = FMath::FRand();
        if (rand > RandomStep) SetForestAt(tilePlace - 1, height);
    }

    //Try right
    if (col < MapSize.X - 1)
    {
        rand = FMath::FRand();
        if (rand > RandomStep) SetForestAt(tilePlace + 1, height);
    }

    return;
}

void AWarhammer_MapMaker::SetCityAt(int32 tilePlace, float height, int8 direction)
{
    //0 = Center; 1 = North; 2 = South; 3 = West; 4 = East
    AWarhammer_Tile* tile = GetTileAt(tilePlace);
    ETileType tileType = tile->TileType;

    int row = tilePlace / MapSize.X;
    int col = tilePlace % MapSize.X;

    if (tileType != ETileType::Low && tileType != ETileType::Medium && tileType != ETileType::High)
    {
        return;
    }

    tile->SetCity(height);
    float rand;

    //Try up
    if (row > 0 && (direction == 0 || direction == 1))
    {
        rand = (direction == 0 ? 1 : FMath::FRand());
        if(rand > RandomStep) SetCityAt(tilePlace - MapSize.X, height, 1);
    }

    //Try down
    if (row < MapSize.Y - 1 && (direction == 0 || direction == 2))
    {
        rand = (direction == 0 ? 1 : FMath::FRand());
        if (rand > RandomStep) SetCityAt(tilePlace + MapSize.X, height, 2);
    }

    //Try left
    if (col > 0 && (direction == 0 || direction == 3))
    {
        rand = (direction == 0 ? 1 : FMath::FRand());
        if (rand > RandomStep) SetCityAt(tilePlace - 1, height, 3);
    }

    //Try right
    if (col < MapSize.X - 1 && (direction == 0 || direction == 4))
    {
        rand = (direction == 0 ? 1 : FMath::FRand());
        if (rand > RandomStep) SetCityAt(tilePlace + 1, height, 4);
    }

    return;
}
