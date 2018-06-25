// Fill out your copyright notice in the Description page of Project Settings.

#include "ISPlayerCharacter.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "DynamicCamera.h"
#include "Engine/World.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "FirstPersonCameraComponent.h"
#include "ThirdPersonCameraComponent.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "ViewRotator.h"
#include "Door.h"
#include "Camera/CameraActor.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

#define PRINT(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT(x));}
#define PRINT_GREEN(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT(x));}
#define PRINT_RED(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT(x));}

#define WARNING(x) UE_LOG(LogTemp, Warning, TEXT(x));
#define ERROR(x) UE_LOG(LogTemp, Error, TEXT(x));
//if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT(x));}

// Sets default values
AISPlayerCharacter::AISPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/// make sure the pawn is set up to be controlled
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	/// set turn rates for look around input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	

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

	SpawnAndSetCamera();
	SetupComponents();
}

// Called every frame
void AISPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (bShowDebugLine)
	{
		FVector PlayerViewPointLocation;
		FRotator PlayerViewPointRotation;

		GetActorEyesViewPoint(
			PlayerViewPointLocation,
			PlayerViewPointRotation
		);

		FVector LineTraceEnd = (PlayerViewPointLocation + (GetActorForwardVector() * Reach));

		DrawDebugLine(
			GetWorld(),
			PlayerViewPointLocation,
			LineTraceEnd,
			FColor(255, 0, 0),
			false,
			0.f,
			0.f,
			10.f
		);
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

	///InputAxis bindings
	PlayerInputComponent->BindAxis("MoveForward", this, &AISPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AISPlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &AISPlayerCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &AISPlayerCharacter::Turn);
}

void AISPlayerCharacter::PlayerSwitchCamera()
{
	if (bPlayerControlsCameraPerspective)
	{
		//TODO add back camera switch functionality
	}
	///Decide which camera is the active camera and switch to the other
	return;
}

//If set to true the camera will be first person
void AISPlayerCharacter::EnvironmentSwitchCamera(bool bSetFirstPerson)
{
	if (!bPlayerControlsCameraPerspective)
	{
		if (bSetFirstPerson)
		{
			bIsFirstPerson = true;
			//TODO add back camera switch functionality
		}
		else
		{
			bIsFirstPerson = false;
			//TODO add back camera switch functionality
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
			UE_LOG(LogTemp, Warning, TEXT("DOOR HIT"));
			FVector PlayerFacingDirection = GetActorForwardVector();
			Door->AttemptToOpenDoor(bHasKey, PlayerFacingDirection);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NO DOOR"));
		}
		
	}
}

void AISPlayerCharacter::StartSprint()
{
	//TODO adjust movement speed to be at max
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	return;
}

void AISPlayerCharacter::StopSprint()
{
	//TODO adjust movement speed to be at min
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	return;
}

void AISPlayerCharacter::StartCrouch()
{
	MeshComponent->SetRelativeScale3D(FVector(1.f, 1.f, 0.5f)); //TODO figure out why this isn't actually scaling
	Crouch();
	return;
}

void AISPlayerCharacter::StopCrouch()
{
	MeshComponent->SetRelativeScale3D(FVector(1.f, 1.f, 1.f)); //TODO figure out why this isn't actually scaling
	UnCrouch();
	return;
}

//Mostly Follows the Unreal Third person example project's implementation to keep things simple
void AISPlayerCharacter::MoveForward(float InputAmount)
{
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

void AISPlayerCharacter::LookUp(float InputAmount)
{
	if (bIsFirstPerson)
	{
		AddControllerPitchInput(InputAmount * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
	else
	{
		SpringArm->AddLocalRotation(FRotator(-InputAmount, 0.f, 0.f));
	}
	return;
}

void AISPlayerCharacter::Turn(float InputAmount)
{
	AddControllerYawInput(InputAmount * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	return;
}

void AISPlayerCharacter::SetupComponents()
{
	//TODO Clean up commented out lines
	//FirstPersonCamera->SetActive(false);
	//FirstPersonCamera->bUsePawnControlRotation = true;
	//ThirdPersonCamera->SetActive(true);
	MeshComponent = FindComponentByClass<UStaticMeshComponent>();
	SpringArm = FindComponentByClass<USpringArmComponent>();
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

ACameraActor* AISPlayerCharacter::SpawnAndSetCamera()
{
	FVector Location(0.0f, 0.0f, 0.0f);
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	ACameraActor* Camera = GetWorld()->SpawnActor<ADynamicCamera>(Location, Rotation);
	GetWorld()->GetFirstPlayerController()->SetViewTarget(Camera);
	return Camera;
}

void AISPlayerCharacter::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (OtherActor->ActorHasTag(FName("Inside")))
		{
			EnvironmentSwitchCamera(true);
		}
	}
}

void AISPlayerCharacter::OnOverlapEnd(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		if (OtherActor->ActorHasTag(FName("Inside")))
		{
			EnvironmentSwitchCamera(false);
		}
	}
}