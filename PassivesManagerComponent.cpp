// Fill out your copyright notice in the Description page of Project Settings.


#include "Engine/ActorChannel.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"
#include "PassivesManagerComponent.h"

UPassivesManagerComponent::UPassivesManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicatedByDefault(true);
    bReplicateUsingRegisteredSubObjectList = true;
}

void UPassivesManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    TArray<TObjectPtr<UPassiveObject>> PassivesToRemove{};
    int NumPassives = ListPassives.Num();
    for (int i = 0; i < NumPassives; i++)
    {
        TObjectPtr<UPassiveObject> Passive = ListPassives[i];
        if (!IsValid(Passive))
        {
            UE_LOG(LogTemp, Error, TEXT("Invalid Passive"));
            continue;
        }
        if (!Passive->IsAlive())
        {
            PassivesToRemove.Add(Passive);
            continue;
        }

        // Passive OnTick is already called on the server: OnTick is an RPC an re-trigger the Passive Update on the server
        Passive->OnTick(DeltaTime);

        if (!ensure(NumPassives == ListPassives.Num() && Passive == ListPassives[i]))
        {
            UE_LOG(LogTemp, Error, TEXT("Passive removed during its OnTick, breaking for loop"));
            break;
        }
        if (ListPassives[i] && !ListPassives[i]->IsAlive())
        {
            PassivesToRemove.Add(ListPassives[i]);
        }
    }

    if (GetOwner()->HasAuthority())
    {
        for (auto PassiveToDelete : PassivesToRemove)
        {
            if (PassiveToDelete)
            {
                RemovePassive(PassiveToDelete->GetGuid());
            }
        }
    }
}

void UPassivesManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UPassivesManagerComponent, ListPassives);
}

void UPassivesManagerComponent::OnRep_ListPassives(const TArray<UPassiveObject*>& OldList)
{
    TSet<UPassiveObject*> OldSet(OldList);
    TSet<UPassiveObject*> CurrentSet(ListPassives);


    TSet<UPassiveObject*> AddedPassives = CurrentSet.Difference(OldSet); // In Current but not in Old
    TSet<UPassiveObject*> RemovedPassives = OldSet.Difference(CurrentSet); // In Old but not in Current

    OnPassivesChanged.Broadcast();

    for (auto Passive : AddedPassives)
    {
        if (IsValid(Passive))
        {
            Passive->OnApply();
            OnPassiveAdded.Broadcast(Passive, Passive->Definition);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Passive was not received yet, it may be incorrectly applied"));
            // FIXME: how can we detect the object's replication and apply the Passive then?
        }
    }

    for (auto Passive : RemovedPassives)
    {
        if (IsValid(Passive))
        {
            Passive->OnRemove();
            OnPassiveRemoved.Broadcast(Passive->GetGuid());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Passive was not received yet, it may be incorrectly removed"));
        }
    }
}

float UPassivesManagerComponent::DamageCalculator(bool IsDealer, float BaseDamage,
                                               const TSubclassOf<UDamageType>& DamageType)
{
    for (auto Passive : ListPassives)
    {
        if (IsValid(Passive))
        {
            BaseDamage = Passive->DamageCalculation(IsDealer, BaseDamage, DamageType);
        }
    }
    return BaseDamage;
}

//server
void UPassivesManagerComponent::AddPassive(FPassiveDefinition Definition, FGuid Guid, UObject* Instigator)
{
    auto OldListPassive = ListPassives;
    if (GetOwner()->HasAuthority())
    {
        int PlaceToModify = -1;

        if (InsensitiveClass.Contains(Definition.PassiveClass))
        {
            return;
        }

        for (int i = 0; i < ListPassives.Num(); i++)
        {
            if (!IsValid(ListPassives[i]))
            {
                continue;
            }
            if (!ListPassives[i]->CanAdd(Guid, Definition))
            {
                return;
            }

            if (ListPassives[i]->GetPassiveName().EqualTo(Definition.Name))
            {
                PlaceToModify = i;
            }
        }

        ACharacter* owner = Cast<ACharacter>(GetOwner());
        UPassiveObject* NewPassive = UPassiveObject::MakePassive(Definition, owner, Guid, Instigator);

        if (PlaceToModify == -1)
        {
            ListPassives.Add(NewPassive);
            AddReplicatedSubObject(NewPassive);
        } else
        {
            switch (ListPassives[PlaceToModify]->GetDuplicationType())
            {
            case EPassiveDuplication::CUMULATIVE:
                ListPassives.Add(NewPassive);
                AddReplicatedSubObject(NewPassive);
                break;
                 
            case EPassiveDuplication::REFRESH:
                UpdatePassiveLifetime(ListPassives[PlaceToModify]->GetGuid(), 0);
                break;

            default:
                break;
            }
        }
        
        //Since this is server/trusted only call on_rep in order to apply the logic on all instances
        OnRep_ListPassives(OldListPassive);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Function called by client"));
    }
}


bool UPassivesManagerComponent::HasPassive(EPassiveClass PassiveToCheck)
{
    return GetPassiveByClass(PassiveToCheck) != nullptr ? true : false;
}

#pragma region Getters
UPassiveObject* UPassivesManagerComponent::GetPassiveById(FGuid guid)
{
    for (int i = 0; i < ListPassives.Num(); i++)
    {
        if (IsValid(ListPassives[i]) && ListPassives[i]->GetGuid() == guid)
        {
            return ListPassives[i];
        }
    }
    return nullptr;
}

UPassiveObject* UPassivesManagerComponent::GetPassiveByClass(EPassiveClass Class)
{
    for (auto Passive : ListPassives)
    {
        if (IsValid(Passive) && Passive->GetPassiveClass() == Class)
        {
            return Passive;
        }
    }
    return nullptr;
}

TArray<UPassiveObject*> UPassivesManagerComponent::GetAllPassiveByClass(EPassiveClass Class)
{
    TArray<UPassiveObject*> PassiveList;
    for (auto Passive : ListPassives)
    {
        if (IsValid(Passive) && Passive->GetPassiveClass() == Class)
        {
            PassiveList.Add(Passive);
        }
    }
    return PassiveList;
}

TArray<UPassiveObject*> UPassivesManagerComponent::GetAllPassiveByLogicName(FName LogicName)
{
    TArray<UPassiveObject*> tmpArray;
    for (int i = 0; i < ListPassives.Num(); i++)
    {
        if (IsValid(ListPassives[i]) && ListPassives[i]->GetLogicName() == LogicName)
        {
            tmpArray.Add(ListPassives[i]);
        }
    }
    return tmpArray;
}

UPassiveObject* UPassivesManagerComponent::GetPassiveByLogicName(FName LogicName)
{
    for (int i = 0; i < ListPassives.Num(); i++)
    {
        if (IsValid(ListPassives[i]) && ListPassives[i]->GetLogicName() == LogicName)
        {
            return ListPassives[i];
        }
    }
    return nullptr;
}

TArray<UPassiveObject*> UPassivesManagerComponent::GetAllPassives()
{
    TArray<UPassiveObject*> tmpArray;
    for (int i = 0; i < ListPassives.Num(); i++)
    {
        if (IsValid(ListPassives[i]))
        {
            tmpArray.Add(ListPassives[i]);
        }
    }
    return tmpArray;
}

TArray<EPassiveClass> UPassivesManagerComponent::GetAllPassivesAsEnum()
{
    TArray<EPassiveClass> tmpArray;
    for (int i = 0; i < ListPassives.Num(); i++)
    {
        if (IsValid(ListPassives[i]))
        {
            tmpArray.Add(ListPassives[i]->GetPassiveClass());
        }
    }
    return tmpArray;
}

#pragma endregion

#pragma region Remover
void UPassivesManagerComponent::RemovePassive(FGuid Guid)
{
    auto OldListPassive = ListPassives;
    if (GetOwner()->HasAuthority())
    {
        for (int i = 0; i < ListPassives.Num(); i++)
        {
            if (IsValid(ListPassives[i]) && ListPassives[i]->GetGuid() == Guid)
            {
                LogPassiveRemoved(ListPassives[i]);
                RemoveReplicatedSubObject(ListPassives[i]);
                ListPassives.RemoveAt(i);
            }
        }
        OnRep_ListPassives(OldListPassive);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Function called by client"));
    }
}

void UPassivesManagerComponent::RemoveAllPassiveOfClass(EPassiveClass Class)
{
    auto OldListPassive = ListPassives;
    if (GetOwner()->HasAuthority())
    {
        for (int i = 0; i < ListPassives.Num(); i++)
        {
            if (IsValid(ListPassives[i]) && ListPassives[i]->GetPassiveClass() == Class)
            {
                LogPassiveRemoved(ListPassives[i]);
                RemoveReplicatedSubObject(ListPassives[i]);
                ListPassives.RemoveAt(i);
            }
        }
        OnRep_ListPassives(OldListPassive);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Function called by client"));
    }
}

void UPassivesManagerComponent::RemoveAllPassives()
{
    auto OldListPassive = ListPassives;
    for (int i = 0; i < ListPassives.Num(); i++)
    {
        if (IsValid(ListPassives[i]))
        {
            ListPassives.RemoveAt(i);
        }
    }
    OnRep_ListPassives(OldListPassive);
}

#pragma endregion

#pragma region Updaters
void UPassivesManagerComponent::UpdatePassive(float NewModifier, FGuid PassiveToUpdate)
{
    if (GetOwner()->HasAuthority())
    {
        for (auto Passive : ListPassives)
        {
            if (IsValid(Passive) && Passive->GetGuid() == PassiveToUpdate)
            {
                Passive->UpdateModifier(NewModifier);
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Function called by client"));
    }
}

void UPassivesManagerComponent::UpdatePassiveLifetime_Multicast_Implementation(FGuid PassiveGuid, float NewLifetime)
{
    for (auto Passive : ListPassives)
    {
        if (Passive->GetGuid() == PassiveGuid)
        {
            Passive->SetCurrentLifetime(NewLifetime);
        }
    }
}
#pragma endregion
