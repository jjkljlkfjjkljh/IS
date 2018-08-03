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
class UPauseMenu;
class AISPlayerController;
class UMaterialInstanceDynamic;
class USpringArmComponent;

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
	void EnvironmentSwitchCamera();
	void Interact();
	void StartSprint();
	void StopSprint();
	void StartCrouch();
	void StopCrouch();
	void MoveForward(float InputAmount);
	void MoveRight(float InputAmount);
	void LookUp(float InputAmount);
	void Turn(float InputAmount);

	void LerpPlayerOpacityInvisible(float A, float B, float Alpha);
	void LerpPlayerOpacityVisible(float A, float B, float Alpha);

	//Function the pause widget can call to clear the pause screen UI and set variables correctly
	UFUNCTION(BlueprintCallable, Category = "CustomUI")
	void Pause();

	//Spawns dynamic camera
	ADynamicCamera* SpawnAndSetCamera();

	//Makes sure the game starts off with the correct camera settings
	void SetupComponents();

	ADynamicCamera* DynamicCamera = nullptr;
	AISPlayerController* PlayerController = nullptr;

	bool bIsGamePaused = false;
	bool bIsPlayerCrouched = false;

	int32 InsideCount = 0;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float MaxTurnRate = 120.f;

private:

	FHitResult GetFirstWorldDynamicInReach();

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// declare overlap end function
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	AActor* Player;

	USkeletalMeshComponent* MeshComponent = nullptr;

	UCapsuleComponent* CapsuleComponent = nullptr;

	USpringArmComponent* SpringArm = nullptr;

	UFirstPersonCameraLocation* FirstPersonCameraLocation = nullptr;
	UThirdPersonCameraLocation* ThirdPersonCameraLocation = nullptr;

	UPauseMenu* PauseMenu = nullptr;

	UMaterialInstanceDynamic* PlayerMaterialInstance = nullptr;

	bool bSprinting = false;
	bool bIsLerpingToVisible = false;
	bool bIsLerpingToInvisible = false;

	float GlobalDeltaTime = 0.f;
	float SprintAlpha = 0.f;
	float CurrentSprintSpeed;

	float CurrentTurnInput;
	float CurrentLookInput;
	float CurrentForwardInput;
	float CurrentRightInput;

	float FirstPersonLookUpOffset = 0.f;
	float MaxLookAmount = 50.f;

	float TurnAlpha = 0.f;
	float CurrentTurnSpeed = 0.f;

	float HeadHeight = 70.f;
	float ForwardHeadOffset = 10.f;

	float LerpAlpha = 0.f;
	float PlayerOpacity = 1.f;
	float LerpModifier = 2.5f;

	FTransform SpawnTransform;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float BaseTurnRate = 45.f;

	float BaseLookUpRate = 45.f;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float TurnRampOverTime = 0.3f;

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
	float SprintRampUpMultiplier = 0.01f;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float CrouchSpeed = 85.f;

	//force applied for jumps
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float JumpVelocity = 450.f;

	//percentage of max walk speed allowed to move while in air
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	float AirControl = 0.3;

	//How far ahead of the player can we reach in cm
	float Reach = 200.f;

	bool bHasKey = false;
};
