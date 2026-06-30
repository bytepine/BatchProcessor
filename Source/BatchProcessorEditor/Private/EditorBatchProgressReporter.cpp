// Copyright Byteyang Games, Inc. All Rights Reserved.

#include "EditorBatchProgressReporter.h"

void FEditorBatchProgressReporter::OnBegin()
{
    bIsRunning     = true;
    bLastSucceeded = false;
    CurrentMessage.Reset();
    FinishedMessage.Reset();
    OnStateChanged.Broadcast();
}

void FEditorBatchProgressReporter::OnProgress(const FString& Message)
{
    CurrentMessage = Message;
    OnStateChanged.Broadcast();
}

void FEditorBatchProgressReporter::OnFinished(bool bSuccess, const FString& Message)
{
    bIsRunning      = false;
    bLastSucceeded  = bSuccess;
    FinishedMessage = Message;
    CurrentMessage.Reset();
    OnStateChanged.Broadcast();
}
