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
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool bIsFirstPerson = false;

	bool bIsPlayerControlled = true;

private:
	void SetFirstPersonLocation();

	void SetThirdPersonLocation();

	AActor* Player = nullptr;
	UFirstPersonCameraLocation* FirstPersonLocation;
	UThirdPersonCameraLocation* ThirdPersonLocation;
};
