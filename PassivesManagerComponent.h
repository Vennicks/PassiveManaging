// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PassiveObject.h"
#include "EnumPassives.h"
#include "Components/ActorComponent.h"
#include "PassivesManagerComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPassiveAdded, UPassiveObject*, NewPassive, FPassiveDefinition, PassiveDef);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPassiveRemoved, FGuid, Guid);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPassivesChanged);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAME_API UPassivesManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPassivesManagerComponent();

    UFUNCTION(BlueprintCallable)
    float DamageCalculator(bool IsDealer, float BaseDamage, const TSubclassOf<class UDamageType>& DamageType);

    UFUNCTION(BlueprintCallable)
    TArray<UPassiveObject*> GetAllPassives();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    TArray<EPassiveClass> GetAllPassivesAsEnum();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    UPassiveObject* GetPassiveById(FGuid guid);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    UPassiveObject* GetPassiveByLogicName(FName LogicName);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    UPassiveObject* GetPassiveByClass(EPassiveClass type);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    TArray<UPassiveObject*> GetAllPassivesByClass(EPassiveClass type);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    TArray<UPassiveObject*> GetAllPassivesByLogicName(FName LogicName);

    UFUNCTION(BlueprintCallable)
    void AddPassive(FPassiveDefinition Definition, FGuid Guid, UObject* Instigator);

    UFUNCTION(BlueprintCallable)
    bool HasPassive(EPassiveClass PassiveToCheck);

    UFUNCTION(BlueprintCallable)
    void RemovePassive(FGuid Guid);

    UFUNCTION(BlueprintCallable)
    void RemoveAllPassiveOfClass(EPassiveClass Class);

    UFUNCTION(BlueprintCallable)
    void RemoveAllPassives();

    UFUNCTION(BlueprintCallable)
    void UpdatePassive(float NewModifier, FGuid PassiveToUpdate);

    UPROPERTY(BlueprintAssignable)
    FOnPassiveAdded OnPassiveAdded;

    UPROPERTY(BlueprintAssignable)
    FOnPassiveRemoved OnPassiveRemoved;

    // Triggers when an Passive is added, removed or changed
    UPROPERTY(BlueprintAssignable)
    FOnPassivesChanged OnPassivesChanged;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<EPassiveClass> InsensitiveClass;

protected:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
private:
    void UpdatePassiveLifetime(FGuid PassiveGuid, float NewLifetime);

    UFUNCTION(NetMulticast, Reliable)
    void UpdatePassiveLifetime_Multicast(FGuid PassiveGuid, float NewLifetime);

    UPROPERTY(ReplicatedUsing = OnRep_ListPassives, VisibleAnywhere)
    TArray<TObjectPtr<UPassiveObject>> ListPassives;

    UFUNCTION()
    void OnRep_ListPassives(const TArray<UPassiveObject*>& OldList);
};

