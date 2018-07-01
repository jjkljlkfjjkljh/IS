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

}

//Set the camera to amtch the first person location on the player character
void ADynamicCamera::SetFirstPersonLocation(FTransform TargetTransform, float DeltaTime)
{
	//jump to the correct position if in a transition from one location to another
	if (!bWasFirstPerson || bTransitioning)
	{
		bWasFirstPerson = true;
		bTransitioning = true;
		LerpToNewTransform(TargetTransform, DeltaTime);
		return;
	}

	//if none of the above conditions need met then set the default first person location
	SetActorTransform(TargetTransform);
	return;
}
//Set the camera to amtch the third person location on the player character
void ADynamicCamera::SetThirdPersonLocation(FTransform TargetTransform, FVector PlayerPosition, float DeltaTime)
{
	//jump to the correct position if in a transition from one location to another
	if (bWasFirstPerson || bTransitioning)
	{
		bWasFirstPerson = false;
		bTransitioning = true;
		LerpToNewTransform(TargetTransform, DeltaTime);
		return;
	}
	//Prevent the camera from going inside the player. If the camera is too close reposition it above the player
	if ((PlayerPosition - TargetTransform.GetLocation()).Size() < ClosestDistanceAllowed)
	{
		RepositionThirdPersonCamera(TargetTransform, PlayerPosition);
		return;
	}

	//if none of the above conditions need met then set the default third person location
	SetActorTransform(TargetTransform);
	return;
}

//Lerp from the current camera position to the target position
void ADynamicCamera::LerpToNewTransform(FTransform Target, float DeltaTime)
{
	Alpha += (DeltaTime * AlphaMultiplier);

	//check if the target location is reached
	if ((Alpha >= 1.0f) || ((this->GetActorLocation() - Target.GetLocation()).Size() < 10.0f ))
	{
		//set the exact location of the target and reset lerp values
		SetActorTransform(Target);
		bTransitioning = false;
		Alpha = 0.0f;
		return;
	}
	
	//Lerp between the current position and the target 
	FTransform CurrentPosition = GetActorTransform();

	FVector LerpVector = FMath::Lerp<FVector>(CurrentPosition.GetLocation(), Target.GetLocation(), Alpha);
	FRotator LerpRotator = FMath::Lerp<FRotator>(CurrentPosition.Rotator(), Target.Rotator(), Alpha);

	SetActorLocation(LerpVector);
	SetActorRotation(LerpRotator);

	return;
}

//Find a new location above the player to position the camera
void ADynamicCamera::RepositionThirdPersonCamera(FTransform Target, FVector PlayerPosition)
{
	float DistanceFromPlayerAlpha = 1 - (((PlayerPosition - Target.GetLocation()).Size()) / ClosestDistanceAllowed);

	UE_LOG(LogTemp, Warning, TEXT("Distnace Alpha is: %f"), DistanceFromPlayerAlpha);

	float NewLocationZ = Target.GetLocation().Z + (MaxHeightAbovePlayer * DistanceFromPlayerAlpha);

	SetActorLocation(FVector(Target.GetLocation().X, Target.GetLocation().Y, NewLocationZ));
	SetActorRotation(Target.Rotator());

	return;
}