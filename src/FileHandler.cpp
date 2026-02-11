#include "FileHandler.h"
#include "AdvancedPlotGUI.h"
#include "CSVPreviewDialog.h"
#include "RootDataInspector.h"

#include <TGFileDialog.h>
#include <TGMsgBox.h>
#include <TGClient.h>
#include <TSystem.h>


// ============================================================================
// Constructor
// ============================================================================
FileHandler::FileHandler(AdvancedPlotGUI* mainGUI)
    : fMainGUI(mainGUI),
      fCurrentRootFile(nullptr)
{
}

// ============================================================================
// Destructor
// ============================================================================
FileHandler::~FileHandler()
{
    if (fCurrentRootFile) {
        fCurrentRootFile->Close();
        delete fCurrentRootFile;
        fCurrentRootFile = nullptr;
    }
}

// ============================================================================
// Browse for file
// ============================================================================
void FileHandler::Browse()
{
    const char* filetypes[] = {
        "All files", "*",
        "ROOT files", "*.root",
        "CSV files", "*.csv",
        "Text files", "*.txt;*.dat",
        nullptr, nullptr
    };
    
    TGFileInfo fileInfo;
    fileInfo.fFileTypes = filetypes;
    
    new TGFileDialog(gClient->GetRoot(), fMainGUI, kFDOpen, &fileInfo);
    
    if (fileInfo.fFilename) {
        fMainGUI->SetFilePath(fileInfo.fFilename);
    }
}

// ============================================================================
// Load file based on type
// ============================================================================
void FileHandler::Load()
{
    const char* filepath = fMainGUI->GetFilePath();
    if (!filepath || strlen(filepath) == 0) {
        new TGMsgBox(gClient->GetRoot(), fMainGUI,
            "Error", "Please enter a file path or drag a file.",
            kMBIconExclamation, kMBOk);
        return;
    }

    // Check if ROOT file
    TString filename(filepath);
    if (filename.EndsWith(".root")) {
        // This should have been handled by LoadRootFile already
        // But if called directly, handle it here too
        if (!fCurrentRootFile || TString(fCurrentRootFile->GetName()) != filename) {
            LoadRootFile(filepath);
        }
        return;
    }

    // Load CSV/text data
    if (!DataReader::ReadFile(filepath, fCurrentData)) {
        new TGMsgBox(gClient->GetRoot(), fMainGUI,
            "Error", "Failed to load data file. Check console for details.",
            kMBIconStop, kMBOk);
        return;
    }

    fMainGUI->EnablePlotControls(true);

    new TGMsgBox(gClient->GetRoot(), fMainGUI,
        "Success", Form("Data loaded successfully!\nRows: %zu\nColumns: %zu",
            fCurrentData.data.size(), fCurrentData.headers.size()),
        kMBIconAsterisk, kMBOk);
}

// ============================================================================
// Load file from drag-and-drop
// ============================================================================
void FileHandler::LoadFromDrop(const char* filepath)
{
    if (!filepath || strlen(filepath) == 0) {
        return;
        //FileHandler->LoadFromDrop(filepath); // this should show preview
    }

    fMainGUI->SetFilePath(filepath);
    
    // Determine file type and load accordingly
    TString filename(filepath);
    
    if (filename.EndsWith(".root")) {
        LoadRootFile(filepath);
    } else if (filename.EndsWith(".csv") || filename.EndsWith(".txt") || 
               filename.EndsWith(".dat")) {
        LoadCSVFile(filepath);
    } else {
        Load();
    }
}

// ============================================================================
// Load ROOT file with inspector dialog
// ============================================================================
void FileHandler::LoadRootFile(const char* filepath)
{
    // Close previous file
    if (fCurrentRootFile) {
        fCurrentRootFile->Close();
        delete fCurrentRootFile;
        fCurrentRootFile = nullptr;
    }

    fCurrentRootFile = TFile::Open(filepath, "READ");
    if (!fCurrentRootFile || fCurrentRootFile->IsZombie()) {
        new TGMsgBox(gClient->GetRoot(), nullptr,
            "Error", Form("Cannot open ROOT file:\n%s", filepath),
            kMBIconStop, kMBOk);
        fCurrentRootFile = nullptr;
        return;
    }

    // Wrap RootDataInspector (a TGGroupFrame) in a transient window
    TGTransientFrame* win = new TGTransientFrame(
        gClient->GetRoot(), nullptr, 400, 500);
    win->SetWindowName(Form("ROOT Inspector — %s",
        gSystem->BaseName(filepath)));
    win->SetCleanup(kDeepCleanup);

    RootDataInspector* inspector = new RootDataInspector(win, filepath);
    inspector->LoadFile(filepath);

    win->AddFrame(inspector, new TGLayoutHints(
        kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
    win->MapSubwindows();
    win->Resize(win->GetDefaultSize());
    win->MapWindow();
}

// ============================================================================
// Load CSV file with preview dialog
// ============================================================================
void FileHandler::LoadCSVFile(const char* filepath)
{
    CSVPreviewDialog* preview = new CSVPreviewDialog(gClient->GetRoot(), filepath);
    preview->DoModal();
    
    // After user configures CSV, load the actual data
    Load();
}