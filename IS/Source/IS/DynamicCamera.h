// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "DynamicCamera.generated.h"

///Forward Declarations
class UFirstPersonCameraLocation;
class UThirdPersonCameraLocation;
/**
 * 
 */
UCLASS()
class IS_API ADynamicCamera : public ACameraActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this component's properties
	ADynamicCamera();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	bool bIsFirstPerson = false;

	bool bIsPlayerControlled = true;

	void SetFirstPersonLocation(FTransform TargetTransform, float DeltaTime);

	void SetThirdPersonLocation(FTransform TargetTransform, float DeltaTime);

private:

	void LerpToNewTransform(FTransform Target, float DeltaTime);

	bool bWasFirstPerson = false;
	bool bTransitioning = false;

	FTransform CurrentTransform;

	float AlphaMultiplier = 0.3f;
	float Alpha = 0.0f;

	float ClosestDistanceAllowed = 120.0f;

	AActor* Player = nullptr;
	UFirstPersonCameraLocation* FirstPersonLocation;
	UThirdPersonCameraLocation* ThirdPersonLocation;
};
