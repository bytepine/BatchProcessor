// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "FilterBase.h"


bool UFilterBase::Filter(const UBlueprint* Blueprint) const
{
	return OnFilter(Blueprint);
}

bool UFilterBase::OnFilter(const UBlueprint* Blueprint) const
{
	return false;
}
