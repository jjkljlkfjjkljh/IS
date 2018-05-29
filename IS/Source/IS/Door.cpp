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
	
}


// Called every frame
void UDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UDoor::AttemptToOpenDoor(bool HasKey, FVector PlayerFacingDirection)
{
	///Determine which direction to open the door
	bool bOpenForward;
	FVector DoorForwardVector = GetOwner()->GetActorForwardVector();
	///Check if player forward vector is closest to the doors forward vector or if they are opposite
	//If this value is greater than 0 both vectors are facing each other. If they are less than 0 they are facing the same direction
	float DotProduct = FVector::DotProduct(PlayerFacingDirection, DoorForwardVector);
	if (DotProduct > 0)
	{
		bOpenForward = false;
	}
	else
	{
		bOpenForward = true;
	}

	if (bIsOpen)
	{
		bIsOpen = false;
		OnClose.Broadcast(bOpenForward);
		return false;
	}
	else
	{
		if (bDoorIsLocked)
		{
			if (HasKey)
			{
				//Open Door
				OnOpen.Broadcast(bOpenForward);
				bDoorIsLocked = false;
				bIsOpen = true;
				return true;
			}
		}
		else
		{
			//Open Door
			OnOpen.Broadcast(bOpenForward);
			bIsOpen = true;
			return false;
		}
	}
	return false;
}