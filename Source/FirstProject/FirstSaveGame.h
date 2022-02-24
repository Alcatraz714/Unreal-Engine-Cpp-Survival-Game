// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FirstSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

	//player stats
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Health;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Stamina;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxStamina;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int32 Coins;

	// spawn location
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FVector Location;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FRotator Rotation;

	//weapon
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FString WeaponName;

	//weapon
	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FString LevelName;

};

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API UFirstSaveGame : public USaveGame
{
	GENERATED_BODY()
public:

	UFirstSaveGame();

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	FString PlayerName;

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	uint32 UserIndex;

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	FCharacterStats CharacterStats;
	
};
