// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/BoxComponent.h>
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/BillboardComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChopterLandingPointBase.generated.h"

UCLASS()
class CHOPTERCOMBAT_API AChopterLandingPointBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChopterLandingPointBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "Landing Component")
		UBoxComponent* LandingZone;

	UPROPERTY(VisibleAnywhere, Category = "Landing Component")
		UBillboardComponent* LandingBilboard;
	
	UPROPERTY(BlueprintReadOnly, Category = "Landing Component")
		FVector  LandingCenterPoint;


};
