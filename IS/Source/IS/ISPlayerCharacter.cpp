#include "ISPlayerCharacter.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "DynamicCamera.h"
#include "Engine/World.h"
#include "Containers/Array.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "FirstPersonCameraComponent.h" 
#include "ThirdPersonCameraComponent.h"
#include "FirstPersonCameraLocation.h"
#include "ThirdPersonCameraLocation.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "ViewRotator.h"
#include "Math/UnrealMathVectorCommon.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Door.h"
#include "Components/SkeletalMeshComponent.h"
#include "ISPlayerController.h"
#include "Camera/CameraActor.h"
#include "ISPlayerController.h"
#include "PauseMenu.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

#define PRINT(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT(x));}
#define PRINT_GREEN(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT(x));}
#define PRINT_RED(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT(x));}

#define WARNING(x) UE_LOG(LogTemp, Warning, TEXT(x));
#define ERROR(x) UE_LOG(LogTemp, Error, TEXT(x));

// Sets default values
AISPlayerCharacter::AISPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;

	/// make sure the pawn is set up to be controlled
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	///Set up character movement component with default values
	GetCharacterMovement()->bOrientRotationToMovement = true; //Move character in direction of input
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = JumpVelocity;
	GetCharacterMovement()->AirControl = AirControl;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	///Bind Overlap Events
	CapsuleComponent = FindComponentByClass<UCapsuleComponent>();
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &AISPlayerCharacter::OnOverlapBegin);
	CapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &AISPlayerCharacter::OnOverlapEnd);
}

// Called when the game starts or when spawned
void AISPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	Player = this;

	DynamicCamera = SpawnAndSetCamera();
	SetupComponents();
}

// Called every frame
void AISPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GlobalDeltaTime = DeltaTime;

	if (GetCharacterMovement()->IsFalling() && bIsPlayerCrouched)
	{
		bIsPlayerCrouched = false;
		CapsuleComponent->SetCapsuleHalfHeight(ColliderStandingHeight);
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		if (MeshComponent)
		{
			FVector MeshLocation = MeshComponent->GetComponentLocation();
			MeshComponent->SetWorldLocation(FVector(
				MeshLocation.X,
				MeshLocation.Y,
				MeshLocation.Z - (ColliderStandingHeight - ColliderCrouchingHeight)));
		}
	}

	if (bIsLerpingToInvisible)
	{
		LerpAlpha -= (DeltaTime * LerpModifier);
		LerpPlayerOpacityInvisible(PlayerOpacity, 0.f, LerpAlpha);
		if (LerpAlpha <= 0)
		{
			LerpAlpha = 0.f;
			PlayerOpacity = 0.f;
			PlayerMaterialInstance->SetScalarParameterValue(FName("Opacity"), 0.f);
			bIsLerpingToInvisible = false;
		}
	}
	if (bIsLerpingToVisible)
	{
		LerpAlpha += (DeltaTime * LerpModifier);
		LerpPlayerOpacityVisible(PlayerOpacity, 1.f, LerpAlpha);
		if (LerpAlpha >= 1)
		{
			LerpAlpha = 0.f;
			PlayerOpacity = 1.f;
			PlayerMaterialInstance->SetScalarParameterValue(FName("Opacity"), 1.f);
			bIsLerpingToVisible = false;
		}
	}

	if (DynamicCamera->bIsFirstPerson)
	{
		FTransform FirstPersonTransform = FirstPersonCameraLocation->GetComponentTransform();
		FirstPersonTransform.SetRotation(GetActorForwardVector().ToOrientationQuat());
		FRotator FirstPersonRotation = FirstPersonTransform.Rotator();
		if (!PlayerController->LoadedData.bSettingsFPHeadBob)
		{
			FVector FirstPersonLocation = FirstPersonTransform.GetLocation();
			FirstPersonLocation.X = GetActorLocation().X;
			FirstPersonLocation.Y = GetActorLocation().Y;
			FirstPersonLocation.Z = (GetActorLocation().Z + HeadHeight);
			FirstPersonTransform.SetLocation(FirstPersonLocation);
		}
		FirstPersonRotation.Pitch += FirstPersonLookUpOffset;
		FirstPersonTransform.SetRotation(FirstPersonRotation.Quaternion());
		DynamicCamera->SetFirstPersonLocation(FirstPersonTransform, DeltaTime);
	}
	else
	{
		FTransform ThirdPersonTransform = ThirdPersonCameraLocation->GetComponentTransform();
		DynamicCamera->SetThirdPersonLocation(
			ThirdPersonTransform,
			GetActorLocation(), 
			GetCharacterMovement()->IsFalling(), 
			bIsPlayerCrouched,
			DeltaTime);
	}

	if (bSprinting)
	{
		SprintAlpha += DeltaTime;
		CurrentSprintSpeed = (GetCharacterMovement()->MaxWalkSpeed + (SprintAlpha * SprintRampUpMultiplier * SprintSpeed));

		if (CurrentSprintSpeed >= SprintSpeed)
		{
			GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = CurrentSprintSpeed;
		}
	}
}

// Called to bind functionality to input
void AISPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);
	///InputAction bindings
	PlayerInputComponent->BindAction("SwitchCamera", IE_Pressed, this, &AISPlayerCharacter::PlayerSwitchCamera);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AISPlayerCharacter::Interact);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AISPlayerCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AISPlayerCharacter::StopSprint);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AISPlayerCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AISPlayerCharacter::StopCrouch);

	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AISPlayerCharacter::Pause);

	///InputAxis bindings
	PlayerInputComponent->BindAxis("MoveForward", this, &AISPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AISPlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &AISPlayerCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &AISPlayerCharacter::Turn);
}

void AISPlayerCharacter::PlayerSwitchCamera()
{
	if (PlayerController->LoadedData.bSettingsPlayerControlledCamera)
	{
		if (DynamicCamera->bIsFirstPerson)
		{
			DynamicCamera->bIsFirstPerson = false;
			FirstPersonLookUpOffset = 0.f;
			bIsLerpingToVisible = true;
			bIsLerpingToInvisible = false;
			PlayerMaterialInstance->GetScalarParameterValue(FName("Opacity"), PlayerOpacity);
			LerpAlpha = PlayerOpacity;
		}
		else
		{
			DynamicCamera->bIsFirstPerson = true;
			bIsLerpingToInvisible = true;
			bIsLerpingToVisible = false;
			PlayerMaterialInstance->GetScalarParameterValue(FName("Opacity"), PlayerOpacity);
			LerpAlpha = PlayerOpacity;
		}
	}
	return;
}

//If set to true the camera will be first person
void AISPlayerCharacter::EnvironmentSwitchCamera()
{
	if (!PlayerController->LoadedData.bSettingsPlayerControlledCamera)
	{
		if (DynamicCamera->bIsFirstPerson && (InsideCount <= 0))
		{
			DynamicCamera->bIsFirstPerson = false;
			FirstPersonLookUpOffset = 0.f;
			bIsLerpingToVisible = true;
			bIsLerpingToInvisible = false;
			PlayerMaterialInstance->GetScalarParameterValue(FName("Opacity"), PlayerOpacity);
			LerpAlpha = PlayerOpacity;
		}
		else if(!DynamicCamera->bIsFirstPerson && (InsideCount <= 0))
		{
			DynamicCamera->bIsFirstPerson = false;
			FirstPersonLookUpOffset = 0.f;
			bIsLerpingToVisible = true;
			bIsLerpingToInvisible = false;
			PlayerMaterialInstance->GetScalarParameterValue(FName("Opacity"), PlayerOpacity);
			LerpAlpha = PlayerOpacity;
		}
		else
		{
			DynamicCamera->bIsFirstPerson = true;
			bIsLerpingToInvisible = true;
			bIsLerpingToVisible = false;
			PlayerMaterialInstance->GetScalarParameterValue(FName("Opacity"), PlayerOpacity);
			LerpAlpha = PlayerOpacity;
		}
	}
	return;
}

void AISPlayerCharacter::Interact()
{
	FHitResult HitResult = GetFirstWorldDynamicInReach();
	AActor* ActorHit = HitResult.GetActor();
	if (ActorHit)
	{
		UDoor* Door = ActorHit->FindComponentByClass<UDoor>();
		if (Door)
		{
			FVector PlayerFacingDirection = GetActorForwardVector();
			Door->AttemptToOpenDoor(bHasKey, PlayerFacingDirection);
		}
		
	}
}

void AISPlayerCharacter::StartSprint()
{
	if (!bSprinting)
	{
		if (bIsPlayerCrouched)
		{
			return;
		}
		bSprinting = true;
		return;
	}
	else
	{
		bSprinting = false;
		SprintAlpha = 0.f;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		return;
	}
}

void AISPlayerCharacter::StopSprint()
{
	//check if toggle to sprint is set and return without stopping sprint if it is
	if (PlayerController->LoadedData.bSettingsToggleSprint) { return; }
	bSprinting = false;
	SprintAlpha = 0.f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	return;
}

void AISPlayerCharacter::StartCrouch()
{
	if (!bIsPlayerCrouched)
	{
		if (bSprinting)
		{
			bSprinting = false;
			SprintAlpha = 0.f;
		}
		GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
		bIsPlayerCrouched = true;
		CapsuleComponent->SetCapsuleHalfHeight(ColliderCrouchingHeight);
		if (MeshComponent)
		{
			FVector MeshLocation = MeshComponent->GetComponentLocation();
			MeshComponent->SetWorldLocation(FVector(
				MeshLocation.X,
				MeshLocation.Y,
				MeshLocation.Z + (ColliderStandingHeight - ColliderCrouchingHeight)));
		}
		return;
	}
	else
	{
		bIsPlayerCrouched = false;
		CapsuleComponent->SetCapsuleHalfHeight(ColliderStandingHeight);
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		if (MeshComponent)
		{
			FVector MeshLocation = MeshComponent->GetComponentLocation();
			MeshComponent->SetWorldLocation(FVector(
				MeshLocation.X,
				MeshLocation.Y,
				MeshLocation.Z - (ColliderStandingHeight - ColliderCrouchingHeight)));
		}
		return;
	}
}

void AISPlayerCharacter::StopCrouch()
{
	//check if toggle to crouch is set and return without stopping crouch if it is
	if (PlayerController->LoadedData.bSettingsToggleCrouch) { return; }
	bIsPlayerCrouched = false;
	CapsuleComponent->SetCapsuleHalfHeight(ColliderStandingHeight);
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	if (MeshComponent)
	{
		FVector MeshLocation = MeshComponent->GetComponentLocation();
		MeshComponent->SetWorldLocation(FVector(
			MeshLocation.X,
			MeshLocation.Y,
			MeshLocation.Z - (ColliderStandingHeight - ColliderCrouchingHeight)));
	}
	return;
}

//Mostly Follows the Unreal Third person example project's implementation to keep things simple
void AISPlayerCharacter::MoveForward(float InputAmount)
{
	float MovementSpeed = GetVelocity().Size();
	CurrentForwardInput = InputAmount;
	if ((InputAmount <= 0.01f) && (InputAmount >= -0.01f))
	{
		if ((CurrentRightInput <= 0.01f) && (CurrentRightInput >= -0.01f))
		{
			if (!bIsPlayerCrouched)
			{
				SprintAlpha = 0.f;
				GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
			}
		}
	}
	if ((Controller != NULL) && InputAmount != 0.f)
	{
		//figure out which direction is forward
		const FRotator ControllerRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, ControllerRotation.Yaw, 0);

		//Get the forward vector and apply movement
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		//Apply movement
		AddMovementInput(ForwardDirection, InputAmount);

	}
	return;
}

//Mostly Follows the Unreal Third person example project's implementation to keep things simple
void AISPlayerCharacter::MoveRight(float InputAmount)
{
	CurrentRightInput = InputAmount;
	if ((InputAmount <= 0.01f) && (InputAmount >= -0.01f))
	{
		if ((CurrentForwardInput <= 0.01f) && (CurrentForwardInput >= -0.01f))
		{
			if (!bIsPlayerCrouched)
			{
				SprintAlpha = 0.f;
				GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
			}
		}
	}
	if ((Controller != NULL) && InputAmount != 0.f)
	{
		//figure out which direction is forward
		const FRotator ControllerRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, ControllerRotation.Yaw, 0);

		//Get the forward vector and apply movement
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		//Apply movement
		AddMovementInput(ForwardDirection, InputAmount);

	}
	return;
}

//TODO Clamp the look height and fix the player looking up and down in first person
void AISPlayerCharacter::LookUp(float InputAmount)
{
	CurrentLookInput = InputAmount;
	if ((InputAmount >= 0.01f) || (InputAmount <= -0.01f))
	{
		if (CurrentTurnSpeed <= MaxTurnRate)
		{
			TurnAlpha = (GlobalDeltaTime * TurnRampOverTime);
			CurrentTurnSpeed += (TurnAlpha * MaxTurnRate);
			if (DynamicCamera->bIsFirstPerson)
			{
				FirstPersonLookUpOffset += -(InputAmount * CurrentTurnSpeed * GetWorld()->GetDeltaSeconds());
			}
			else
			{
				SpringArm->AddLocalRotation(FRotator(-(InputAmount * CurrentTurnSpeed * GetWorld()->GetDeltaSeconds()), 0.f, 0.f));
			}
			return;
		}
		else
		{
			if (DynamicCamera->bIsFirstPerson)
			{
				FirstPersonLookUpOffset += -(InputAmount * MaxTurnRate * GetWorld()->GetDeltaSeconds());
			}
			else
			{
				SpringArm->AddLocalRotation(FRotator(-(InputAmount * MaxTurnRate * GetWorld()->GetDeltaSeconds()), 0.f, 0.f));
			}
			return;
		}
	}
	else if ((CurrentTurnInput <= 0.01f) && (CurrentTurnInput >= -0.01))
	{
		CurrentTurnSpeed = BaseTurnRate;
	}
	return;
}

void AISPlayerCharacter::Turn(float InputAmount)
{
	CurrentTurnInput = InputAmount;
	if ((InputAmount >= 0.01f) || (InputAmount <= -0.01f))
	{
		if (CurrentTurnSpeed <= MaxTurnRate)
		{
			TurnAlpha = (GlobalDeltaTime * TurnRampOverTime);
			CurrentTurnSpeed += (TurnAlpha * MaxTurnRate);
			AddControllerYawInput(InputAmount * CurrentTurnSpeed * GetWorld()->GetDeltaSeconds());
		}
		else
		{
			AddControllerYawInput(InputAmount * MaxTurnRate * GetWorld()->GetDeltaSeconds());
			return;
		}
	}
	else if((CurrentLookInput <= 0.01f) && (CurrentLookInput >= -0.01f))
	{
		CurrentTurnSpeed = BaseTurnRate;
	}
}

void AISPlayerCharacter::Pause()
{
	PRINT_GREEN("Pause");
	switch (bIsGamePaused)
	{
		//unpause
	case true:
		bIsGamePaused = !bIsGamePaused;
		PlayerController->ShowPauseMenu(false);
		break;
		//pause
	case false:
		bIsGamePaused = !bIsGamePaused;
		PlayerController->ShowPauseMenu(true);
		break;
	}
}

void AISPlayerCharacter::SetupComponents()
{
	//TODO Clean up commented out lines
	//FirstPersonCamera->bUsePawnControlRotation = true;
	MeshComponent = FindComponentByClass<USkeletalMeshComponent>();
	SpringArm = FindComponentByClass<USpringArmComponent>();
	FirstPersonCameraLocation = FindComponentByClass<UFirstPersonCameraLocation>();
	ThirdPersonCameraLocation = FindComponentByClass<UThirdPersonCameraLocation>();
	PlayerController = Cast<AISPlayerController>(GetController());
	UMeshComponent* PlayerMesh = FindComponentByClass<UMeshComponent>();
	PlayerMaterialInstance = UMaterialInstanceDynamic::Create(PlayerMesh->GetMaterial(0), this);
	PlayerMesh->SetMaterial(0, PlayerMaterialInstance);
	PlayerMesh->SetMaterial(1, PlayerMaterialInstance);

	///Set up capsule component height
	CapsuleComponent->SetCapsuleHalfHeight(ColliderStandingHeight);
	return;
}

//Line trace out and return the first world dynamic object in reach
FHitResult AISPlayerCharacter::GetFirstWorldDynamicInReach()
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetActorEyesViewPoint(
		PlayerViewPointLocation,
		PlayerViewPointRotation
	);

	FVector LineTraceEnd = (PlayerViewPointLocation + (GetActorForwardVector() * Reach));

	///Line trace (AKA ray-cast) out to reach distance
	FHitResult HitResult;
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
		HitResult,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldDynamic),
		TraceParameters
	);

	return HitResult;
}

FVector AISPlayerCharacter::JumpCollisionTraceLocation()
{
	return FVector(0, 0, 0);
}

ADynamicCamera* AISPlayerCharacter::SpawnAndSetCamera()
{
	FVector Location(0.0f, 0.0f, 0.0f);
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	ADynamicCamera* Camera = GetWorld()->SpawnActor<ADynamicCamera>(Location, Rotation);
	GetWorld()->GetFirstPlayerController()->SetViewTarget(Camera);
	Camera->Player = this;
	return Camera;
}

void AISPlayerCharacter::LerpPlayerOpacityInvisible(float A, float B, float Alpha)
{
	float OpacityValue = FMath::Lerp(B, A, Alpha);
	if (PlayerMaterialInstance)
	{
		PlayerMaterialInstance->SetScalarParameterValue(FName("Opacity"), OpacityValue);
	}
	return;
}

void AISPlayerCharacter::LerpPlayerOpacityVisible(float A, float B, float Alpha)
{
	float OpacityValue = FMath::Lerp(A, B, Alpha);
	if (PlayerMaterialInstance)
	{
		PlayerMaterialInstance->SetScalarParameterValue(FName("Opacity"), OpacityValue);
	}
	return;
}

void AISPlayerCharacter::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (OtherActor->ActorHasTag(FName("Inside")))
		{
			InsideCount++;
			EnvironmentSwitchCamera();
		}
	}
}

void AISPlayerCharacter::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (OtherActor->ActorHasTag(FName("Inside")))
		{
			InsideCount--;
			if (InsideCount <= 0)
			{
				EnvironmentSwitchCamera();
			}
		}
	}
}