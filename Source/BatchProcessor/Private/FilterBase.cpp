// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "FilterBase.h"

void UFilterBase::Filter(TSet<FAssetData>& Assets)
{
	OnFilter(Assets);
}

void UFilterBase::OnFilter(TSet<FAssetData>& Assets)
{
	
}
