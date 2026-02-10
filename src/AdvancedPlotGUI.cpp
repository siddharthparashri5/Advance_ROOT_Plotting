#include "AdvancedPlotGUI.h"
#include "DataReader.h"
#include "PlotTypes.h"    // defines both PlotConfig and PlotCreator
#include "FitUtils.h"
#include "ErrorHandling.h"
#include "FileHandler.h"

#include <TGClient.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGNumberEntry.h>
#include <TGLayout.h>
#include <TGListBox.h>
#include <TGTextEdit.h>
#include <TGComboBox.h>
#include <TGLabel.h>
#include <TGMsgBox.h>
#include <TGTextView.h>
#include <TGTextBuffer.h>
#include <TGFrame.h>

#include <TApplication.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TRint.h>

ClassImp(AdvancedPlotGUI);

// ============================================================================
// Constructor
// ============================================================================
AdvancedPlotGUI::AdvancedPlotGUI(const TGWindow* p, UInt_t w, UInt_t h) 
    : TGMainFrame(p, w, h),
      fMainFrame(this),
      fFileHandler(nullptr),
      fPlotManager(nullptr),
      fScriptEngine(nullptr)
{
    // Create manager objects
    fFileHandler = new FileHandler(this);
    fPlotManager = new PlotManager(this);
    fScriptEngine = new ScriptEngine(this);
    
    // Build GUI sections
    BuildFileSection();
    BuildPlotConfigSection();
    BuildCanvasOptionsSection();
    BuildFitSection();
    BuildScriptPanel();
    
    // Finalize GUI
    SetWindowName("Advanced ROOT Plotting Tool");
    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();
}

// ============================================================================
// Destructor
// ============================================================================
AdvancedPlotGUI::~AdvancedPlotGUI()
{
    delete fFileHandler;
    delete fPlotManager;
    delete fScriptEngine;
}



// ============================================================================
// Build file section
// ============================================================================
void AdvancedPlotGUI::BuildFileSection()
{
    TGGroupFrame* fileGroup = new TGGroupFrame(this, "Data File");
    TGHorizontalFrame* fileFrame = new TGHorizontalFrame(fileGroup);

    // Create text buffer and DropTextEntry (supports drag & drop)
    TGTextBuffer* buffer = new TGTextBuffer(512);
    fFileEntry = new DropTextEntry(fileFrame, buffer, this);
    fFileEntry->SetToolTipText("Drag & drop a file here or type a path");
    fileFrame->AddFrame(fFileEntry, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,2,2));

    // Browse button
    fBrowseButton = new TGTextButton(fileFrame, "Browse...", kBrowseButton);
    fBrowseButton->Associate(this);
    fileFrame->AddFrame(fBrowseButton, new TGLayoutHints(kLHintsLeft, 5,5,2,2));

    fileGroup->AddFrame(fileFrame, new TGLayoutHints(kLHintsExpandX, 5,5,5,5));
    AddFrame(fileGroup, new TGLayoutHints(kLHintsExpandX, 5,5,5,5));
}

// ============================================================================
// Build plot configuration section
// ============================================================================
void AdvancedPlotGUI::BuildPlotConfigSection()
{
    TGGroupFrame* plotGroup = new TGGroupFrame(this, "Plot Configuration");
    
    fAddPlotButton = new TGTextButton(plotGroup, "Add Plot...", kAddPlotButton);
    fAddPlotButton->Associate(this);
    plotGroup->AddFrame(fAddPlotButton, new TGLayoutHints(kLHintsCenterX, 5,5,5,5));
    fAddPlotButton->SetEnabled(false);
    
    fPlotListBox = new TGListBox(plotGroup);
    fPlotListBox->Resize(100, 100);
    plotGroup->AddFrame(fPlotListBox, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5,5,5,5));
    
    TGHorizontalFrame* plotButtonFrame = new TGHorizontalFrame(plotGroup);
    fRemovePlotButton = new TGTextButton(plotButtonFrame, "Remove Selected", kRemovePlotButton);
    fRemovePlotButton->Associate(this);
    fClearAllButton = new TGTextButton(plotButtonFrame, "Clear All", kClearAllButton);
    fClearAllButton->Associate(this);
    plotButtonFrame->AddFrame(fRemovePlotButton, new TGLayoutHints(kLHintsCenterX, 5,5,2,2));
    plotButtonFrame->AddFrame(fClearAllButton, new TGLayoutHints(kLHintsCenterX, 5,5,2,2));
    plotGroup->AddFrame(plotButtonFrame, new TGLayoutHints(kLHintsCenterX, 5,5,5,5));
    
    AddFrame(plotGroup, new TGLayoutHints(kLHintsExpandX, 5,5,5,5));
}

// ============================================================================
// Build canvas options section
// ============================================================================
void AdvancedPlotGUI::BuildCanvasOptionsSection()
{
    // Canvas title
    TGHorizontalFrame* titleFrame = new TGHorizontalFrame(this);
    TGLabel* titleLabel = new TGLabel(titleFrame, "Canvas Title:");
    titleFrame->AddFrame(titleLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,2,2));
    fCanvasTitleEntry = new TGTextEntry(titleFrame);
    fCanvasTitleEntry->SetText("hist");
    fCanvasTitleEntry->Resize(300, 20);
    titleFrame->AddFrame(fCanvasTitleEntry, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,2,2));
    AddFrame(titleFrame, new TGLayoutHints(kLHintsExpandX, 5,5,5,5));

    // Canvas division + overlay
    TGHorizontalFrame* divFrame = new TGHorizontalFrame(this);
    fSameCanvasCheck = new TGCheckButton(divFrame, "Same canvas (Overlay)");
    divFrame->AddFrame(fSameCanvasCheck, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,2,2));
    
    fDividedCanvasCheck = new TGCheckButton(divFrame, "Divide Canvas:");
    divFrame->AddFrame(fDividedCanvasCheck, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,2,2));

    divFrame->AddFrame(new TGLabel(divFrame, "Rows:"), new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,2,2,2));
    fNRowsEntry = new TGNumberEntry(divFrame, 2, 3, -1,
                               TGNumberFormat::kNESInteger,
                               TGNumberFormat::kNEAPositive,
                               TGNumberFormat::kNELLimitMinMax, 1, 10);
    fNRowsEntry->Resize(50, 20);
    divFrame->AddFrame(fNRowsEntry, new TGLayoutHints(kLHintsLeft, 2,5,2,2));
    
    divFrame->AddFrame(new TGLabel(divFrame, "Cols:"), new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,2,2,2));
    fNColsEntry = new TGNumberEntry(divFrame, 2, 3, -1,
                               TGNumberFormat::kNESInteger,
                               TGNumberFormat::kNEAPositive,
                               TGNumberFormat::kNELLimitMinMax, 1, 10);
    fNColsEntry->Resize(50, 20);
    divFrame->AddFrame(fNColsEntry, new TGLayoutHints(kLHintsLeft, 2,5,2,2));
    
    AddFrame(divFrame, new TGLayoutHints(kLHintsLeft, 5,5,5,5));
}

// ============================================================================
// Build fit section
// ============================================================================
void AdvancedPlotGUI::BuildFitSection()
{
    TGHorizontalFrame* fitFrame = new TGHorizontalFrame(this);
    
    fitFrame->AddFrame(new TGLabel(fitFrame, "Fit Function:"), 
        new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,2,2));
    fFitFunctionCombo = new TGComboBox(fitFrame);
    fFitFunctionCombo->AddEntry("No Fit", FitUtils::kNoFit);
    fFitFunctionCombo->AddEntry("Gaussian", FitUtils::kGaus);
    fFitFunctionCombo->AddEntry("Linear", FitUtils::kLinear);
    fFitFunctionCombo->AddEntry("Pol2", FitUtils::kPol2);
    fFitFunctionCombo->AddEntry("Pol3", FitUtils::kPol3);
    fFitFunctionCombo->AddEntry("Exponential", FitUtils::kExpo);
    fFitFunctionCombo->AddEntry("Sine + Offset", FitUtils::kSineOffset);
    fFitFunctionCombo->AddEntry("Custom Function", FitUtils::kCustom);
    fFitFunctionCombo->Select(FitUtils::kNoFit);
    fFitFunctionCombo->Resize(150, 20);
    fitFrame->AddFrame(fFitFunctionCombo, new TGLayoutHints(kLHintsLeft, 5,5,2,2));

    fitFrame->AddFrame(new TGLabel(fitFrame, "Custom Fit:"), 
        new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,2,2));
    fCustomFuncEntry = new TGTextEntry(fitFrame);
    fCustomFuncEntry->SetText("[0]*x^2 + [1]*x + [2]");
    fCustomFuncEntry->Resize(200, 20);
    fitFrame->AddFrame(fCustomFuncEntry, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, 5,5,2,2));

    AddFrame(fitFrame, new TGLayoutHints(kLHintsExpandX, 5,5,5,5));

    // Plot button
    fPlotButton = new TGTextButton(this, "Create Plots", kPlotButton);
    fPlotButton->Associate(this);
    AddFrame(fPlotButton, new TGLayoutHints(kLHintsCenterX, 5,5,10,10));
    fPlotButton->SetEnabled(false);
}

// ============================================================================
// Build script panel
// ============================================================================
void AdvancedPlotGUI::BuildScriptPanel()
{
    TGGroupFrame* scriptGroup = new TGGroupFrame(this, "Script Panel");
    AddFrame(scriptGroup, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5,5,5,5));

    TGHorizontalFrame* scriptControlFrame = new TGHorizontalFrame(scriptGroup);

    TGLabel* langLabel = new TGLabel(scriptControlFrame, "Language:");
    scriptControlFrame->AddFrame(langLabel, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,2,2));

    fScriptLangCombo = new TGComboBox(scriptControlFrame);
    fScriptLangCombo->AddEntry("ROOT/C++", 0);
    fScriptLangCombo->AddEntry("Python", 1);
    fScriptLangCombo->Select(0);
    fScriptLangCombo->Resize(120, 20);
    scriptControlFrame->AddFrame(fScriptLangCombo, new TGLayoutHints(kLHintsLeft, 5,5,2,2));

    TGTextButton* runScriptButton = new TGTextButton(scriptControlFrame, "Run Script", kRunScriptButton);
    runScriptButton->Associate(this);
    scriptControlFrame->AddFrame(runScriptButton, new TGLayoutHints(kLHintsLeft, 5,5,2,2));

    TGTextButton* loadScriptButton = new TGTextButton(scriptControlFrame, "Load...", kLoadScriptButton);
    loadScriptButton->Associate(this);
    scriptControlFrame->AddFrame(loadScriptButton, new TGLayoutHints(kLHintsLeft, 5,5,2,2));

    TGTextButton* saveScriptButton = new TGTextButton(scriptControlFrame, "Save...", kSaveScriptButton);
    saveScriptButton->Associate(this);
    scriptControlFrame->AddFrame(saveScriptButton, new TGLayoutHints(kLHintsLeft, 5,5,2,2));

    TGTextButton* clearEditorButton = new TGTextButton(scriptControlFrame, "Clear Editor", kClearEditorButton);
    clearEditorButton->Associate(this);
    scriptControlFrame->AddFrame(clearEditorButton, new TGLayoutHints(kLHintsLeft, 5,5,2,2));

    scriptGroup->AddFrame(scriptControlFrame, new TGLayoutHints(kLHintsExpandX, 5,5,5,2));

    // Script editor
    fScriptEditor = new TGTextEdit(scriptGroup, 400, 150);
    fScriptEditor->LoadBuffer(
        "// Write ROOT/C++ or Python code here\n"
        "// Example:\n"
        "// TCanvas *c = new TCanvas();\n"
        "// TH1F *h = new TH1F(\"h\",\"Histogram\",100,-3,3);\n"
        "// h->FillRandom(\"gaus\",10000);\n"
        "// h->Draw();\n"
    );
    scriptGroup->AddFrame(fScriptEditor, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5,5,2,5));

    // Output area
    TGGroupFrame* outputGroup = new TGGroupFrame(scriptGroup, "Output / Terminal");
    scriptGroup->AddFrame(outputGroup, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5,5,5,5));

    TGHorizontalFrame* cmdFrame = new TGHorizontalFrame(outputGroup);
    cmdFrame->AddFrame(new TGLabel(cmdFrame, "root [0]"), new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5,5,2,2));
    fCommandEntry = new TGTextEntry(cmdFrame);
    fCommandEntry->Connect("ReturnPressed()", "AdvancedPlotGUI", this, "ProcessMessage(Long_t,Long_t,Long_t)");
    fCommandEntry->Resize(300, 20);
    cmdFrame->AddFrame(fCommandEntry, new TGLayoutHints(kLHintsExpandX, 5,5,2,2));

    TGTextButton* runCmdButton = new TGTextButton(cmdFrame, "Execute", kRunCommandButton);
    runCmdButton->Associate(this);
    cmdFrame->AddFrame(runCmdButton, new TGLayoutHints(kLHintsLeft, 5,5,2,2));

    TGTextButton* clearOutputButton = new TGTextButton(cmdFrame, "Clear Output", kClearOutputButton);
    clearOutputButton->Associate(this);
    cmdFrame->AddFrame(clearOutputButton, new TGLayoutHints(kLHintsLeft, 5,5,2,2));

    outputGroup->AddFrame(cmdFrame, new TGLayoutHints(kLHintsExpandX, 5,5,5,2));

    fScriptOutput = new TGTextView(outputGroup, 400, 100);
    outputGroup->AddFrame(fScriptOutput, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5,5,2,5));
}

// ============================================================================
// Enable/disable plot controls
// ============================================================================
void AdvancedPlotGUI::EnablePlotControls(Bool_t enable)
{
    fAddPlotButton->SetEnabled(enable);
    fPlotButton->SetEnabled(enable);
}

// ============================================================================
// Add entry to plot list box
// ============================================================================
void AdvancedPlotGUI::AddPlotToListBox(const char* desc, Int_t id)
{
    fPlotListBox->AddEntry(desc, id);
    fPlotListBox->Layout();
}

// ============================================================================
// Rebuild plot list box
// ============================================================================
void AdvancedPlotGUI::RebuildPlotListBox(const std::vector<PlotConfig>& configs)
{
    fPlotListBox->RemoveAll();
    for (size_t i = 0; i < configs.size(); ++i) {
        TString plotDesc = Form("Plot %zu: %s", 
            i + 1, 
            configs[i].GetDescription().c_str());
        fPlotListBox->AddEntry(plotDesc.Data(), i);
    }
    fPlotListBox->Layout();
}

// ============================================================================
// Clear plot list box
// ============================================================================
void AdvancedPlotGUI::ClearPlotListBox()
{
    fPlotListBox->RemoveAll();
    fPlotListBox->Layout();
}

// ============================================================================
// Load file using FileHandler
// ============================================================================


void AdvancedPlotGUI::LoadFromDrop(const char* filepath)
{
    if (!filepath || strlen(filepath) == 0) return;

    if (fFileHandler) {
        fFileHandler->LoadFromDrop(filepath);
    }
}

// ============================================================================
// Process messages - now just a dispatcher!
// ============================================================================
Bool_t AdvancedPlotGUI::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
    switch (GET_MSG(msg)) {
        case kC_COMMAND:
            switch (GET_SUBMSG(msg)) {
                case kCM_BUTTON:
                    // File operations
                    if (parm1 == kBrowseButton) {
                        fFileHandler->Browse();
                        fFileHandler->Load();
                        const char* filepath = GetFilePath();
                        if (filepath && strlen(filepath) > 0) {
                            fFileHandler->LoadFromDrop(filepath);
                        }
                    }
                    // Plot operations
                    else if (parm1 == kAddPlotButton) {
                        fPlotManager->AddPlot(fFileHandler->GetCurrentData());
                    }
                    else if (parm1 == kRemovePlotButton) {
                        fPlotManager->RemovePlot(fPlotListBox->GetSelected());
                    }
                    else if (parm1 == kClearAllButton) {
                        fPlotManager->ClearAll();
                    }
                    else if (parm1 == kPlotButton) {
                        std::string canvasTitle = fCanvasTitleEntry->GetText();
                        Bool_t overlayMode = fSameCanvasCheck->IsOn();
                        Bool_t dividedMode = fDividedCanvasCheck->IsOn();
                        FitUtils::FitType fitType = static_cast<FitUtils::FitType>(
                            fFitFunctionCombo->GetSelected());
                        std::string customFunc = fCustomFuncEntry->GetText();
                        
                        fPlotManager->CreatePlots(canvasTitle, overlayMode, dividedMode,
                                                 GetNRows(), GetNCols(), fitType, customFunc,
                                                 fFileHandler->GetCurrentData());
                    }
                    // Script operations
                    else if (parm1 == kRunScriptButton) {
                        fScriptEngine->RunScript(fScriptLangCombo->GetSelected());
                    }
                    else if (parm1 == kRunCommandButton) {
                        fScriptEngine->RunCommand(fScriptLangCombo->GetSelected());
                    }
                    else if (parm1 == kLoadScriptButton) {
                        fScriptEngine->LoadScript();
                    }
                    else if (parm1 == kSaveScriptButton) {
                        fScriptEngine->SaveScript();
                    }
                    else if (parm1 == kClearEditorButton) {
                        fScriptEngine->ClearEditor();
                    }
                    else if (parm1 == kClearOutputButton) {
                        fScriptEngine->ClearOutput();
                    }
                    break;
            }
            break;
            
        case kC_TEXTENTRY:
            switch (GET_SUBMSG(msg)) {
                case kTE_ENTER:
                    if (fCommandEntry && parm1 == (Long_t)fCommandEntry->WidgetId()) {
                        fScriptEngine->RunCommand(fScriptLangCombo->GetSelected());
                    }
                    break;
            }
            break;
    }
    
    return TGMainFrame::ProcessMessage(msg, parm1, parm2);
}

// ============================================================================
// Main
// ============================================================================
int main(int argc, char** argv)
{
    // Batch mode
    if (argc >= 3 && std::string(argv[1]) == "--batch") {
        ColumnData data;
        if (!DataReader::ReadFile(argv[2], data)) {
            std::cerr << "Failed to read file\n";
            return 1;
        }

        PlotConfig cfg;
        cfg.type = PlotConfig::kTH1D;
        cfg.xColumn = 0;
        cfg.bins = 100;

        TCanvas c("batch", "Batch Plot", 800, 600);
        TH1* h = PlotCreator::CreateTH1D(data, cfg);
        if (h) {
            h->Draw();
            c.SaveAs("batch_output.png");
            c.SaveAs("batch_output.pdf");
        }
        return 0;
    }

    // GUI + ROOT Prompt Mode
    TRint app("AdvancedPlotApp", &argc, argv);
    new AdvancedPlotGUI(gClient->GetRoot(), 400, 200);
    app.Run();
    return 0;
}