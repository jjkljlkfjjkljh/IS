// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ISPlayerCharacter.generated.h"

///Forward Declarations
class UFirstPersonCameraComponent;
class UThirdPersonCameraComponent;
class UViewRotator;

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

	//Input functions
	void SwitchCamera();
	void Interact();
	void StartSprint();
	void StopSprint();
	void StartCrouch();
	void StopCrouch();
	void MoveForward(float InputAmount);
	void MoveRight(float InputAmount);
	void LookUp(float InputAmount);
	void Turn(float InputAmount);

	//Finds the camera components and sets them to the appropriate variables
	void FindCameraComponents();

	//Mkaes sure the game starts off with the correct camera settings
	void SetupComponents();

private:

	FHitResult GetFirstWorldDynamicInReach();

	UFirstPersonCameraComponent * FirstPersonCamera = nullptr;
	UThirdPersonCameraComponent * ThirdPersonCamera = nullptr;

	//UCharacterMovementComponent* MovementComponent = nullptr;

	AActor* Player;

	UStaticMeshComponent* MeshComponent = nullptr;;

	USceneComponent* SpringArm = nullptr;

	float BaseTurnRate;
	float BaseLookUpRate;

	//speed when not sprinting
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float WalkSpeed = 250.f;

	//speed when sprinting
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float SprintSpeed = 550.f;

	//force applied for jumps
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float JumpVelocity = 400.f;

	//percentage of max walk speed allowed to move while in air
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float AirControl = 0.3;

	//Wether or not to display the interact debug line
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	bool bShowDebugLine = false;

	bool bIsFirstPerson = true;

	//How far ahead of the player can we reach in cm
	float Reach = 200.f;

	bool bHasKey = false;
};
