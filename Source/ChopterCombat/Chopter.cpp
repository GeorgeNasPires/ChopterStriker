// Fill out your copyright notice in the Description page of Project Settings.


#include "Chopter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"
//#include <Editor/UnrealEd/Public/EditorCategoryUtils.h>


const float AChopter::MAX_ROTOR_ROTATION = 700.0f;

// Sets default values
AChopter::AChopter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	ChopterRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ChopterRoot"));
	ChopterRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ChopterRootComponent"));
	ChopterPitchRoll = CreateDefaultSubobject<USceneComponent>(TEXT("ChopterPitchRoll"));
	ChopterYaw = CreateDefaultSubobject<USceneComponent>(TEXT("ChopterYaw"));
	ChopterBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChopterBody"));
	ChopterTopRotor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChopterTopRotor"));
	ChopterTailRotor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChopterTailRotor"));
	ChopterCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ChopterCamera"));
	ChopterSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("ChopterSpringArm"));
	ChopterLandingGearHeight =  CreateDefaultSubobject<USceneComponent>(TEXT("ChopterLandingGearHeight"));

	AchorCable = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cable"));
	AchorInteractive = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractiveCube"));
	AchorMagneticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MagneticMesh"));

	RootComponent = ChopterRoot;
	ChopterRootComponent->SetupAttachment(RootComponent);
	ChopterPitchRoll->SetupAttachment(RootComponent);
	ChopterYaw->SetupAttachment(ChopterPitchRoll);
	ChopterSpringArm->SetupAttachment(ChopterRootComponent);
	ChopterCamera->SetupAttachment(ChopterSpringArm, USpringArmComponent::SocketName);
	ChopterBody->SetupAttachment(ChopterYaw);
	ChopterTopRotor->SetupAttachment(ChopterPitchRoll);
	ChopterTailRotor->SetupAttachment(ChopterBody);
	ChopterLandingGearHeight->SetupAttachment(ChopterBody);

	AchorCable->SetupAttachment(ChopterRootComponent);
	AchorInteractive->SetupAttachment(ChopterRootComponent);
	AchorMagneticMesh->SetupAttachment(AchorInteractive);

	ChopterSpringArm->TargetArmLength = 1300.0f;
	ChopterSpringArm->bEnableCameraLag = true;
	ChopterSpringArm->CameraLagSpeed = 6.0f;
	ChopterSpringArm->bDrawDebugLagMarkers = false;
	ChopterSpringArm->bDoCollisionTest = false;
	ChopterSpringArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));

	InteractiveLocation = FVector(0, 0, 0);
	TopRotorRotation = FRotator(0, 180.0f, 0);
	TailRotation = FRotator(0, 0, 180.0f);

	ChopterStat = OFF;

	AchorMagneticMesh->SetVisibility(false);

	bDoLineTrace = true;
	InteractingItem = nullptr;
	bCarryingSupplies = false;

	Fuel = 1.0f;
	Life = 1.0f;

}

// Called when the game starts or when spawned
void AChopter::BeginPlay()
{
	Super::BeginPlay();

	if (AnchorRefuelCurve) {
		CurrentCurve = AnchorRefuelCurve;

		FOnTimelineFloat AnchorCallback;
		FOnTimelineEventStatic TimelineFinishedCallback;
		FOnTimelineEvent AnchorEvent;

		AnchorCallback.BindUFunction(this, FName("PickUpItem"));
		TimelineFinishedCallback.BindUFunction(this, FName{ TEXT("AnchorFinish") });
		AnchorEvent.BindUFunction(this, FName("AnchorItemTaken"));
		tlCable.AddInterpFloat(CurrentCurve, AnchorCallback, FName("actionCurve"));
		tlCable.SetTimelineFinishedFunc(TimelineFinishedCallback);
		tlCable.AddEvent(1.5f, AnchorEvent);
	}
	
}

// Called every frame
void AChopter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	tlCable.TickTimeline(DeltaTime);

	switch (ChopterStat)
	{
	case FLING:
		AChopter::ChopterMove();
		break;
	case START_ENGINE:
		AChopter::RotorTimer(true);
		break;
	case TAKEOFF:
		AChopter::InputTakeOff(DeltaTime);
		break;
	case LANDING:
		AChopter::Landing(DeltaTime);
		break;
	case DAMAGED:
		break;
	case DESTROYED:
		break;
	case OFF:
		AChopter::RotorTimer(RotorVelocity > 0);
		break;
	case GOTOPOINT:
		//GoToPoint(DeltaTime);
		break;
	}

	ChopterTopRotor->AddRelativeRotation(TopRotorRotation * DeltaTime * RotorVelocity);
	ChopterTailRotor->AddRelativeRotation(TailRotation * DeltaTime * RotorVelocity);

	AChopter::LineTraceUnderDetector();
	if (ChopterStat != DESTROYED)
		AChopter::FuelConsumption();

	AChopter::LineTraceTarrainPosition();

}

// Called to bind functionality to input
void AChopter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForwardBack");
	PlayerInputComponent->BindAxis("MoveLeftRight");
	PlayerInputComponent->BindAction("InputStartEngine", IE_Pressed, this, &AChopter::InputStartRotors);
	//PlayerInputComponent->BindAction("InputTakeOff", IE_Pressed, this, &AChopter::InputTakeOff);
	//PlayerInputComponent->BindAction("InputInteractionMovePoint", IE_Pressed, this, &AChopter::InputGoToPoint);
}

void AChopter::InteractionAction(ItemTypesEnum Item)
{
	if (bCarryingSupplies)
		AChopter::ChoseInteraction(Item);
	else {
		if (InteractiveDetected)
			if (ChopterStat != OFF) {
				AChopter::LineTraceToInteract();
				thdGoToPointDelegate.BindUFunction(this, "GoToPoint", Item);
				GetWorld()->GetTimerManager().SetTimer(thGoToPoint, thdGoToPointDelegate, UGameplayStatics::GetWorldDeltaSeconds(this), true, 0);
			}
			else {
				AChopter::ChoseInteraction(Item);
			}
	}
}

FVector AChopter::GetLocationInTerrain()
{
	return LandscapePosition;
}

bool AChopter::IsCarrySupply()
{
	return bCarryingSupplies;
}

ItemTypesEnum AChopter::GetInteractiveTypePure()
{
	return ItemTypesEnum();
}

FVector AChopter::GetInteractivePositionPure()
{
	return FVector();
}

void AChopter::ActivatePhysics()
{
}



void AChopter::ChopterMove() {
	if (ChopterStat == FLING) {
		float Xaxis = GetInputAxisValue(TEXT("MoveForwardBack"));
		float Yaxis = GetInputAxisValue(TEXT("MoveLeftRight"));
		Speed.X = Xaxis;
		Speed.Y = Yaxis;
		Speed = Speed.GetClampedToSize(0, 1) * (UGameplayStatics::GetWorldDeltaSeconds(this) * MovimentSpeed);

		if (!Speed.IsZero()) {
			AddActorWorldOffset(Speed, true);
			FRotator yaw;
			ChopterPitchRoll->SetWorldRotation(FRotator(-Xaxis * 10, 0, Yaxis * 10));

			yaw = ChopterYaw->GetRelativeRotation();

			ChopterYaw->SetRelativeRotation(FMath::RInterpTo(yaw,
				FRotator(yaw.Pitch, UKismetMathLibrary::MakeRotFromX(Speed).Yaw, yaw.Roll),
				UGameplayStatics::GetWorldDeltaSeconds(this), 7.0f), true);
		}
		else {
			ChopterPitchRoll->SetWorldRotation(FRotator(0, 0, 0));
		}
	}
}

void AChopter::LineTraceUnderDetector()
{
	FHitResult hitResult;
	FVector start = GetActorLocation();
	FVector end = start + FVector(0, 0, start.Z - 5000);
	FCollisionObjectQueryParams objectParameters;
	bool isHit = false;
	if (bDoLineTrace) {
		objectParameters.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel1);
		isHit = GetWorld()->LineTraceSingleByObjectType(hitResult, start, end, objectParameters);


		if (isHit) {
			if (hitResult.GetActor()->GetClass()->ImplementsInterface(UChopterInteractInterface::StaticClass())) {

				AChopter::OnInteractiveItemResponse(bCarryingSupplies ? ItemTypesEnum::SUPPLIES :
					Cast<AInteractiveItem>(hitResult.GetActor())->GetInteractiveTypePure());
				InteractiveDetected = true;
			}
			else {
				AChopter::OnInteractiveItemResponse(bCarryingSupplies ? ItemTypesEnum::SUPPLIES : ItemTypesEnum::NONE);
				InteractiveDetected = false;
			}
		}
		else {
			AChopter::OnInteractiveItemResponse(bCarryingSupplies ? ItemTypesEnum::SUPPLIES : ItemTypesEnum::NONE);
			InteractiveDetected = false;
		}
	}
	else {
		AChopter::OnInteractiveItemResponse(bCarryingSupplies ? ItemTypesEnum::SUPPLIES : ItemTypesEnum::NONE);
		InteractiveDetected = false;
	}
}

void AChopter::LineTraceToInteract()
{
	if (!bCarryingSupplies) {
		FHitResult hitResult;
		FVector start = GetActorLocation();
		FVector end = start + FVector(0, 0, start.Z - 5000);
		FCollisionObjectQueryParams objectParameters;
		bool isHit = false;

		objectParameters.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel1);
		isHit = GetWorld()->LineTraceSingleByObjectType(hitResult, start, end, objectParameters);

		if (isHit) {
			if (hitResult.GetActor()->GetClass()->ImplementsInterface(UChopterInteractInterface::StaticClass())) {
				InteractingItem = Cast<AInteractiveItem>(hitResult.GetActor());
				InteractiveLocation = InteractingItem->GetInteractivePositionPure();
				DistanceToItemBellow = FVector::Dist(ChopterRootComponent->GetComponentLocation(), InteractiveLocation);
			}
			else
				InteractingItem = nullptr;
		}
		else
			InteractingItem = nullptr;
	}
}

void AChopter::LineTraceTarrainPosition()
{
	FHitResult hitResult;
	FVector start = GetActorLocation();
	FVector end = start + FVector(0, 0, start.Z - 5000);
	FCollisionObjectQueryParams objectParameters;
	bool isHit = false;

	objectParameters.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel2);
	isHit = GetWorld()->LineTraceSingleByObjectType(hitResult, start, end, objectParameters);
	DrawDebugLine(GetWorld(), start, end, FColor::Red);
	if (isHit) {
		DrawDebugSphere(GetWorld(), hitResult.ImpactPoint, 50, 8, FColor::Red);
			LandscapePosition = hitResult.ImpactPoint;
		
	}
}

//void AChopter::PrintTest(FString msg) {
//	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, msg);
//}

void AChopter::GoToPoint(ItemTypesEnum Item)
{
	//PrintTest("GoToPoint");
	FVector newPosition = GetActorLocation();
	newPosition.X = InteractiveLocation.X;
	newPosition.Y = InteractiveLocation.Y;

	FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(ChopterYaw->GetComponentLocation(),
														FVector(newPosition.X, newPosition.Y, ChopterYaw->GetComponentLocation().Z));
	
	ChopterYaw->SetWorldRotation(FMath::RInterpTo(ChopterYaw->GetComponentRotation(), Rotator,
									UGameplayStatics::GetWorldDeltaSeconds(this), 7.0f), true);
	SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), newPosition, UGameplayStatics::GetWorldDeltaSeconds(this) * MovimentSpeed,1));

	if (FMath::IsNearlyEqual(InteractiveLocation.X, GetActorLocation().X) && FMath::IsNearlyEqual(InteractiveLocation.Y, GetActorLocation().Y)) {
		AChopter::ChangeStat(FLING);
		//ChopterStat = FLING;
		GetWorldTimerManager().ClearTimer(thGoToPoint);
		AChopter::ChoseInteraction(Item);
		
	}
}

void AChopter::ChoseInteraction(ItemTypesEnum Item) {
	//AChopter::PrintTest("ChoseInteraction");
	AChopter::LineTraceToInteract();
	ItemDetected = Item;

	switch (Item)
	{
	case ItemTypesEnum::NONE:

		break;
	case ItemTypesEnum::LANDINGZONE:
		if (ChopterStat == OFF) {
			AChopter::InputStartRotors();
		}
		else {
			bDoLineTrace = false;
			AChopter::ChangeStat(LANDING);
			//ChopterStat = LANDING;
		}
		break;
	case ItemTypesEnum::REFUGEEZONE:
		AChopter::RescueRefugees();

		break;
	case ItemTypesEnum::FUEL:
		AChopter::SetAnchorAnimation(AnchorRefuelCurve, false);
		break;
	case ItemTypesEnum::SUPPLIES:

		if (InteractiveDetected && !bCarryingSupplies)
			AChopter::SetAnchorAnimation(AnchorSupplyCurve, false);
		else if(!InteractiveDetected && bCarryingSupplies)
			AChopter::SetAnchorAnimation(AnchorSupplyCurve, true);

		break;
	}
}


void AChopter::SetAnchorAnimation(UCurveFloat* curve, bool isReverse) {
	if (curve) {
		CurrentCurve = curve;
		tlCable.SetFloatCurve(curve, FName("actionCurve"));

		if (isReverse)
			tlCable.ReverseFromEnd();
		else {
			AchorMagneticMesh->SetVisibility(true);
			tlCable.PlayFromStart();
		}
	}
}

#pragma region Chopter Controllers

void AChopter::InputStartRotors() {
	bDoLineTrace = false;
	AChopter::OnInteractiveItemResponse(ItemTypesEnum::NONE);
	bWasLanding = false;
	AChopter::ChangeStat(START_ENGINE);
	//ChopterStat = START_ENGINE;
}

void AChopter::RotorTimer(bool isOn)
{
	if (isOn) {
		if (RotorVelocity <= 5.0f) {
			RotorVelocity += 0.1f;
		}
		else {
			if (!bWasLanding)
				AChopter::ChangeStat(TAKEOFF);
				//ChopterStat = TAKEOFF;
		}
	}
	else {
		if (RotorVelocity >= 0.0f) {
			RotorVelocity -= 0.1f;
		}
	}
}

void AChopter::InputTakeOff(float deltatime)
{
	if (!InteractingItem)
		return;

	FVector position = InteractingItem->GetInteractivePositionPure();
	position.Z = position.Z + Altitude;

	if (!position.Equals(GetActorLocation(), 5)) {

		SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), position, deltatime, Altitude), true);
	}
	else {
		bDoLineTrace = true;
		AChopter::ChangeStat(FLING);
		//ChopterStat = FLING;
	}
}

void AChopter::InputGoToPoint(void* action) {
	if (InteractiveDetected)
		AChopter::ChangeStat(GOTOPOINT);
		//ChopterStat = GOTOPOINT;
}


void AChopter::PickUpItem() {

	TimeLineValue = tlCable.GetPlaybackPosition();

	CurveFloatValue = DistanceToItemBellow * CurrentCurve->GetFloatValue(TimeLineValue);

	AchorInteractive->SetWorldLocation(FVector(AchorInteractive->GetComponentLocation().X,
		AchorInteractive->GetComponentLocation().Y,
		ChopterRootComponent->GetComponentLocation().Z - CurveFloatValue));
}

void AChopter::AnchorItemTaken() {
	FAttachmentTransformRules rule(EAttachmentRule::KeepWorld, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
	//PrintTest("ItemTaken");
	switch (ItemDetected)
	{
	case ItemTypesEnum::NONE:

		break;
	case ItemTypesEnum::LANDINGZONE:

		break;
	case ItemTypesEnum::REFUGEEZONE:

		break;
	case ItemTypesEnum::FUEL:
		InteractingItem->AttachToComponent(AchorInteractive, rule);
		break;
	case ItemTypesEnum::SUPPLIES:
		AChopter::CarrySupply(rule);
		break;
	}
}

void AChopter::AnchorFinish() {
	switch (ItemDetected)
	{
	case ItemTypesEnum::NONE:

		break;
	case ItemTypesEnum::LANDINGZONE:

		break;
	case ItemTypesEnum::REFUGEEZONE:

		break;
	case ItemTypesEnum::FUEL:
		AChopter::Refuel();
		AchorMagneticMesh->SetVisibility(false);
		break;
	case ItemTypesEnum::SUPPLIES:
		bCarryingSupplies = !bCarryingSupplies;
		if(!bCarryingSupplies)
			AchorMagneticMesh->SetVisibility(false);
		break;
	}
}

void AChopter::Refuel()
{
	InteractingItem->Destroy();
	InteractingItem = nullptr;
	Fuel = 1.0f;
}

void AChopter::FuelConsumption() {
	if (ChopterStat != OFF) {
		Fuel -= FuelCons;
	}

	if (Fuel <= 0) {
		AChopter::ChangeStat(DESTROYED);
		//ChopterStat = DESTROYED;

		AChopter::ChopterDestroy(FUEL_EMPTY);
	}
}

void AChopter::ChangeStat(ChopterStats stat)
{
	ChopterStat = stat;
	AChopter::OnStatChanged(stat);
}

void AChopter::ChopterDestroy(DestroyedType type) {

	AChopter::ChangeStat(DESTROYED);
	//ChopterStat = OFF;
	AchorInteractive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ChopterBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ChopterTopRotor->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ChopterTailRotor->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ChopterBody->SetVisibility(false);
	ChopterTopRotor->SetVisibility(false);
	ChopterTailRotor->SetVisibility(false);
	if (bCarryingSupplies)
		InteractingItem->ActivatePhysics();

	switch (type)
	{
	case FUEL_EMPTY:
		AChopter::SpawnChopterFaller(ChopterBody->GetComponentRotation(), ChopterTopRotor->GetComponentRotation(), ChopterTailRotor->GetComponentRotation());
		break;
	case LIFE_EMPTY:
		AChopter::OnChopterExplode();
		break;
	}
}

void AChopter::ChopterDamage(float damage) {
	Life -= damage;
	if (Life <= 0) {
		AChopter::ChopterDestroy(LIFE_EMPTY);
	}
}

void AChopter::CarrySupply(FAttachmentTransformRules rule)
{
	if (bCarryingSupplies)
		InteractingItem->GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	else
		InteractingItem->AttachToComponent(AchorInteractive, rule);

	InteractingItem->SetActorEnableCollision(bCarryingSupplies);

}

void AChopter::DeliverSupply()
{
	
}

void AChopter::RescueRefugees()
{
	//AChopter::PrintTest("REFUGEEZONE");
}

void AChopter::DisembarkRefugees()
{
}

void AChopter::Landing(float deltatime) {
	FVector position = InteractingItem->GetInteractivePositionPure();

	if (!position.Equals(ChopterLandingGearHeight->GetComponentLocation(), 5))
	{
		SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), position, deltatime, Altitude), true);
	}
	else {
		bWasLanding = true;
		bDoLineTrace = true;
		AChopter::ChangeStat(OFF);
		//ChopterStat = OFF;
	}
}

FHitResult AChopter::GroundLineTrace() {
	FHitResult OutHit;
	FVector Start = GetActorLocation();
	FVector End = Start + FVector(0, 0, Start.Z * (-5000));
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldDynamic, TraceParams);
	
	return 	OutHit;
}

#pragma endregion