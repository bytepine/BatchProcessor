// Copyright Byteyang Games, Inc. All Rights Reserved.


#include "BatchProgressReporter.h"

#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

void FSlateBatchProgressReporter::OnBegin()
{
	FNotificationInfo Info(FText::FromString(TEXT("批处理开始")));
	Info.bFireAndForget = false;
	Info.bUseThrobber = true;
	Info.bUseSuccessFailIcons = false;
	Info.bUseLargeFont = false;
	ProgressNotification = FSlateNotificationManager::Get().AddNotification(Info);
}

void FSlateBatchProgressReporter::OnProgress(const FString& Message)
{
	if (ProgressNotification.IsValid())
	{
		ProgressNotification->SetText(FText::FromString(Message));
		ProgressNotification->SetCompletionState(SNotificationItem::CS_Pending);
	}
}

void FSlateBatchProgressReporter::OnFinished(bool bSuccess, const FString& Message)
{
	if (ProgressNotification.IsValid())
	{
		ProgressNotification->SetText(FText::FromString(Message));
		ProgressNotification->SetCompletionState(bSuccess ? SNotificationItem::CS_Success : SNotificationItem::CS_Fail);
		ProgressNotification->SetFadeOutDuration(3.0f);
		ProgressNotification->Fadeout();
	}
}
