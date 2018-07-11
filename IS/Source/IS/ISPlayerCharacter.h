#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ISPlayerCharacter.generated.h"

///Forward Declarations
class UFirstPersonCameraLocation;
class UThirdPersonCameraLocation;
class UViewRotator;
class UCapsuleComponent;
class ADynamicCamera;

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
	void PlayerSwitchCamera();
	void EnvironmentSwitchCamera(bool bIsFirstPerson);
	void Interact();
	void StartSprint();
	void StopSprint();
	void StartCrouch();
	void StopCrouch();
	void MoveForward(float InputAmount);
	void MoveRight(float InputAmount);
	void LookUp(float InputAmount);
	void Turn(float InputAmount);

	//Spawns dynamic camera
	ADynamicCamera* SpawnAndSetCamera();

	//Makes sure the game starts off with the correct camera settings
	void SetupComponents();

	ADynamicCamera* DynamicCamera = nullptr;

	bool bIsPlayerCrouched = false;

private:

	FHitResult GetFirstWorldDynamicInReach();

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// declare overlap end function
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//UCharacterMovementComponent* MovementComponent = nullptr;

	AActor* Player;

	UStaticMeshComponent* MeshComponent = nullptr;;

	USceneComponent* SpringArm = nullptr;

	UCapsuleComponent* CapsuleComponent = nullptr;

	UFirstPersonCameraLocation* FirstPersonCameraLocation = nullptr;
	UThirdPersonCameraLocation* ThirdPersonCameraLocation = nullptr;

	bool bSprinting = false;

	float SprintAlpha = 0.f;
	float CurrentSprintSpeed;

	float TurnAlpha = 0.f;
	float CurrentTurnSpeed;

	float BaseTurnRate;
	float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float MaxTurnRate = 50.f;

	//Capsule half-height when standing
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float ColliderStandingHeight = 88.f;

	//Capsule half-height when crouching
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float ColliderCrouchingHeight = 60.f;

	//speed when not sprinting
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float WalkSpeed = 250.f;

	//speed when sprinting
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float SprintSpeed = 550.f;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float SprintRampUpMultiplier = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float CrouchSpeed = 150.f;

	//force applied for jumps
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float JumpVelocity = 450.f;

	//percentage of max walk speed allowed to move while in air
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float AirControl = 0.3;

	//Wether or not the player can control if the camera is first or third person
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	bool bPlayerControlsCameraPerspective = false;

	//How far ahead of the player can we reach in cm
	float Reach = 200.f;

	bool bHasKey = false;
};
