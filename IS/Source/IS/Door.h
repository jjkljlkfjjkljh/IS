// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Door.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IS_API UDoor : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDoor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Attempts to open door and returns true if a key is used
	bool Open(bool HasKey);

	UPROPERTY(EditAnywhere, Category = Setup)
	bool bDoorIsLocked = false;

	FRotator OriginalRotation;

	AActor* Door = nullptr;

	float DoorOpenAngle = -80.f;
	
};
