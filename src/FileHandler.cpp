#include "FileHandler.h"
#include "AdvancedPlotGUI.h"
#include "CSVPreviewDialog.h"
#include "RootDataInspector.h"

#include <TGFileDialog.h>
#include <TGMsgBox.h>
#include <TGClient.h>
#include <TSystem.h>

ClassImp(FileHandler);

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
    // Instead of opening TBrowser, open our custom inspector
    RootDataInspector* inspector = new RootDataInspector(gClient->GetRoot(), filepath);
    //inspector->ShowModal();
    if (!filepath) return;

    new RootDataInspector(gClient->GetRoot(), filepath);
    
    // Optionally still keep the file reference
    if (fCurrentRootFile) {
        fCurrentRootFile->Close();
        delete fCurrentRootFile;
    }
    fCurrentRootFile = TFile::Open(filepath);
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
