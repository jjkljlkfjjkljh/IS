#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Door.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDoorEvent, bool, bOpenForward);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
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
	bool AttemptToOpenDoor(bool HasKey, FVector DirectionToOpen);

	UPROPERTY(BlueprintAssignable)
	FDoorEvent OnOpen;

	UPROPERTY(BlueprintAssignable)
	FDoorEvent OnClose;

private:
	UPROPERTY(EditAnywhere, Category = Setup)
	bool bDoorIsLocked = false;

	bool bIsOpen = false;
};
