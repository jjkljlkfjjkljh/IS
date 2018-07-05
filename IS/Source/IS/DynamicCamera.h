#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "DynamicCamera.generated.h"

///Forward Declarations
class AISPlayerCharacter;
class UFirstPersonCameraLocation;
class UThirdPersonCameraLocation;
/**
 * 
 */
UCLASS()
class IS_API ADynamicCamera : public ACameraActor
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	bool bIsFirstPerson = false;

	bool bIsPlayerControlled = true;

	void SetFirstPersonLocation(FTransform TargetTransform, float DeltaTime);

	void SetThirdPersonLocation(FTransform TargetTransform, FVector PlayerPosition, bool bIsFalling, bool bIsCrouching, float DeltaTime);

	AISPlayerCharacter* Player = nullptr;

private:

	void LerpToNewTransform(bool RotationMatters, FTransform Target, float AlphaOverrideDistance, float TimeScale, float DeltaTime);

	void RepositionThirdPersonCamera(FTransform Target, FVector PlayerPosition);

	void CameraPositionDuringJump(FTransform TargetTransform, FVector PlayerPosition);

	void CameraPositionDuringCrouch(FTransform TargetTransform, FVector PlayerPosition, float DeltaTime);

	bool bWasFirstPerson = false;
	bool bTransitioning = false;
	bool bInAir = false;
	bool bJustLanded = false;
	bool bJumpStartLocationSet = false;
	bool bFinishedCrouchTransition = false;

	FTransform CurrentTransform;
	FVector JumpStartLocation;
	FVector CrouchStartLocation;

	float AlphaMultiplier = 0.3f;
	float Alpha = 0.0f;
	float MainOverrideDistance = 15.0f;
	float CrouchOverrideDistance = 1.0f;

	float ClosestDistanceAllowed = 200.0f;
	float MaxHeightAbovePlayer = 120.0f;
};
