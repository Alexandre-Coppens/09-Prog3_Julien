// Fill out your copyright notice in the Description page of Project Settings.


#include "BP_Warhammer_MapMaker.h"

#include "Components/StaticMeshComponent.h"

// Sets default values
ABP_Warhammer_MapMaker::ABP_Warhammer_MapMaker()
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
    
    Scale = 0.1f;

}

// Called when the game starts or when spawned
void ABP_Warhammer_MapMaker::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABP_Warhammer_MapMaker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABP_Warhammer_MapMaker::MyEditorFunction()
{
    // Example: Print to the output log
    UE_LOG(LogTemp, Warning, TEXT("MyEditorFunction was called in the editor!"));

    // Or show a notification in the editor
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Called in Editor!"));
    }
}

#if WITH_EDITOR
void ABP_Warhammer_MapMaker::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    FName PropertyName = (PropertyChangedEvent.Property != nullptr)
        ? PropertyChangedEvent.Property->GetFName()
        : NAME_None;

    if (PropertyName == GET_MEMBER_NAME_CHECKED(FIntVector2, X) || PropertyName == GET_MEMBER_NAME_CHECKED(FIntVector2, Y))
    {
        MyEditorFunction();
    }
}
#endif

// Check if value been modified in editor
//void ABP_Warhammer_MapMaker::OnConstruction(const FTransform& Transform)
//{
//    Super::OnConstruction(Transform);
//
//    MyEditorFunction();
//}