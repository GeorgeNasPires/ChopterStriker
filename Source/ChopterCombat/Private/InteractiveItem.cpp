// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveItem.h"

// Sets default values
AInteractiveItem::AInteractiveItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InteractiveRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("InteractiveRootComponent"));
	DetectionArea = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractiveDetectionArea"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InteractiveMesh"));
	InteractionPoint = CreateDefaultSubobject<UBillboardComponent>(TEXT("InteractiveInteractionPoint"));

	RootComponent = InteractiveRootComponent;
	DetectionArea->SetupAttachment(InteractiveRootComponent);
	Mesh->SetupAttachment(InteractiveRootComponent);
	InteractionPoint->SetupAttachment(InteractiveRootComponent);

}

// Called when the game starts or when spawned
void AInteractiveItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractiveItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
ItemTypesEnum AInteractiveItem::GetInteractiveType_Implementation()
{
	return ItemType;
}

ItemTypesEnum AInteractiveItem::GetInteractiveTypePure()
{
	return ItemType;
}

FVector AInteractiveItem::GetInteractivePositionPure()
{
	return InteractionPoint->GetComponentLocation();
}

void AInteractiveItem::ActivatePhysics()
{
	Mesh->SetSimulatePhysics(true);
}



