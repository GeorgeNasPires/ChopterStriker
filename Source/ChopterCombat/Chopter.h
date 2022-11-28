// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include "Components/TimelineComponent.h"
#include "ItemTypesEnum.h"
#include "Public/ChopterInteractInterface.h"
#include "Public/InteractiveItem.h"
#include <Kismet/KismetMathLibrary.h>
#include "ChopterStats.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Chopter.generated.h"

UCLASS()
class CHOPTERCOMBAT_API AChopter : public APawn, public IChopterInteractInterface
{
	GENERATED_BODY()

		static const float MAX_ROTOR_ROTATION;

public:
	// Sets default values for this pawn's properties
	AChopter();

private:
	float RotorVelocity;
	bool bDoLineTrace;
	bool bWasLanding = false;
	bool bCarryingSupplies;

	FVector InteractiveLocation;

	float DistanceToItemBellow;
	AInteractiveItem* InteractingItem;
	ItemTypesEnum ItemDetected;

	FVector MovimentDirection;

	FVector LandscapePosition;


	FRotator LastDirection;
	FTimerHandle thGoToPoint;
	FTimerDelegate thdGoToPointDelegate;

	FTimerHandle thCable;
	FTimerDelegate thdCableDelegate;

	FTimerHandle TimerHandleClock;

	FRotator TopRotorRotation;
	FRotator TailRotation;

	FTimeline tlCable;
	float TimeLineValue;
	float CurveFloatValue;


	UCurveFloat* CurrentCurve;



	void InputStartRotors();
	void InputTakeOff(float deltatime);
	void Landing(float deltatime);
	FHitResult GroundLineTrace();
	void RotorTimer(bool isOn);
	void ChopterMove();
	void LineTraceUnderDetector();
	void LineTraceToInteract();

	void LineTraceTarrainPosition();

	void InputGoToPoint(void* action);
	void Refuel();
	void CarrySupply(FAttachmentTransformRules rule);
	void DeliverSupply();
	void RescueRefugees();
	void DisembarkRefugees();
	void SetAnchorAnimation(UCurveFloat* curve, bool isReverse);
	void FuelConsumption();
	void ChangeStat(ChopterStats stat);

	void ChoseInteraction(ItemTypesEnum Item);
	void ChopterDestroy(DestroyedType type);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "InteractiveItem")
		void OnInteractiveItemResponse(ItemTypesEnum itemType);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "InteractiveItem")
		void SpawnChopterFaller(FRotator bodyRotator, FRotator topRotator, FRotator tailRotator);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "InteractiveItem")
		void OnChopterExplode();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "InteractiveItem")
		void OnStatChanged(ChopterStats Stat);

	UFUNCTION(BlueprintCallable, Category = "InteractiveItem")
		void InteractionAction(ItemTypesEnum Item);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "InteractiveItem")
		FVector GetLocationInTerrain();

	UFUNCTION(BlueprintPure, BlueprintPure, Category = "InteractiveItem")
		bool IsCarrySupply();

	UFUNCTION(BlueprintCallable, Category = "InteractiveItem")
		void ChopterDamage(float damage);

	UFUNCTION()
		void GoToPoint(ItemTypesEnum itemType);

	//UFUNCTION()
	//	void PrintTest(FString msg);

	UFUNCTION()
		void PickUpItem();

	UFUNCTION()
		void AnchorItemTaken();

	UFUNCTION()
		void AnchorFinish();


	UPROPERTY(EditAnywhere)
		UCurveFloat* AnchorRefuelCurve;

	UPROPERTY(EditAnywhere)
		UCurveFloat* AnchorSupplyCurve;

	UPROPERTY(EditAnywhere, Category = "Chopter Controller")
		float MovimentSpeed = 1000;

	UPROPERTY(EditAnywhere, Category = "Chopter Controller")
		float Altitude = 1000;

	UPROPERTY(EditAnywhere, Category = "Chopter Controller")
		float FuelCons = 0.00002;

	UPROPERTY(EditAnywhere, Category = "Chopter Controller")
		bool InteractiveDetected = false;

	UPROPERTY(BlueprintReadWrite, Category = "Chopter Controller")
		float Fuel;


	UPROPERTY(BlueprintReadWrite, Category = "Chopter Controller")
		float Life;

	UPROPERTY(BlueprintReadWrite, Category = "Chopter Controller")
		FVector Speed;

	UPROPERTY(VisibleAnywhere, Category = "Chopter Component")
		USceneComponent* ChopterRootComponent;

	UPROPERTY(VisibleAnywhere, Category = "Chopter Component")
		USceneComponent* ChopterPitchRoll;

	UPROPERTY(VisibleAnywhere, Category = "Chopter Component")
		USceneComponent* ChopterRoot;

	UPROPERTY(VisibleAnywhere, Category = "Chopter Component")
		USceneComponent* ChopterLandingGearHeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chopter Component")
		USceneComponent* ChopterYaw;

	UPROPERTY(VisibleAnywhere, Category = "Chopter Component")
		UStaticMeshComponent* ChopterBody;

	UPROPERTY(VisibleAnywhere, Category = "Chopter Component")
		UStaticMeshComponent* ChopterTopRotor;

	UPROPERTY(VisibleAnywhere, Category = "Chopter Component")
		UStaticMeshComponent* ChopterTailRotor;

	UPROPERTY(VisibleAnywhere, Category = "Chopter Component")
		USpringArmComponent* ChopterSpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Chopter Component")
		UCameraComponent* ChopterCamera;

	UPROPERTY(VisibleAnywhere, Category = "Chopter Component")
		UBoxComponent* AchorInteractive;

	UPROPERTY(VisibleAnywhere, Category = "Chopter Component")
		UStaticMeshComponent* AchorCable;

	UPROPERTY(VisibleAnywhere, Category = "Chopter Component")
		UStaticMeshComponent* AchorMagneticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<ChopterStats> ChopterStat;

	// Herdado por meio de IChopterInteractInterface
	virtual ItemTypesEnum GetInteractiveTypePure() override;

	// Herdado por meio de IChopterInteractInterface
	virtual FVector GetInteractivePositionPure() override;

	// Herdado por meio de IChopterInteractInterface
	virtual void ActivatePhysics() override;
};
