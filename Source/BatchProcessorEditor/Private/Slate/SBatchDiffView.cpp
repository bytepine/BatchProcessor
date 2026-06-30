// Copyright Byteyang Games, Inc. All Rights Reserved.

#include "Slate/SBatchDiffView.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "SBatchDiffView"

void SBatchDiffView::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SBox)
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        [
            SNew(SVerticalBox)

            + SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(HAlign_Center)
            .Padding(0.f, 0.f, 0.f, 8.f)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("Title", "Dry-run 差异视图（P3）"))
                .Font(FAppStyle::GetFontStyle("BoldFont"))
            ]

            + SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(HAlign_Center)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("Placeholder",
                    "执行「试运行」后，\n属性变更差异（改前 → 改后）将显示于此。\n\n"
                    "此功能计划在 P3 迭代中实现：\n"
                    "· 对每个资产的目标属性做「改前快照」\n"
                    "· Dry-run 执行后对比「改后快照」\n"
                    "· 以表格形式展示属性路径 / 旧值 / 新值"))
                .AutoWrapText(true)
                .Justification(ETextJustify::Center)
                .ColorAndOpacity(FSlateColor::UseSubduedForeground())
            ]
        ]
    ];
}

#undef LOCTEXT_NAMESPACE
