#include "SidneyFiles.h"

#include "Sidney.h"
#include "SidneyUtil.h"
#include "Texture.h"
#include "UIButton.h"
#include "UIImage.h"
#include "UILabel.h"

void SidneyFiles::Init(Sidney* parent)
{
    // Build directory structure.
    mData.emplace_back();
    mData.back().name = "ImageDir";
    mData.back().type = SidneyFileType::Image;
    mData.emplace_back();
    mData.back().name = "AudioDir";
    mData.back().type = SidneyFileType::Audio;
    mData.emplace_back();
    mData.back().name = "TextDir";
    mData.back().type = SidneyFileType::Text;
    mData.emplace_back();
    mData.back().name = "FingerDir";
    mData.back().type = SidneyFileType::Fingerprint;
    mData.emplace_back();
    mData.back().name = "LicenseDir";
    mData.back().type = SidneyFileType::License;
    mData.emplace_back();
    mData.back().name = "ShapeDir";
    mData.back().type = SidneyFileType::Shape;

    // Build list of known files.
    //TODO: Would be cool if this was data-driven?
    mAllFiles.emplace_back(SidneyFileType::Fingerprint, "fileAbbePrint"); // 1
    mAllFiles.emplace_back(SidneyFileType::Fingerprint, "fileBuchelliPrint");
    mAllFiles.emplace_back(SidneyFileType::Fingerprint, "fileButhanePrint");
    mAllFiles.emplace_back(SidneyFileType::Fingerprint, "fileEstellePrint");
    mAllFiles.emplace_back(SidneyFileType::Fingerprint, "fileLadyHowardPrint");
    mAllFiles.emplace_back(SidneyFileType::Fingerprint, "fileMontreauxPrint");
    mAllFiles.emplace_back(SidneyFileType::Fingerprint, "fileWilkesPrint");
    mAllFiles.emplace_back(SidneyFileType::Fingerprint, "fileMoselyPrint");
    mAllFiles.emplace_back(SidneyFileType::Fingerprint, "fileLarryPrint");
    mAllFiles.emplace_back(SidneyFileType::Fingerprint, "fileWilkesPrint2"); // 10
    mAllFiles.emplace_back(SidneyFileType::Fingerprint, "fileBuchelliPrint2");
    mAllFiles.emplace_back(SidneyFileType::Fingerprint, "fileUnknownPrint1");
    mAllFiles.emplace_back(SidneyFileType::Fingerprint, "fileUnknownPrint2");
    mAllFiles.emplace_back(SidneyFileType::Fingerprint, "fileUnknownPrint3");
    mAllFiles.emplace_back(SidneyFileType::Fingerprint, "fileUnknownPrint4");
    mAllFiles.emplace_back(SidneyFileType::Fingerprint, "fileUnknownPrint5");
    mAllFiles.emplace_back(SidneyFileType::Fingerprint, "fileUnknownPrint6");
    mAllFiles.emplace_back(SidneyFileType::Fingerprint, "fileLSR1Print"); // Unused?
    mAllFiles.emplace_back(SidneyFileType::Fingerprint, "fileEstellesLSRPrint");
    mAllFiles.emplace_back(SidneyFileType::Image, "fileMap"); // 20
    mAllFiles.emplace_back(SidneyFileType::Image, "fileParchment1");
    mAllFiles.emplace_back(SidneyFileType::Image, "fileParchment2");
    mAllFiles.emplace_back(SidneyFileType::Image, "filePoussinPostcard");
    mAllFiles.emplace_back(SidneyFileType::Image, "fileTeniersPostcard1");
    mAllFiles.emplace_back(SidneyFileType::Image, "fileTeniersPostcard2");
    mAllFiles.emplace_back(SidneyFileType::Image, "fileHermeticSymbols");
    mAllFiles.emplace_back(SidneyFileType::Image, "fileSUMNote");
    mAllFiles.emplace_back(SidneyFileType::Audio, "fileAbbeTape");
    mAllFiles.emplace_back(SidneyFileType::Audio, "fileBuchelliTape");
    mAllFiles.emplace_back(SidneyFileType::Text, "fileArcadiaText"); // 30
    mAllFiles.emplace_back(SidneyFileType::Text, "fileTempleOfSolomonText"); //TODO: Is this a text type?
    mAllFiles.emplace_back(SidneyFileType::Image, "fileHermeticSymbols"); //TODO: Seems doubled up and unused?
    mAllFiles.emplace_back(SidneyFileType::License, "fileBuchelliLicense");
    mAllFiles.emplace_back(SidneyFileType::License, "fileEmilioLicense");
    mAllFiles.emplace_back(SidneyFileType::License, "fileLadyHowardLicense");
    mAllFiles.emplace_back(SidneyFileType::License, "fileMoselyLicense");
    mAllFiles.emplace_back(SidneyFileType::License, "fileWilkesLicense");

    // Set file indexes (as this is how files are referenced when adding).
    for(int i = 0; i < mAllFiles.size(); ++i)
    {
        mAllFiles[i].index = i;
    }

    // The Files UI is a bit unique. It's more of a "floating dialog" that can appear over other screens.
    // Though files UI doesn't have a background image, it's helpful to create a "dummy" rect to help with positioning things.
    {
        mRoot = new Actor(TransformType::RectTransform);
        mRoot->GetTransform()->SetParent(parent->GetTransform());

        RectTransform* rt = mRoot->GetComponent<RectTransform>();
        Texture* backgroundTexture = Services::GetAssets()->LoadTexture("S_BKGND.BMP");
        rt->SetSizeDelta(backgroundTexture->GetWidth(), backgroundTexture->GetHeight());
    }

    // Add dialog background.
    {
        // Create a root actor for the dialog.
        mDialogRoot = new Actor(TransformType::RectTransform);
        mDialogRoot->GetTransform()->SetParent(mRoot->GetTransform());

        // Create a black background.
        UIImage* backgroundImage = mDialogRoot->AddComponent<UIImage>();
        backgroundImage->SetColor(Color32::Black);

        // Receive input to avoid sending inputs to main screen below this screen.
        backgroundImage->SetReceivesInput(true);

        // Set to correct size and position.
        RectTransform* rt = backgroundImage->GetRectTransform();
        rt->SetSizeDelta(153.0f, 350.0f);
        rt->SetAnchor(0.0f, 1.0f); // Top-Left
        rt->SetPivot(0.0f, 1.0f); // Top-Left
        rt->SetAnchoredPosition(40.0f, -66.0f);
    }

    // Add close button.
    {
        Actor* closeActor = new Actor(TransformType::RectTransform);
        closeActor->GetTransform()->SetParent(mDialogRoot->GetTransform());

        UIButton* closeButton = closeActor->AddComponent<UIButton>();
        closeButton->GetRectTransform()->SetPivot(0.0f, 1.0f);
        closeButton->GetRectTransform()->SetAnchor(0.0f, 1.0f);
        closeButton->GetRectTransform()->SetAnchoredPosition(2.0f, -2.0f);

        closeButton->SetUpTexture(Services::GetAssets()->LoadTexture("CLOSEWIN_UP.BMP"));
        closeButton->SetDownTexture(Services::GetAssets()->LoadTexture("CLOSEWIN_DOWN.BMP"));
        closeButton->SetHoverTexture(Services::GetAssets()->LoadTexture("CLOSEWIN_HOVER.BMP"));
        
        closeButton->SetPressCallback([this](UIButton* button){
            Hide();
        });
    }

    // Add title/header.
    {
        Actor* titleActor = new Actor(TransformType::RectTransform);
        titleActor->GetTransform()->SetParent(mDialogRoot->GetTransform());

        UILabel* titleLabel = titleActor->AddComponent<UILabel>();
        titleLabel->GetRectTransform()->SetPivot(0.5f, 1.0f); // Top-Center
        titleLabel->GetRectTransform()->SetAnchorMin(0.0f, 1.0f);
        titleLabel->GetRectTransform()->SetAnchorMax(1.0f, 1.0f); // Fill space horizontally, anchor to top.
        titleLabel->GetRectTransform()->SetSizeDeltaY(20.0f);

        titleLabel->SetFont(Services::GetAssets()->LoadFont("SID_TEXT_18.FON"));
        titleLabel->SetText("File List");
        titleLabel->SetHorizonalAlignment(HorizontalAlignment::Center);
        titleLabel->SetVerticalAlignment(VerticalAlignment::Top);
    }

    // Hide by default.
    Hide();
}

void SidneyFiles::Show()
{
    mRoot->SetActive(true);

    // We must refresh this each time we show it, since the files may have changed since last time.
    RefreshFileListUI();
}

void SidneyFiles::Hide()
{
    mRoot->SetActive(false);
}

void SidneyFiles::AddFile(int fileIndex)
{
    if(fileIndex < 0 || fileIndex >= mAllFiles.size()) { return; }

    // Find appropriate directory.
    for(auto& dir : mData)
    {
        // Add file if not already in this directory.
        if(dir.type == mAllFiles[fileIndex].type && !dir.HasFile(mAllFiles[fileIndex].name))
        {
            dir.files.emplace_back(mAllFiles[fileIndex]);
        }
    }
}

bool SidneyFiles::HasFile(const std::string& fileName) const
{
    for(auto& dir : mData)
    {
        if(dir.HasFile(fileName))
        {
            return true;
        }
    }
    return false;
}

UILabel* SidneyFiles::GetFileListLabel()
{
    UILabel* label = nullptr;
    if(mFileListLabelIndex < mFileListLabels.size())
    {
        label = mFileListLabels[mFileListLabelIndex];
    }
    else
    {
        Actor* labelActor = new Actor(TransformType::RectTransform);
        labelActor->GetTransform()->SetParent(mDialogRoot->GetTransform());

        label = labelActor->AddComponent<UILabel>();
        label->GetRectTransform()->SetPivot(0.0f, 1.0f); // Top-Left
        label->GetRectTransform()->SetAnchor(0.0f, 1.0f); // Top-Left
        label->GetRectTransform()->SetSizeDelta(128.0f, 16.0f);

        label->SetHorizonalAlignment(HorizontalAlignment::Left);
        label->SetVerticalAlignment(VerticalAlignment::Center);

        mFileListLabels.push_back(label);
    }

    ++mFileListLabelIndex;
    return label;
}

void SidneyFiles::RefreshFileListUI()
{
    // Disable all existing labels.
    for(UILabel* label : mFileListLabels)
    {
        label->SetEnabled(false);
    }
    mFileListLabelIndex = 0;

    // Iterate and place each label.
    Vector2 topLeft(10.0f, -28.0f);
    for(auto& dir : mData)
    {
        // One unique bit of behavior: the shapes directory only appears if there's something in it.
        if(dir.type == SidneyFileType::Shape && dir.files.empty())
        {
            continue;
        }

        // Create the label for the directory itself.
        // This has a leading "-" and uses gray colored text.
        UILabel* label = GetFileListLabel();
        label->SetEnabled(true);
        label->GetRectTransform()->SetAnchoredPosition(topLeft);
        label->SetFont(Services::GetAssets()->LoadFont("SID_TEXT_14_UL.FON"));
        label->SetText("-" + SidneyUtil::GetMainScreenLocalizer().GetText(dir.name));
        topLeft.y -= 16.0f;

        // Create labels for each file within the directory.
        // These are indented slightly and use gold colored text.
        for(auto& file : dir.files)
        {
            UILabel* label = GetFileListLabel();
            label->SetEnabled(true);
            label->GetRectTransform()->SetAnchoredPosition(topLeft);
            label->SetFont(Services::GetAssets()->LoadFont("SID_TEXT_14.FON"));
            label->SetText("  " + SidneyUtil::GetAddDataLocalizer().GetText("ScanItem" + std::to_string(file.index + 1)));
            topLeft.y -= 16.0f;
        }
    }
}