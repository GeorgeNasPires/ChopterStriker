// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/BoxComponent.h"
#include "ChopterInteractInterface.h"
#include "Components/BillboardComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveItem.generated.h"

UCLASS()
class CHOPTERCOMBAT_API AInteractiveItem : public AActor, public IChopterInteractInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInteractiveItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	ItemTypesEnum GetInteractiveType_Implementation() override;

	virtual ItemTypesEnum GetInteractiveTypePure() override;

	virtual FVector GetInteractivePositionPure() override;

	virtual void ActivatePhysics() override;
	
	UPROPERTY(VisibleAnywhere, Category = "InteractiveItem")
		USceneComponent* InteractiveRootComponent;

	UPROPERTY(VisibleAnywhere, Category = "InteractiveItem")
		UBoxComponent* DetectionArea;

	UPROPERTY(VisibleAnywhere, Category = "InteractiveItem")
		UStaticMeshComponent* Mesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InteractiveItem")
		ItemTypesEnum ItemType;

	UPROPERTY(VisibleAnywhere, Category = "InteractiveItem")
		UBillboardComponent* InteractionPoint;



};