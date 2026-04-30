// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PassiveDefinition.h"
#include "PassivesData.h"
#include "SharedData/Enums.h"

#include "PassiveObject.generated.h"

USTRUCT()
struct FListMaterials
{
    GENERATED_BODY()
    UPROPERTY()
    TArray<UMaterialInterface*> ListMaterials;
};

UCLASS(Blueprintable)
class GAME_API UPassiveObject : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="PassivesHandling | Maker")
    static UPassiveObject* MakePassive(FPassiveDefinition Definition, ACharacter* CurrentPlayer, FGuid Guid,
                                 UObject* PassiveInstigator);

    UPROPERTY(Replicated, BlueprintReadOnly)
    FPassiveDefinition Definition;

    UFUNCTION(BlueprintCallable, Category=PassivesHandling)
    virtual void OnApply();

    UFUNCTION(BlueprintCallable, Category=PassivesHandling)
    virtual void OnRemove();

    UFUNCTION(BlueprintCallable, Category=PassivesHandling)
    virtual void OnTick(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category="PassivesHandling | Check")
    virtual bool CanAdd(FGuid GUID, FPassiveDefinition Def) { return GetGuid() != GUID; }

    UFUNCTION(BlueprintCallable, Category=PassivesHandling)
    virtual void ResetPassive() { CurrentLifeTime = 0; }

    UFUNCTION(BlueprintCallable, Category=PassivesHandling)
    virtual float DamageCalculation(bool IsDealer, float InitialDamages,
                                    const TSubclassOf<class UDamageType>& DamageType) { return InitialDamages; }

    //----Getter
    UFUNCTION(BlueprintCallable, Category="PassivesHandling | Check")
    bool const IsAlive();

    UFUNCTION(BlueprintCallable, Category="PassivesHandling | Getter")
    TArray<TEnumAsByte<ECharacterStatus>> GetStatusModifiers() { return StatusModifiers; }


    UFUNCTION(BlueprintCallable, Category = "PassivesHandling | Getter")
    FORCEINLINE FName GetLogicName() const { return Definition.LogicName; }

    UFUNCTION(BlueprintCallable, Category="PassivesHandling | Getter")
    FORCEINLINE FText GetPassiveName() const { return Definition.Name; }

    UFUNCTION(BlueprintCallable, Category="PassivesHandling | Getter")
    FORCEINLINE EPassiveClass GetPassiveClass() const { return Definition.PassiveClass; }

    UFUNCTION(BlueprintCallable, Category="PassivesHandling | Getter")
    FORCEINLINE EPassiveDuplication GetDuplicationType() const { return Definition.DuplicationType; }

    UFUNCTION(BlueprintCallable, Category="PassivesHandling | Getter")
    FORCEINLINE FGuid GetGuid() const { return Guid; }

    UFUNCTION(BlueprintCallable, Category="PassivesHandling | Getter")
    FORCEINLINE float GetLifeTime() const { return CurrentLifeTime; }

    UFUNCTION(BlueprintCallable, Category = "PassivesHandling | Getter")
    FORCEINLINE float GetMaxLifeTime() const { return Definition.LifeTime; }

    UFUNCTION(BlueprintCallable, Category = "PassivesHandling | Getter")
    FORCEINLINE UObject* GetInstigator() const { return Instigator.Get(); }

    UFUNCTION(BlueprintCallable, Category = "PassivesHandling | Getter")
    FORCEINLINE UTexture2D* GetBuffIcons() const { return Definition.BuffIcons; }

    virtual void UpdateModifier(float Modifier) {};

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameplayTags | Utils")
    static FGameplayTag GetGameplayTagFromString(const FString& TagString) { return FGameplayTag::RequestGameplayTag(FName(*TagString)); }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameplayTags | Utils")
    static FString GetGameplayTagToString(const FGameplayTag& Tag) { return Tag.ToString(); }

    //----Setter
    UFUNCTION(BlueprintCallable, Category = "PassivesHandling | Setter")
    void SetCurrentLifetime(float LifeTime);

protected:
    float CurrentLifeTime = 0;
    float ToDestroy = false;


    UPROPERTY(Replicated, BlueprintReadOnly)
    TObjectPtr<ACharacter> Player;

    UPROPERTY()
    TArray<TEnumAsByte<ECharacterStatus>> StatusModifiers;

    UPROPERTY()
    TObjectPtr<UNiagaraComponent> OnApplyVFXComponent = nullptr;

    UPROPERTY()
    TObjectPtr<UNiagaraComponent> OnRemoveVFXComponent = nullptr;

    UPROPERTY()
    TObjectPtr<UAudioComponent> OnApplySFXComponent = nullptr;

    UPROPERTY()
    TObjectPtr<UAudioComponent> OnRemoveSFXComponent = nullptr;

    UPROPERTY(Replicated, BlueprintReadOnly)
    TObjectPtr<UObject> Instigator = nullptr;

    UPROPERTY(Replicated)
    FGuid Guid;

    UPROPERTY()
    TMap<TObjectPtr<class USkeletalMeshComponent>, FListMaterials> SKPlayerMaterials;

    UPROPERTY()
    TMap<TObjectPtr<class UGroomComponent>, FListMaterials> GRPlayerMaterials;

    UPROPERTY()
    TMap<TObjectPtr<class UChaosClothComponent>, FListMaterials> CLPlayerMaterials;
    
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual bool IsSupportedForNetworking() const override { return true; }
};
