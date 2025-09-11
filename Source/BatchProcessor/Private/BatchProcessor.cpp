#include "BatchProcessor.h"

#include "BatchBase.h"
#include "BlueprintEditor.h"
#include "EditorStyleSet.h"
#include "Toolkits/AssetEditorToolkitMenuContext.h"

class FBlueprintEditor;
DEFINE_LOG_CATEGORY(LogBatchProcessor);

#define LOCTEXT_NAMESPACE "FBatchProcessorModule"

void FBatchProcessorModule::StartupModule()
{
	// 注册蓝图编辑器工具栏扩展
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateStatic(&FBatchProcessorModule::RegisterBlueprintEditorToolbar));
}

void FBatchProcessorModule::ShutdownModule()
{
    
}

void FBatchProcessorModule::RegisterBlueprintEditorToolbar()
{
	UToolMenu* Toolbar = UToolMenus::Get()->ExtendMenu("AssetEditor.BlueprintEditor.ToolBar");
	FToolMenuSection& Section = Toolbar->AddSection("BatchProcessing");
	// 执行批处理
	Section.AddDynamicEntry("ExecuteBatchProcessing", FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& InSection)
	{
		const UAssetEditorToolkitMenuContext* MenuContext = InSection.FindContext<UAssetEditorToolkitMenuContext>();
		if (!MenuContext || !MenuContext->Toolkit.IsValid()) return;
		
		FBlueprintEditor* BlueprintEditor = static_cast<FBlueprintEditor*>(MenuContext->Toolkit.Pin().Get());
		if (BlueprintEditor && BlueprintEditor->GetBlueprintObj() && BlueprintEditor->GetBlueprintObj()->GeneratedClass->IsChildOf<UBatchBase>())
		{
			InSection.AddEntry(FToolMenuEntry::InitToolBarButton(
				"ExecuteBatchProcessing",
				FUIAction(
					FExecuteAction::CreateLambda([BlueprintEditor]()
					{
						if (UBatchBase* Processor = BlueprintEditor->GetBlueprintObj()->GeneratedClass->GetDefaultObject<UBatchBase>())
						{
							Processor->Start();
						}
					}),
					FCanExecuteAction::CreateLambda([]() { return true; })
				),
				LOCTEXT("ExecuteBatchProcessing", "执行批处理"),
				LOCTEXT("ExecuteBatchProcessing_Tooltip", "执行批处理操作"),
				FSlateIcon(FAppStyle::GetAppStyleSetName(), "Sequencer.PlaybackOptions")
			));
		}
	}));
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FBatchProcessorModule, BatchProcessor)