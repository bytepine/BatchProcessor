// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "ScannerBase.h"

bool UScannerBase::ScannerAssets(TSet<FAssetData>& Assets) const
{
	return OnScannerAssets(Assets);
}

bool UScannerBase::OnScannerAssets(TSet<FAssetData>& Assets) const
{
	return false;
}
