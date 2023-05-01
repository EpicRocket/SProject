//Copyright(c) 2021 FinalRockstar All Rights Reserved


#include "MortarActorsObject.h"


void UMortarActorsObject::SetActorsObject(const TArray<AActor*>& Actors)
{
	ActorsObject = Actors;
}

TArray<AActor*> UMortarActorsObject::GetActorsObject()
{
	return ActorsObject;
}
