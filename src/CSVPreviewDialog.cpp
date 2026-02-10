#include "CSVPreviewDialog.h"

#include <TGLayout.h>
#include <TSystem.h>
#include <TGSplitter.h>
#include "TG3DLine.h"
#include "TGListView.h"
#include "TString.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

// ============================================================================
// Constructor
// ============================================================================
CSVPreviewDialog::CSVPreviewDialog(const TGWindow* p, const char* filename)
    : TGTransientFrame(p, nullptr, 900, 600),
      fFilename(filename),
      fSelectedDelimiter(','),
      fSkipRows(0),
      fUseHeaderRow(kTRUE),
      fEncoding("UTF-8")
{
    SetWindowName("CSV/Text File Import Preview");
    SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputModeless);
    
    // Main layout
    TGVerticalFrame* mainFrame = new TGVerticalFrame(this);
    AddFrame(mainFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));

    fListView = new TGListView(this, 400, 300); // Use 'this' if inside the dialog
    TGLVContainer *container = new TGLVContainer(fListView);
    fListView->SetContainer(container);
    fListView->SetViewMode(kLVSmallIcons); // Standard list view mode
    
    // ─── FILE INFO ────────────────────────────────────
    TGHorizontalFrame* infoFrame = new TGHorizontalFrame(mainFrame);
    infoFrame->AddFrame(new TGLabel(infoFrame, "File:"), 
        new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 5, 5, 5, 5));
    
    fFilenameLabel = new TGLabel(infoFrame, filename);
    infoFrame->AddFrame(fFilenameLabel, 
        new TGLayoutHints(kLHintsExpandX | kLHintsCenterY, 5, 5, 5, 5));
    
    mainFrame->AddFrame(infoFrame, 
        new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    
    // ─── SEPARATOR ────────────────────────────────────
    mainFrame->AddFrame(new TGHorizontal3DLine(mainFrame),
        new TGLayoutHints(kLHintsExpandX, 0, 0, 5, 5));
    
    // ─── CONFIGURATION PANEL ──────────────────────────
    TGGroupFrame* configFrame = new TGGroupFrame(mainFrame, "Import Settings", kHorizontalFrame);
    
    // Delimiter selection
    TGVerticalFrame* delimFrame = new TGVerticalFrame(configFrame);
    delimFrame->AddFrame(new TGLabel(delimFrame, "Delimiter:"),
        new TGLayoutHints(kLHintsLeft, 5, 5, 5, 2));
    
    fDelimiterCombo = new TGComboBox(delimFrame);
    fDelimiterCombo->AddEntry("Comma (,)", 0);
    fDelimiterCombo->AddEntry("Semicolon (;)", 1);
    fDelimiterCombo->AddEntry("Tab (\\t)", 2);
    fDelimiterCombo->AddEntry("Space", 3);
    fDelimiterCombo->AddEntry("Custom", 4);
    fDelimiterCombo->Select(0);
    fDelimiterCombo->Associate(this);
    fDelimiterCombo->Connect("Selected(Int_t)", "CSVPreviewDialog", this, "UpdatePreview()");
    delimFrame->AddFrame(fDelimiterCombo, 
        new TGLayoutHints(kLHintsExpandX, 5, 5, 2, 5));
    
    fCustomDelimEntry = new TGTextEntry(delimFrame);
    fCustomDelimEntry->SetMaxLength(1);
    fCustomDelimEntry->SetText(",");
    //fCustomDelimEntry->Disable();
    fCustomDelimEntry->SetState(kFALSE); 
    delimFrame->AddFrame(fCustomDelimEntry,
        new TGLayoutHints(kLHintsExpandX, 5, 5, 2, 5));
    
    configFrame->AddFrame(delimFrame, 
        new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
    
    // Skip rows
    TGVerticalFrame* skipFrame = new TGVerticalFrame(configFrame);
    skipFrame->AddFrame(new TGLabel(skipFrame, "Skip First Rows:"),
        new TGLayoutHints(kLHintsLeft, 5, 5, 5, 2));
    
    fSkipRowsEntry = new TGNumberEntry(skipFrame, 0, 5, kSkipRowsEntry);
    fSkipRowsEntry->SetLimits(TGNumberEntry::kNELLimitMinMax, 0, 100);
    fSkipRowsEntry->Associate(this);
    skipFrame->AddFrame(fSkipRowsEntry,
        new TGLayoutHints(kLHintsExpandX, 5, 5, 2, 5));
    
    configFrame->AddFrame(skipFrame,
        new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
    
    // Header row
    TGVerticalFrame* headerFrame = new TGVerticalFrame(configFrame);
    fHeaderCheckBox = new TGCheckButton(headerFrame, "First row as header");
    fHeaderCheckBox->SetState(kButtonDown);
    fHeaderCheckBox->Associate(this);
    headerFrame->AddFrame(fHeaderCheckBox,
        new TGLayoutHints(kLHintsLeft, 5, 5, 5, 5));
    
    configFrame->AddFrame(headerFrame,
        new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
    
    // Encoding
    TGVerticalFrame* encodingFrame = new TGVerticalFrame(configFrame);
    encodingFrame->AddFrame(new TGLabel(encodingFrame, "Encoding:"),
        new TGLayoutHints(kLHintsLeft, 5, 5, 5, 2));
    
    fEncodingCombo = new TGComboBox(encodingFrame);
    fEncodingCombo->AddEntry("UTF-8", 0);
    fEncodingCombo->AddEntry("ISO-8859-1", 1);
    fEncodingCombo->AddEntry("ASCII", 2);
    fEncodingCombo->Select(0);
    fEncodingCombo->Associate(this);
    encodingFrame->AddFrame(fEncodingCombo,
        new TGLayoutHints(kLHintsExpandX, 5, 5, 2, 5));
    
    configFrame->AddFrame(encodingFrame,
        new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
    
    mainFrame->AddFrame(configFrame,
        new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    
    // ─── SEPARATOR ────────────────────────────────────
    mainFrame->AddFrame(new TGHorizontal3DLine(mainFrame),
        new TGLayoutHints(kLHintsExpandX, 0, 0, 5, 5));
    
    // ─── PREVIEW TABLE ────────────────────────────────
    TGGroupFrame* previewFrame = new TGGroupFrame(mainFrame, "Data Preview", kVerticalFrame);
    
    // Table for preview
    fPreviewTable = new TGTable(previewFrame, 100, 0, 100, 20);
    //fPreviewTable->SetSelectableRow(kTRUE);
    //fPreviewTable->SetSelectableColumn(kFALSE);
    //fPreviewTable->SetRowHeaderWidth(50);
    //fPreviewTable->GetDefaultWidth(80);
    //fPreviewTable->GetDefaultHeight(25);
    
    previewFrame->AddFrame(fPreviewTable,
        new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
    
    // Statistics
    TGHorizontalFrame* statsFrame = new TGHorizontalFrame(previewFrame);
    
    fRowCountLabel = new TGLabel(statsFrame, "Rows: 0");
    statsFrame->AddFrame(fRowCountLabel,
        new TGLayoutHints(kLHintsLeft, 5, 20, 5, 5));
    
    fColCountLabel = new TGLabel(statsFrame, "Columns: 0");
    statsFrame->AddFrame(fColCountLabel,
        new TGLayoutHints(kLHintsLeft, 5, 5, 5, 5));
    
    previewFrame->AddFrame(statsFrame,
        new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    
    mainFrame->AddFrame(previewFrame,
        new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
    
    // ─── BUTTONS ──────────────────────────────────────
    TGHorizontalFrame* buttonFrame = new TGHorizontalFrame(mainFrame);
    
    fOkButton = new TGTextButton(buttonFrame, "Load", kOkButton);
    fOkButton->Associate(this);
    buttonFrame->AddFrame(fOkButton,
        new TGLayoutHints(kLHintsRight, 5, 5, 5, 5));
    
    fCancelButton = new TGTextButton(buttonFrame, "Cancel", kCancelButton);
    fCancelButton->Associate(this);
    buttonFrame->AddFrame(fCancelButton,
        new TGLayoutHints(kLHintsRight, 5, 5, 5, 5));
    
    mainFrame->AddFrame(buttonFrame,
        new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
    
    MapSubwindows();
    Resize(GetDefaultSize());
    CenterOnParent();
    
    // Load and parse file
    LoadFile();
    ParseFile();
}

// ============================================================================
// Destructor
// ============================================================================
CSVPreviewDialog::~CSVPreviewDialog()
{
}

// ============================================================================
// Load file contents into memory
// ============================================================================
void CSVPreviewDialog::LoadFile()
{
    fFileLines.clear();
    
    std::ifstream file(fFilename.Data());
    if (!file.is_open()) {
        fprintf(stderr, "CSVPreviewDialog: Cannot open file: %s\n", fFilename.Data());
        return;
    }
    
    std::string line;
    const Int_t maxLines = 1000; // Limit to prevent memory issues
    
    while (std::getline(file, line) && fFileLines.size() < maxLines) {
        fFileLines.push_back(line);
    }
    
    file.close();
}

// ============================================================================
// Split line by delimiter
// ============================================================================
std::vector<std::string> CSVPreviewDialog::SplitLine(const std::string& line, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        // Trim leading/trailing spaces
        token.erase(0, token.find_first_not_of(" \t\r\n"));
        token.erase(token.find_last_not_of(" \t\r\n") + 1);
        tokens.push_back(token);
    }
    
    return tokens;
}

// ============================================================================
// Parse file with current settings
// ============================================================================
void CSVPreviewDialog::ParseFile()
{
    // Update preview
    UpdatePreview();
}

// ============================================================================
// Update preview table based on current settings
// ============================================================================
void CSVPreviewDialog::UpdatePreview()
{
    // Determine delimiter
    char delimiter = ',';
    Int_t delimChoice = fDelimiterCombo->GetSelected();
    
    switch(delimChoice) {
        case 0: delimiter = ','; break;
        case 1: delimiter = ';'; break;
        case 2: delimiter = '\t'; break;
        case 3: delimiter = ' '; break;
        case 4: 
            const char* customStr = fCustomDelimEntry->GetText();
            if (customStr && strlen(customStr) > 0) {
                delimiter = customStr[0];
            }
            break;
    }
    
    fSelectedDelimiter = delimiter;
    fSkipRows = (Int_t)fSkipRowsEntry->GetNumber();
    fUseHeaderRow = fHeaderCheckBox->IsDown();
    
    // Clear table
    fPreviewTable->Clear();
    
    Int_t rowIdx = 0;
    Int_t maxCols = 0;
    
    for (size_t i = 0; i < fFileLines.size(); ++i) {
        if ((Int_t)i < fSkipRows) continue;
        
        const std::string& line = fFileLines[i];
        std::vector<std::string> columns = SplitLine(line, delimiter);
        
        if (columns.size() > maxCols) {
            maxCols = columns.size();
        }
        TGLVContainer* container = (TGLVContainer*)fListView->GetContainer();
        TGLVEntry *item = new TGLVEntry(container, columns[0].c_str(), columns[0].c_str());
        for (size_t j = 1; j < columns.size(); j++) {
        // Note: This requires including "TGString.h"
            item->SetSubnames(columns[1].c_str(), 
                      columns.size() > 2 ? columns[2].c_str() : "",
                      columns.size() > 3 ? columns[3].c_str() : ""); 
            break; // SetSubnames handles multiple at once; loop isn't ideal here
        }
        container->AddItem(item);
        
        rowIdx++;
        if (rowIdx >= 100) break; // Limit preview rows
    }
    
    // Update labels
    fRowCountLabel->SetText(Form("Rows: %d", rowIdx));
    fColCountLabel->SetText(Form("Columns: %d", maxCols));
    
    fPreviewTable->Layout();
}

// ============================================================================
// Process messages
// ============================================================================
Bool_t CSVPreviewDialog::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
    switch(GET_MSG(msg)) {
        case kC_COMMAND:
            switch(GET_SUBMSG(msg)) {
                case kCM_BUTTON:
                    if (parm1 == kOkButton) {
                        // Update settings and close
                        UpdatePreview();
                        CloseWindow();
                        return kTRUE;
                    } else if (parm1 == kCancelButton) {
                        CloseWindow();
                        return kTRUE;
                    }
                    break;
                    
                case kCM_COMBOBOX:
                    if (parm1 == kDelimiterCombo) {
                        if (parm2 == 4) { // Custom selected
                            fCustomDelimEntry->SetState(kTRUE);
                        } else {
                            fCustomDelimEntry->SetState(kFALSE);
                        }
                        UpdatePreview();
                        return kTRUE;
                    }
                    break;
                    
                case kCM_CHECKBUTTON:
                    UpdatePreview();
                    return kTRUE;
            }
            break;
            
        case kC_TEXTENTRY:
            if (parm1 == kSkipRowsEntry || parm1 == kCustomDelimEntry) {
                UpdatePreview();
                return kTRUE;
            }
            break;
    }
    
    return TGTransientFrame::ProcessMessage(msg, parm1, parm2);
}

// ============================================================================
// Modal dialog
// ============================================================================
Int_t CSVPreviewDialog::DoModal()
{
    MapWindow();
    gClient->WaitFor(this);
    return 1; // Always return OK
}

// ============================================================================
// Validation
// ============================================================================
Bool_t CSVPreviewDialog::IsValidDelimiter(char c) const
{
    // Accept printable ASCII delimiters
    return std::isprint(c) && c != '"' && c != '\n' && c != '\r';
}
