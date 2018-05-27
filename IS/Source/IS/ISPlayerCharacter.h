// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ISPlayerCharacter.generated.h"

///Forward Declarations
class UFirstPersonCameraComponent;
class UThirdPersonCameraComponent;

UCLASS()
class IS_API AISPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AISPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Switches between the 2 cameras on the player
	void SwitchCamera();

	void Jump();

	//Finds the camera components and sets them to the appropriate variables
	void FindCameraComponents();

	//Mkaes sure the game starts off with the correct camera settings
	void SetupComponents();

private:

	UFirstPersonCameraComponent * FirstPersonCamera = nullptr;
	UThirdPersonCameraComponent * ThirdPersonCamera = nullptr;

	//UCharacterMovementComponent* MovementComponent = nullptr;

	AActor* Player;

};
