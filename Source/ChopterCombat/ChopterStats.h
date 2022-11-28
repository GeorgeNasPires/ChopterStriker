// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ChopterStats.generated.h"

UENUM(BlueprintType)
    enum ChopterStats{
	OFF				UMETA(DisplayName = "Off"),
	START_ENGINE	UMETA(DisplayName = "Start Engine"),
	TAKEOFF			UMETA(DisplayName = "Take-Off"),
	LANDING			UMETA(DisplayName = "Landing"),
	FLING			UMETA(DisplayName = "Fling"),
	DAMAGED			UMETA(DisplayName = "Damaged"),
	DESTROYED		UMETA(DisplayName = "Destroyed"),
	GOTOPOINT		UMETA(DisplayName = "GoToPoint")

};

UENUM(BlueprintType)
enum DestroyedType {
	FUEL_EMPTY		UMETA(DisplayName = "FuelEmpty"),
	LIFE_EMPTY		UMETA(DisplayName = "LifeEmpty"),


};