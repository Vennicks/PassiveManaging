// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EPassiveClass : uint8
{
    NONE UMETA(Display="None"),

    //Following values are what I used in my own projected but feel free to remove and set the one you would use
    STUN UMETA(Display="Stun"),
    DAMAGEOVERTIME UMETA(Display = "Damage over time"),
    DAMAGEINCREASE UMETA(Display = "Damage increase"),
    ROOT UMETA(Display="Root"),
    SHIELD UMETA(Display="Shield"),
    DAMAGEREDUCTION UMETA(Display="Damage reduction"),
    MOVESPEED UMETA(Display="Move speed"),
    STEALTH UMETA(Display="Stealth"),
    REGENERATION UMETA(Display="Regeneration"),
    STASIS UMETA(Display="Stasis"),
    COOLDOWN UMETA(Display = "Ability Cooldown"),
    INTANGIBLE UMETA(Display = "Intangible"),

    LAST UMETA(Display="Last")
};

UENUM(BlueprintType, meta=(BitFlags, UseEnumAsMaskValuesInEditor = "true"))
enum EStatus
{
    CANMOVE = 0,
    CANUSEABILITY = 1,
    CANATTACK = 2,
    CANRECEIVEDAMAGE = 3,
    CANHEAL = 4,
    VISIBLE = 5,
};

UENUM(BlueprintType)
enum class EPassiveDuplication : uint8
{
    NONE UMETA(Display="None"),
    REFRESH UMETA(Display="REFRESH"),
    CUMULATIVE UMETA(Display="Cumulative"),
    LAST UMETA(Display="Last")
};