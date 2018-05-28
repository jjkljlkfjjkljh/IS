// Fill out your copyright notice in the Description page of Project Settings.

#include "Door.h"
#include "GameFramework/Actor.h"


// Sets default values for this component's properties
UDoor::UDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true; //TODO possibly turn this off if tick is not used

	// ...
}


// Called when the game starts
void UDoor::BeginPlay()
{
	Super::BeginPlay();

	OriginalRotation = GetOwner()->GetActorRotation();
	Door = GetOwner();
	
}


// Called every frame
void UDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UDoor::Open(bool HasKey)
{
	//TODO Smooth Door opening
	if (bDoorIsLocked)
	{
		if (HasKey)
		{
			//Open Door
			Door->SetActorRelativeRotation(FRotator(
				OriginalRotation.Pitch, OriginalRotation.Yaw + DoorOpenAngle, OriginalRotation.Roll));
			return true;
		}
	}
	else
	{
		//Open Door
		Door->SetActorRelativeRotation(FRotator(
			OriginalRotation.Pitch, OriginalRotation.Yaw + DoorOpenAngle, OriginalRotation.Roll));
		return false;
	}
	return false;
}