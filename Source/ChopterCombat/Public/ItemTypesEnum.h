// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


UENUM(BlueprintType)
enum class ItemTypesEnum : uint8 {
	NONE UMETA(DisplayName = "None"),
	LANDINGZONE UMETA(DisplayName = "landingZone"),
	REFUGEEZONE UMETA(DisplayName = "RefugeeZone"),
	FUEL UMETA(DisplayName = "Fuel"),
	SUPPLIES UMETA(DisplayName = "Supplies"),
};
