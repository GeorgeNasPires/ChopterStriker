// Fill out your copyright notice in the Description page of Project Settings.


#include "ChopterLandingPointBase.h"

// Sets default values
AChopterLandingPointBase::AChopterLandingPointBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LandingZone = CreateDefaultSubobject<UBoxComponent>(TEXT("LandingZone"));
	LandingBilboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("LandingBilboard"));
	LandingBilboard->SetHiddenInGame(false, true);

	LandingZone->SetBoxExtent(FVector(400, 400, 10));
	LandingBilboard->SetRelativeLocation(LandingZone->GetRelativeLocation());

	RootComponent = LandingBilboard;
	LandingCenterPoint = LandingZone->GetComponentLocation();
}

// Called when the game starts or when spawned
void AChopterLandingPointBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChopterLandingPointBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

