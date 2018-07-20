// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ISSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class IS_API UISSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UISSaveGame();

	UPROPERTY(EditAnywhere)
	bool bAimAccelerationEnabled;

	UPROPERTY(EditAnywhere)
	bool bPlayerControlledCamera;

	UPROPERTY(EditAnywhere)
	bool bToggleSprint;

	UPROPERTY(EditAnywhere)
	bool bToggleCrouch;

	UPROPERTY(EditAnywhere)
	int32 AimResponseCurve;

	UPROPERTY(EditAnywhere)
	float FieldOfView;

	UPROPERTY(EditAnywhere)
	bool bFPHeadBob;
	
};
