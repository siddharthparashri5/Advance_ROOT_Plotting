#include "FileHandler.h"
#include "AdvancedPlotGUI.h"
#include "CSVPreviewDialog.h"
#include "ROOTFileBrowser.h"
#include "RootDataInspector.h"
#include "DataReader.h"
#include "RootEntrySelector.h"
#include "ROOTBranchSelectorDialog.h"

#include <TGFileDialog.h>
#include <TGNumberEntry.h>
#include <TGMsgBox.h>
#include "PopupControl.h"
#include <TGClient.h>
#include <TSystem.h>
#include <TBrowser.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TTree.h>
#include <TBranch.h>
#include <TObjArray.h>
#include <fstream>
#include <sstream>

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
std::string FileHandler::Browse()
{
    const char* filetypes[] = {
        "All files", "*",
        "ROOT files", "*.root",
        "CSV files", "*.csv",
        "Text files", "*.txt",
        "Data files", "*.dat",
        nullptr, nullptr
    };
    
    TGFileInfo fileInfo;
    fileInfo.fFileTypes = filetypes;
    
    new TGFileDialog(gClient->GetRoot(), fMainGUI, kFDOpen, &fileInfo);
    
    if (fileInfo.fFilename && strlen(fileInfo.fFilename) > 0) {
        fMainGUI->SetFilePath(fileInfo.fFilename);
        return std::string(fileInfo.fFilename);
    }
    
    return "";
}

// ============================================================================
// Open Entry Selector
// ============================================================================

void FileHandler::OpenEntrySelector(const char* filepath)
{
    // Verify it's a ROOT file
    TString fname(filepath);
    if (!fname.EndsWith(".root")) {
        ShowMsgBox(gClient->GetRoot(), fMainGUI,
            "Not a ROOT file", 
            "Entry selector works only with .root files",
            kMBIconExclamation, kMBOk);
        return;
    }
    
    // Open the entry selector dialog
    new RootEntrySelector(gClient->GetRoot(), filepath);
}

// ============================================================================
// Load file based on type
// ============================================================================
void FileHandler::Load(const std::string& filepath)
{
    if (filepath.empty()) {
        ShowMsgBox(gClient->GetRoot(), fMainGUI,
            "Error", "Please enter a file path.",
            kMBIconExclamation, kMBOk);
        return;
    }

    // Check if ROOT file
    TString filename(filepath.c_str());
    if (filename.EndsWith(".root")) {
        LoadRootFile(filepath.c_str());
        return;
    }

    // Check if CSV file - show preview dialog
    if (filename.EndsWith(".csv")) {
        LoadCSVFile(filepath.c_str());
        return;
    }

    // Load other text data using DataReader
    if (!DataReader::ReadFile(filepath, fCurrentData)) {
        ShowMsgBox(gClient->GetRoot(), fMainGUI,
            "Error", "Failed to load data file. Check console for details.",
            kMBIconStop, kMBOk);
        return;
    }

    fMainGUI->EnablePlotControls(true);

    ShowMsgBox(gClient->GetRoot(), fMainGUI,
        "Success", Form("Data loaded successfully!\nRows: %d\nColumns: %d",
            fCurrentData.GetNumRows(), fCurrentData.GetNumColumns()),
        kMBIconAsterisk, kMBOk);
}



// ────────────────────────────────────────────────────────────────────────────
// NEW METHOD  — FileHandler::LoadROOTIntoGUI()
//
// Called by:
//   - The "ROOT Analysis → Load into GUI" button  (kEntrySelectorLoadGUI)
//   - The yes/no dialog inside LoadRootFile() above
//   - Directly from AdvancedPlotGUI::ProcessMessage() for the Browse button
//     when the user selects a .root file and wants Add Plot to work
// ────────────────────────────────────────────────────────────────────────────
bool FileHandler::LoadROOTIntoGUI(const char* filepath)
{
    // ROOTBranchSelectorDialog* dlg =new ROOTBranchSelectorDialog(fMainGUI, filepath);
    ROOTBranchSelectorDialog* dlg = new ROOTBranchSelectorDialog(gClient->GetRoot(), filepath);

    Int_t ret = dlg->DoModal();

    if (ret != 1) {
        delete dlg;
        return false;
    }

    // Copy the populated ColumnData out of the dialog
    fCurrentData = dlg->GetColumnData();
    delete dlg;

    if (fCurrentData.GetNumColumns() == 0 || fCurrentData.GetNumRows() == 0) {
        ShowMsgBox(gClient->GetRoot(), fMainGUI,
            "Warning",
            "No data was loaded (empty result).\n"
            "Try selecting different branches or a smaller entry range.",
            kMBIconExclamation, kMBOk);
        return false;
    }

    // Enable the Add Plot and Create Plots buttons
    fMainGUI->EnablePlotControls(kTRUE);

    ShowMsgBox(gClient->GetRoot(), fMainGUI,
        "ROOT Data Loaded",
        Form("Successfully loaded into GUI!\n\n"
             "Columns (branches): %d\n"
             "Rows (entries):     %d\n\n"
             "You can now use 'Add Plot...' to configure plots.",
             fCurrentData.GetNumColumns(),
             fCurrentData.GetNumRows()),
        kMBIconAsterisk, kMBOk);

    return true;
}


// ============================================================================
// Load ROOT file with content browser
// ============================================================================
/*
void FileHandler::LoadRootFile(const char* filepath)
{
    // Close previous file
    if (fCurrentRootFile) {
        fCurrentRootFile->Close();
        delete fCurrentRootFile;
        fCurrentRootFile = nullptr;
    }

    // Open the browser dialog
    ROOTFileBrowser* browser = new ROOTFileBrowser(gClient->GetRoot(), filepath);
    
    Int_t ret = browser->DoModal();
    
    if (ret == 0) {
        // User cancelled or error
        gSystem->ProcessEvents();
        gSystem->Sleep(100);
        delete browser;
        return;
    }
    
    Bool_t showBrowser = browser->ShowBrowser();
    std::vector<ROOTObjectInfo> selectedObjects = browser->GetSelectedObjects();
    
    // Clean up dialog
    gSystem->ProcessEvents();
    gSystem->Sleep(100);
    delete browser;
    
    // Open the file for our use
    fCurrentRootFile = TFile::Open(filepath, "READ");
    if (!fCurrentRootFile || fCurrentRootFile->IsZombie()) {
        ShowMsgBox(gClient->GetRoot(), nullptr,
            "Error", Form("Cannot open ROOT file:\n%s", filepath),
            kMBIconStop, kMBOk);
        fCurrentRootFile = nullptr;
        return;
    }
    
    // If user wants TBrowser, open it
    if (showBrowser || ret == 2) {
        new TBrowser("browser", fCurrentRootFile);
    }
    
    // Handle selected objects
    if (ret == 1 && !selectedObjects.empty()) {
        std::cout << "\n=== Loading " << selectedObjects.size() << " objects ===" << std::endl;
        
        for (const auto& objInfo : selectedObjects) {
            std::cout << "Loading: " << objInfo.name << " (" << objInfo.type << ")" << std::endl;
            
            // Get the object from file
            TObject* obj = fCurrentRootFile->Get(objInfo.name.c_str());
            if (!obj) {
                std::cout << "  WARNING: Could not retrieve object!" << std::endl;
                continue;
            }
            
            // Handle different object types
            if (objInfo.category == "Histogram") {
                PlotHistogram(obj, objInfo.name.c_str());
            } else if (objInfo.category == "Graph") {
                PlotGraph(obj, objInfo.name.c_str());
            } else if (objInfo.category == "Tree") {
                ShowTreeInfo(obj, objInfo.name.c_str());
            }
        }
        
        std::cout << "=== Loading complete ===" << std::endl;
    }
    
    // Show summary message
    if (ret == 1) {
        ShowMsgBox(gClient->GetRoot(), fMainGUI,
            "ROOT File Loaded",
            Form("Loaded %d objects from ROOT file.\n\n"
                 "Objects have been plotted in separate canvases.",
                 (int)selectedObjects.size()),
            kMBIconAsterisk, kMBOk);
    }
}

*/

void FileHandler::LoadRootFile(const char* filepath)
{
    // Close previous file
    if (fCurrentRootFile) {
        fCurrentRootFile->Close();
        delete fCurrentRootFile;
        fCurrentRootFile = nullptr;
    }

    // Open the object-browser dialog (existing ROOTFileBrowser)
    ROOTFileBrowser* browser = new ROOTFileBrowser(gClient->GetRoot(), filepath);
    Int_t ret = browser->DoModal();

    if (ret == 0) {                         // user cancelled
        gSystem->ProcessEvents();
        gSystem->Sleep(100);
        delete browser;
        return;
    }

    Bool_t  showBrowser     = browser->ShowBrowser();
    std::vector<ROOTObjectInfo> selectedObjects = browser->GetSelectedObjects();
    gSystem->ProcessEvents();
    gSystem->Sleep(100);
    delete browser;

    // Open the file for our use
    fCurrentRootFile = TFile::Open(filepath, "READ");
    if (!fCurrentRootFile || fCurrentRootFile->IsZombie()) {
        ShowMsgBox(gClient->GetRoot(), nullptr,
            "Error", Form("Cannot open ROOT file:\n%s", filepath),
            kMBIconStop, kMBOk);
        fCurrentRootFile = nullptr;
        return;
    }

    // ── ret == 2  →  TBrowser only ──────────────────────────────────────────
    if (ret == 2 || showBrowser) {
        new TBrowser("browser", fCurrentRootFile);
        return;
    }

    // ── ret == 1  →  "Load for GUI"  ────────────────────────────────────────
    if (ret == 1) {
        // Direct-plot the objects (existing behaviour)
        for (const auto& objInfo : selectedObjects) {
            TObject* obj = fCurrentRootFile->Get(objInfo.name.c_str());
            if (!obj) continue;
            if (objInfo.category == "Histogram") PlotHistogram(obj, objInfo.name.c_str());
            else if (objInfo.category == "Graph") PlotGraph(obj, objInfo.name.c_str());
            else if (objInfo.category == "Tree")  ShowTreeInfo(obj, objInfo.name.c_str());
        }

        // NOW also ask if the user wants to load into the plot-config workflow
        Int_t answer = 0;
        ShowMsgBox(gClient->GetRoot(), fMainGUI,
            "Load into GUI?",
            "Objects plotted directly.\n\n"
            "Do you also want to load a TTree/histogram into the\n"
            "main GUI so you can use 'Add Plot' for custom plots?",
            kMBIconQuestion, kMBYes | kMBNo, &answer);

        if (answer == kMBYes) {
            LoadROOTIntoGUI(filepath);
        }
    }
}

// ============================================================================
// Load CSV file with preview dialog
// ============================================================================
void FileHandler::LoadCSVFile(const char* filepath)
{
    CSVPreviewDialog* preview = new CSVPreviewDialog(gClient->GetRoot(), filepath);
    
    Int_t ret = preview->DoModal();
    
    if (ret != 1) {
        // User cancelled
        gSystem->ProcessEvents();
        gSystem->Sleep(100);
        delete preview;
        return;
    }
    
    char    delimiter = preview->GetDelimiter();
    Int_t   skipRows  = preview->GetSkipRows();
    Bool_t  useHeader = preview->UseHeaderRow();
    
    // Clean up dialog before proceeding
    gSystem->ProcessEvents();
    gSystem->Sleep(100);
    delete preview;
    
    LoadCSVWithSettings(filepath, delimiter, skipRows, useHeader);
}

// ============================================================================
// Load CSV with user-specified settings
// ============================================================================
void FileHandler::LoadCSVWithSettings(const char* filepath, char delim, 
                                      Int_t skipRows, Bool_t useHeader)
{
    // CRITICAL: Clear old data first
    fCurrentData = ColumnData();

    {
        std::ifstream testOpen(filepath);
        if (!testOpen.is_open()) {
            ShowMsgBox(gClient->GetRoot(), fMainGUI,
                "Error", Form("Cannot open: %s", filepath),
                kMBIconStop, kMBOk);
            return;
        }
    }

    // NOTE: This used to be a hand-rolled parser that assumed every column
    // was numeric — any non-numeric token (e.g. a string category column)
    // was silently dropped via `catch(...) { /* skip */ }`, which desynced
    // every column after it row-by-row. It also had a leftover debug check
    // (`if (dataRowCount == 0) Warning(...)`) that fired on the very first
    // data row regardless of whether any data was found.
    //
    // DataReader::ReadCSVFile already correctly classifies each column as
    // numeric or string (string columns go to fCurrentData.stringHeaders /
    // stringData, keeping row alignment intact), so delegate to it here
    // instead of duplicating that logic.
    bool ok = DataReader::ReadCSVFile(std::string(filepath), fCurrentData,
                                       delim, (int)skipRows, (bool)useHeader);

    // CRITICAL: Check data validity and enable controls
    bool hasData = ok && fCurrentData.GetNumRows() > 0;

    if (hasData) {
        fMainGUI->EnablePlotControls(kTRUE);

        std::string extra;
        if (fCurrentData.GetNumStringColumns() > 0) {
            extra = Form("\nString columns: %d (usable as labels / category axis)",
                          fCurrentData.GetNumStringColumns());
        }

        ShowMsgBox(gClient->GetRoot(), fMainGUI,
            "Success", Form("CSV loaded successfully!\n\nNumeric columns: %d\nRows: %d%s",
                fCurrentData.GetNumColumns(),
                fCurrentData.GetNumRows(),
                extra.c_str()),
            kMBIconAsterisk, kMBOk);
    } else {
        ShowMsgBox(gClient->GetRoot(), fMainGUI,
            "Warning", "No numeric data found in file.\n"
                       "Check delimiter and format.",
            kMBIconExclamation, kMBOk);
    }
}

// ============================================================================
// Helper: Plot histogram
// ============================================================================
void FileHandler::PlotHistogram(TObject* obj, const char* name)
{
    if (!obj) return;
    
    TH1* hist = dynamic_cast<TH1*>(obj);
    if (!hist) {
        std::cout << "  WARNING: Object is not a histogram!" << std::endl;
        return;
    }
    
    // Create new canvas
    static int canvasCounter = 0;
    TCanvas* c = new TCanvas(Form("c_hist_%d", canvasCounter++),
                             Form("Histogram: %s", name),
                             800, 600);
    
    hist->Draw();
    c->Update();
    
    std::cout << "  ✓ Histogram plotted in canvas: " << c->GetName() << std::endl;
}

// ============================================================================
// Helper: Plot graph
// ============================================================================
void FileHandler::PlotGraph(TObject* obj, const char* name)
{
    if (!obj) return;
    
    TGraph* graph = dynamic_cast<TGraph*>(obj);
    if (!graph) {
        std::cout << "  WARNING: Object is not a graph!" << std::endl;
        return;
    }
    
    // Create new canvas
    static int canvasCounter = 0;
    TCanvas* c = new TCanvas(Form("c_graph_%d", canvasCounter++),
                             Form("Graph: %s", name),
                             800, 600);
    
    graph->Draw("AP");
    c->Update();
    
    std::cout << "  ✓ Graph plotted in canvas: " << c->GetName() << std::endl;
}

// ============================================================================
// Helper: Show TTree info
// ============================================================================
void FileHandler::ShowTreeInfo(TObject* obj, const char* name)
{
    if (!obj) return;
    
    TTree* tree = dynamic_cast<TTree*>(obj);
    if (!tree) {
        std::cout << "  WARNING: Object is not a TTree!" << std::endl;
        return;
    }
    
    std::cout << "\n=== TTree Information ===" << std::endl;
    std::cout << "Name: " << tree->GetName() << std::endl;
    std::cout << "Title: " << tree->GetTitle() << std::endl;
    std::cout << "Entries: " << tree->GetEntries() << std::endl;
    std::cout << "Branches: " << tree->GetNbranches() << std::endl;
    
    // Show branch info
    TObjArray* branches = tree->GetListOfBranches();
    if (branches) {
        std::cout << "\nBranches:" << std::endl;
        for (int i = 0; i < branches->GetEntries() && i < 10; i++) {
            TBranch* branch = (TBranch*)branches->At(i);
            if (branch) {
                std::cout << "  • " << branch->GetName();
                if (branch->GetClassName()[0]) {
                    std::cout << " [" << branch->GetClassName() << "]";
                }
                std::cout << std::endl;
            }
        }
        if (branches->GetEntries() > 10) {
            std::cout << "  ... and " << (branches->GetEntries() - 10) << " more branches" << std::endl;
        }
    }
    std::cout << "=========================\n" << std::endl;
    
    // Show message box
    ShowMsgBox(gClient->GetRoot(), fMainGUI,
        "TTree Information",
        Form("TTree: %s\n\nEntries: %lld\nBranches: %d\n\n"
             "See console for branch details.\n"
             "Note: TTree plotting from browser not yet implemented.",
             tree->GetName(), tree->GetEntries(), tree->GetNbranches()),
        kMBIconAsterisk, kMBOk);
}
