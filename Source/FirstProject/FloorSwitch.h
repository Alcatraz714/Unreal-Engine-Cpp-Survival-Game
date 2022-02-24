// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorSwitch.generated.h"

UCLASS()
class FIRSTPROJECT_API AFloorSwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloorSwitch();
	// area for trigger
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	class UBoxComponent* TriggerBox;
	// Floor switch 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	class UStaticMeshComponent* FloorSwitch;
	// open a door 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
    UStaticMeshComponent* Door;
	//initial vector
	UPROPERTY(BlueprintReadWrite, Category = "Floor Switch")
	FVector InitialDoorLocation;
	//switch location initial
	UPROPERTY(BlueprintReadWrite, Category = "Floor Switch")
	FVector InitialSwitchLocation;

	FTimerHandle SwitchHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	float SwitchTime;

	bool bCharacterOnSwitch;

	void CloseDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "FloorSwitch")
	void RaiseDoor();
	UFUNCTION(BlueprintImplementableEvent, Category = "FloorSwitch")
	void LowerDoor();

	UFUNCTION(BlueprintImplementableEvent, Category = "FloorSwitch")
	void RaiseFloorSwitch();
	UFUNCTION(BlueprintImplementableEvent, Category = "FloorSwitch")
	void LowerFloorSwitch();

	//update door location - event
	UFUNCTION(BlueprintCallable, Category = "FloorSwitch")
	void UpdateDoorLocation(float Z);
	
	//update floor swicth location - event
	UFUNCTION(BlueprintCallable, Category = "FloorSwitch")
	void UpdateFloorSwitchLocation(float Z);
};
