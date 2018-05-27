// Fill out your copyright notice in the Description page of Project Settings.

#include "ISPlayerCharacter.h"
#include "Components/InputComponent.h"
//#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "FirstPersonCameraComponent.h"
#include "ThirdPersonCameraComponent.h"

// Sets default values
AISPlayerCharacter::AISPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	/// make sure the pawn is set up to be controlled
	AutoPossessPlayer = EAutoReceiveInput::Player0;

}

// Called when the game starts or when spawned
void AISPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	Player = this;

	FindCameraComponents();
	SetupComponents();
}

// Called every frame
void AISPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AISPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAction("SwitchCamera", IE_Pressed, this, &AISPlayerCharacter::SwitchCamera);
	InputComponent->BindAction("Jump", IE_Pressed, this, &AISPlayerCharacter::Jump);
}

void AISPlayerCharacter::SwitchCamera()
{
	UE_LOG(LogTemp, Warning, TEXT("Switch"));
	///Decide which camera is the active camera and switch to the other
	if (FirstPersonCamera->IsActive())
	{
		FirstPersonCamera->SetActive(false);
		ThirdPersonCamera->SetActive(true);
	}
	else if (ThirdPersonCamera->IsActive())
	{
		FirstPersonCamera->SetActive(true);
		ThirdPersonCamera->SetActive(false);
	}
	else
	{
		FirstPersonCamera->SetActive(true);
		ThirdPersonCamera->SetActive(false);
	}
	return;
}


void AISPlayerCharacter::Jump()
{
	/*
	if (!MovementComponent->IsFalling())
	{
		UE_LOG(LogTemp, Warning, TEXT("Jump"));

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Falling"));
	}
	*/
}


void AISPlayerCharacter::FindCameraComponents()
{
	FirstPersonCamera = FindComponentByClass<UFirstPersonCameraComponent>();
	ThirdPersonCamera = FindComponentByClass<UThirdPersonCameraComponent>();
}

void AISPlayerCharacter::SetupComponents()
{
	FirstPersonCamera->SetActive(true);
	ThirdPersonCamera->SetActive(false);
	//MovementComponent = FindComponentByClass<UCharacterMovementComponent>();
}