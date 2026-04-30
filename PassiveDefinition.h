// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Engine/DataTable.h"
#include "PassiveDefinition.generated.h"

/**
 * 
 */
USTRUCT(Blueprintable)
struct GAME_API FPassiveDefinition : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
    FName LogicName{};

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
    FText Name{};

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
    EPassiveClass PassiveClass = EPassiveClass::NONE;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
    EPassiveDuplication DuplicationType = EPassiveDuplication::CUMULATIVE;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Modifier")
    float LifeTime = -1; //-1 = infinite

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Modifier")
    float Modifier1 = 0.;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Modifier")
    float Modifier2 = 0.;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Modifier")
    float Modifier3 = 0.;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Modifier")
    float Modifier4 = 0.;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Modifier")
    float Modifier5 = 0.;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Modifier")
    FString StringModifier = "";

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Modifier")
    FString StringModifierReference = "";

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Feedbacks")
    TObjectPtr<UCurveFloat> CurveModifier = nullptr;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Feedbacks")
    TObjectPtr<UNiagaraSystem> VFXOnApply = nullptr;;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Feedbacks")
    TObjectPtr<UNiagaraSystem> VFXOnRemove = nullptr;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Feedbacks")
    TObjectPtr<USoundBase> SFXOnApply = nullptr;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Feedbacks")
    TObjectPtr<USoundBase> SFXOnRemove = nullptr;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Feedbacks")
    TObjectPtr<UMaterial> Shader = nullptr;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Feedbacks|UI")
    TObjectPtr<UTexture2D> BuffIcons;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Feedbacks|UI")
    FText PassiveExplanationText;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Feedbacks|UI")
    bool ShouldDisplayPassive = true;
};
