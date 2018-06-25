// Fill out your copyright notice in the Description page of Project Settings.

#include "DynamicCamera.h"
#include "FirstPersonCameraLocation.h"
#include "ThirdPersonCameraLocation.h"
#include "ISPlayerCharacter.h"
#include "Engine/World.h"


///Defines for making prints and errors easier to type
#define PRINT(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT(x));}
#define PRINT_GREEN(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT(x));}
#define PRINT_RED(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT(x));}

#define WARNING(x) UE_LOG(LogTemp, Warning, TEXT(x));
#define ERROR(x) UE_LOG(LogTemp, Error, TEXT(x));

ADynamicCamera::ADynamicCamera()
{
	
}


void ADynamicCamera::BeginPlay()
{
	Super::BeginPlay();

	RegisterAllActorTickFunctions(true, false);

	Player = GetWorld()->GetFirstPlayerController();

	if (Player != nullptr)
	{
		WARNING("Player Found");
	}
	else
	{
		ERROR("Player NOT Found");
	}

	FirstPersonLocation = Player->FindComponentByClass<UFirstPersonCameraLocation>();
	ThirdPersonLocation = Player->FindComponentByClass<UThirdPersonCameraLocation>();

}

// Called every frame
void ADynamicCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	WARNING("TICKING");

	if (bIsPlayerControlled)
	{
		if (bIsFirstPerson)
		{
			SetFirstPersonLocation();
		}
		else
		{
			SetThirdPersonLocation();
		}
	}
}

void ADynamicCamera::SetFirstPersonLocation()
{
	SetActorTransform(FirstPersonLocation->GetComponentTransform());
	return;
}

void ADynamicCamera::SetThirdPersonLocation()
{
	SetActorTransform(ThirdPersonLocation->GetComponentTransform());
	UE_LOG(LogTemp, Warning, TEXT("componet transform is %s"), *ThirdPersonLocation->GetComponentTransform().ToString());
	return;
}
