// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ChopterInteractInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UChopterInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CHOPTERCOMBAT_API IChopterInteractInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent)
		ItemTypesEnum GetInteractiveType();

	UFUNCTION()
		virtual ItemTypesEnum GetInteractiveTypePure() = 0;

	virtual FVector GetInteractivePositionPure()  = 0;

	virtual void ActivatePhysics() = 0;
};
