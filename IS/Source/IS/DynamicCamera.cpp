#include "DynamicCamera.h"
#include "FirstPersonCameraLocation.h"
#include "ThirdPersonCameraLocation.h"
#include "ISPlayerCharacter.h"
#include "ViewRotator.h"
#include "TransformCheck.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"


///Defines for making prints and errors easier to type
#define PRINT(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT(x));}
#define PRINT_GREEN(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT(x));}
#define PRINT_RED(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT(x));}

#define WARNING(x) UE_LOG(LogTemp, Warning, TEXT(x));
#define ERROR(x) UE_LOG(LogTemp, Error, TEXT(x));

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
		LerpToNewTransform(false, TargetTransform, MainOverrideDistance, 1, DeltaTime);
		return;
	}

	//if none of the above conditions need met then set the default first person location
	SetActorTransform(TargetTransform);

	return;
}
//Set the camera to match the third person location on the player character
void ADynamicCamera::SetThirdPersonLocation(FTransform TargetTransform,
	FVector PlayerPosition, bool bIsFalling, bool bIsCrouching, float DeltaTime)
{
	if (!bJumpStartLocationSet)
	{
		JumpStartLocation = GetActorLocation();
	}

	//jump to the correct position if in a transition from one location to another
	if (bWasFirstPerson || bTransitioning)
	{
		bWasFirstPerson = false;
		bTransitioning = true;
		LerpToNewTransform(false, TargetTransform, MainOverrideDistance, 1, DeltaTime);
		return;
	}

	//Prevent the camera from going inside the player. If the camera is too close reposition it above the player
	if ((PlayerPosition - TargetTransform.GetLocation()).Size() < ClosestDistanceAllowed)
	{
		RepositionThirdPersonCamera(TargetTransform, PlayerPosition);
		return;
	}

	//position the camera if the player is jumping or in the air
	if (bIsFalling)
	{
		bInAir = true;
		bJumpStartLocationSet = true;
		CameraPositionDuringJump(TargetTransform, PlayerPosition);
		return;
	}
	else
	{
		bJumpStartLocationSet = false;
		if (bInAir)
		{
			bJustLanded = true;
			bInAir = false;
		}
	}

	//Make sure the camera lerps back to a normal position after the player lands from a jump or fall
	if (bJustLanded)
	{
		LerpToNewTransform(false, TargetTransform, MainOverrideDistance, 1, DeltaTime);
		return;
	}

	//Change camera to follow crouching behavior
	if (bIsCrouching)
	{
		bOriginalTargetHeightSet = true;
		bCrouchStartLocationSet = true;
		CameraPositionDuringCrouch(TargetTransform, PlayerPosition, DeltaTime);
		bFinishedCrouchTransition = false;
		return;
	}
	else
	{
		bOriginalTargetHeightSet = false;
		bCrouchStartLocationSet = false;
		if (!bFinishedCrouchTransition)
		{
			LerpToNewTransform(false, TargetTransform, MainOverrideDistance, 1, DeltaTime);
			return;
		}
	}

	if (!bCrouchStartLocationSet)
	{
		CrouchStartLocation = GetActorLocation();
	}
	if (!bOriginalTargetHeightSet)
	{
		OriginalTargetLocation = Player->FindComponentByClass<UViewRotator>()->GetComponentLocation();
	}

	//if none of the above conditions need met then set the default third person location
	SetActorTransform(TargetTransform);
	return;
}

//Lerp from the current camera position to the target position
void ADynamicCamera::LerpToNewTransform(bool RotationMatters, FTransform Target, float AlphaOverrideDistance, float TimeScale, float DeltaTime)
{
	Alpha += (DeltaTime * AlphaMultiplier * TimeScale);

	//check if the target location is reached or if it is close enough to set the true location
	//TODO fix the sudden jerkiness of the override distance
	if ((Alpha >= 1.0f) || ((this->GetActorLocation() - Target.GetLocation()).Size() < AlphaOverrideDistance ))
	{
		//set the exact transform of the target and reset lerp values
		SetActorTransform(Target);
		bTransitioning = false;
		Alpha = 0.0f;
		if (bJustLanded)
		{
			bJustLanded = false;
		}
		if (!bFinishedCrouchTransition)
		{
			bFinishedCrouchTransition = true;
		}
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
	//TODO possible change the lerp to always look a tthe player while lerping only location

	///Calculate an alpha value between 0 and 1 for where the camera
	///is in relation to the center of the player and the spring arm
	float DistanceFromPlayerAlpha = 1 - (((PlayerPosition - Target.GetLocation()).Size()) / ClosestDistanceAllowed);

	///Decide how high to adjust the camera to move smoothly from
	///the spring arm location to directly above the player based on the alpha value
	float NewLocationZ = Target.GetLocation().Z + (MaxHeightAbovePlayer * DistanceFromPlayerAlpha);

	SetActorLocation(FVector(Target.GetLocation().X, Target.GetLocation().Y, NewLocationZ));
	SetActorRotation(Target.Rotator());
	return;
}

//Called to handle the camera while the player is jumping or falling
void ADynamicCamera::CameraPositionDuringJump(FTransform TargetTransform, FVector PlayerPosition)
{
	//TODO fix the camera for when the player moves the camera up or down during a jump

	///Set the location of the camera to match the height of the start of the jump but the player controlled position
	FVector NewCameraLocation = FVector(TargetTransform.GetLocation().X, TargetTransform.GetLocation().Y, JumpStartLocation.Z);
	SetActorLocation(NewCameraLocation);

	///get the look direction from the camera to the player and set the correct rotation
	FVector CameraLocation = GetActorLocation();
	FVector PlayerLocation = Player->FindComponentByClass<UViewRotator>()->GetComponentLocation();
	FVector LookDirection = PlayerLocation - CameraLocation;
	LookDirection.Normalize();
	SetActorRotation(LookDirection.Rotation());
	return;
}

void ADynamicCamera::CameraPositionDuringCrouch(FTransform TargetTransform, FVector PlayerPosition, float DeltaTime)
{
	///Set the location of the camera to match the height of the start of the crouch but the player controlled position
	FVector NewCameraLocation = FVector(TargetTransform.GetLocation().X, TargetTransform.GetLocation().Y, CrouchStartLocation.Z);
	SetActorLocation(NewCameraLocation);

	///get the look direction from the camera to the player and set the correct rotation
	FVector CameraLocation = GetActorLocation();
	FVector PlayerLocation = FVector(Player->FindComponentByClass<UViewRotator>()->GetComponentLocation().X,
		Player->FindComponentByClass<UViewRotator>()->GetComponentLocation().Y,
		OriginalTargetLocation.Z);
	FVector LookDirection = PlayerLocation - CameraLocation;
	LookDirection.Normalize();
	SetActorRotation(LookDirection.Rotation());
	return;
}