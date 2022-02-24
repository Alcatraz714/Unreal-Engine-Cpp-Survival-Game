// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstSaveGame.h"

UFirstSaveGame::UFirstSaveGame()
{
	PlayerName = TEXT("Default");

	UserIndex = 0;

	CharacterStats.WeaponName = TEXT("");//none for no weapon
	CharacterStats.LevelName = TEXT("");//set map name to none
}