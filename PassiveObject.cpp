// Fill out your copyright notice in the Description page of Project Settings.


#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "ChaosClothAsset/ClothComponent.h"
#include "GroomComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


UPassiveObject* UPassiveObject::MakePassive(FPassiveDefinition Definition, AActor* Actor, FGuid Guid,
                                   UObject* PassiveInstigator)
{
    UPassiveObject* NewPassive;
    switch (Definition.PassiveClass)
    {
        //Insert different types of Passiveclass
    default:
        UE_LOG(LogTemp, Error, TEXT("Trying to create an unset Passive."));
        NewPassive = NewObject<UPassiveObject>(Actor->FindComponentByClass<UPassivesManagerComponent>(););
    }

    //Copy modifier
    NewPassive->Definition = Definition;

    NewPassive->Instigator = PassiveInstigator;
    NewPassive->Guid = Guid;

    NewPassive->CurrentActor = Actor;
    return NewPassive;
}

void UPassiveObject::OnApply()
{
    if (Player->HasAuthority())
    {
        for (auto Status : StatusModifiers)
        {
            //Handle StatusModifier here
        }
    }
    
    //Handle player feedbacks
    if (IsValid(Definition.VFXOnApply))
    {
        OnApplyVFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(Definition.VFXOnApply, Player->GetMesh(), "root", FVector::Zero(),
                                                                                FRotator::ZeroRotator,
                                                                                EAttachLocation::SnapToTarget, true);
    }

    if (IsValid(Definition.SFXOnApply))
    {
        OnApplySFXComponent = UGameplayStatics::SpawnSoundAttached(Definition.SFXOnApply.Get(), Player->GetRootComponent(), "root");
    }

    if (IsValid(Definition.Shader))
    {
        for(auto comp : Player->K2_GetComponentsByClass(USkeletalMeshComponent::StaticClass()))
        {
            if(auto sm = Cast<USkeletalMeshComponent>(comp))
            {
                FListMaterials ListMat = {sm->GetMaterials()};
                SKPlayerMaterials.Add(sm, ListMat);
                for (int i = 0; i != sm->GetMaterials().Num(); i++)
                {
                    sm->SetMaterial(i, Definition.Shader);
                }
            }
        }
            
        for(auto comp : Player->K2_GetComponentsByClass(UChaosClothComponent::StaticClass()))
        {
            if (auto cl = Cast<UChaosClothComponent>(comp))
            {
                FListMaterials ListMat = {cl->GetMaterials()};
                CLPlayerMaterials.Add(cl, ListMat);
                for (int i = 0; i != cl->GetMaterials().Num(); i++)
                {
                    cl->SetMaterial(i, Definition.Shader);
                }
            }
        }

        for(auto comp : Player->K2_GetComponentsByClass(UGroomComponent::StaticClass()))
        {
            if (auto cl = Cast<UGroomComponent>(comp))
            {
                FListMaterials ListMat = {cl->GetMaterials()};
                GRPlayerMaterials.Add(cl, ListMat);
                for (int i = 0; i != cl->GetMaterials().Num(); i++)
                {
                    cl->SetMaterial(i, Definition.Shader);
                }
            }
        }
    }
}

void UPassiveObject::OnRemove()
{
    if (Player->HasAuthority())
    {
        for (auto Status : StatusModifiers)
        {
            //Handle StatusModifier here
        }
    }
    
    //Remove player feedbacks
    if (OnApplyVFXComponent && OnApplyVFXComponent->IsValidLowLevel())
    {
        OnApplyVFXComponent->DestroyComponent();
    }

    if (OnApplySFXComponent && OnApplySFXComponent->IsValidLowLevel())
    {
        OnApplySFXComponent->Stop();
        OnApplySFXComponent->DestroyComponent();
    }

    //Remove feedbacks
    if (IsValid(Definition.VFXOnRemove))
    {
        OnRemoveVFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(Definition.VFXOnRemove, Player->GetMesh(), "root", FVector::Zero(),
                                                                                FRotator::ZeroRotator,
                                                                                EAttachLocation::SnapToTarget, true);
    }

    if (IsValid(Definition.SFXOnRemove))
    {
        OnRemoveSFXComponent = UGameplayStatics::SpawnSoundAttached(Definition.SFXOnRemove.Get(), Player->GetRootComponent(),"root");
    }

    if (!SKPlayerMaterials.IsEmpty())
    {
        for (auto comp : Player->K2_GetComponentsByClass(USkeletalMeshComponent::StaticClass()))
        {
            if (auto sm = Cast<USkeletalMeshComponent>(comp))
            {
                if (auto SMatList = SKPlayerMaterials.Find(sm))
                {
                    int i = 0;
                    for (auto mat : SMatList->ListMaterials)
                    {
                        sm->SetMaterial(i++, mat);
                    }
                }
            }
        }

        SKPlayerMaterials.Empty();
    }

    if (!GRPlayerMaterials.IsEmpty())
    {
        for (auto comp : Player->K2_GetComponentsByClass(UGroomComponent::StaticClass()))
        {
            if (auto gr = Cast<UGroomComponent>(comp))
            {
                if (auto SMatList = GRPlayerMaterials.Find(gr))
                {
                    int i = 0;
                    for (auto mat : SMatList->ListMaterials)
                    {
                        gr->SetMaterial(i++, mat);
                    }
                }
            }
        }
        GRPlayerMaterials.Empty();
    }
        
    if (!CLPlayerMaterials.IsEmpty())
    {
        for (auto comp : Player->K2_GetComponentsByClass(UChaosClothComponent::StaticClass()))
        {
            if (auto cl = Cast<UChaosClothComponent>(comp))
            {
                if (auto SMatList = CLPlayerMaterials.Find(cl))
                {
                    int i = 0;
                    for (auto mat : SMatList->ListMaterials)
                    {
                        cl->SetMaterial(i++, mat);
                    }
                }
            }
        }
        CLPlayerMaterials.Empty();
    }
}

bool const UPassiveObject::IsAlive()
{
    if (ToDestroy)
        return false;
    if (Definition.LifeTime == -1)
        return true;
    if (Definition.LifeTime <= CurrentLifeTime)
        return false;
    return true;
}

void UPassiveObject::SetCurrentLifetime(float LifeTime)
{
    CurrentLifeTime = LifeTime;
}

void UPassiveObject::OnTick(float DeltaTime)
{
    CurrentLifeTime += DeltaTime;
}

void UPassiveObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UPassiveObject, Player);
    DOREPLIFETIME(UPassiveObject, Instigator);
    DOREPLIFETIME(UPassiveObject, Guid);

    DOREPLIFETIME(UPassiveObject, Definition);
}
