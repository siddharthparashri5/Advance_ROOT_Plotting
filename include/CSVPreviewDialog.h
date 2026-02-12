#ifndef CSV_PREVIEW_DIALOG_H
#define CSV_PREVIEW_DIALOG_H

#include <TGFrame.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGComboBox.h>
#include <TGNumberEntry.h>
#include <TGTextEdit.h>
#include <TGLabel.h>
#include <TGMsgBox.h>


#include <string>
#include <vector>


class CSVPreviewDialog : public TGTransientFrame {
public:
    CSVPreviewDialog(const TGWindow* p, const char* filename);
    virtual ~CSVPreviewDialog();
    
    // Get configuration
    char GetDelimiter() const { return fSelectedDelimiter; }
    Int_t GetSkipRows() const { return fSkipRows; }
    Bool_t UseHeaderRow() const { return fUseHeaderRow; }
    const char* GetEncoding() const { return fEncoding.c_str(); }
    
    // Modal dialog
    Int_t DoModal();
    void UpdatePreview();
    virtual void CloseWindow() { UnmapWindow(); }

protected:
    virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

private:
    // File information
    TString fFilename;
    std::vector<std::string> fFileLines;
    
    // Configuration
    char fSelectedDelimiter;
    Int_t fSkipRows;
    Bool_t fUseHeaderRow;
    std::string fEncoding;
    Int_t fModalResult;  // ADDED: to track OK/Cancel
    
    // GUI Components
    TGLabel* fFilenameLabel;
    TGComboBox* fDelimiterCombo;
    TGTextEntry* fCustomDelimEntry;
    TGNumberEntry* fSkipRowsEntry;
    TGCheckButton* fHeaderCheckBox;
    TGComboBox* fEncodingCombo;
    
    // FIXED: Using TGTextEdit instead of TGTable/TGListView for ROOT 6.26 compatibility
    TGTextEdit* fPreviewText;
    TGListView* fListView;  // Keep declaration for compatibility but use TGTextEdit
    
    TGLabel* fRowCountLabel;
    TGLabel* fColCountLabel;
    
    TGTextButton* fOkButton;
    TGTextButton* fCancelButton;
    
    // Control IDs
    enum {
        kDelimiterCombo = 100,
        kCustomDelimEntry,
        kSkipRowsEntry,
        kHeaderCheckBox,
        kEncodingCombo,
        kOkButton,
        kCancelButton
    };
    
    // Helper methods
    void LoadFile();
    void ParseFile();
    
    std::vector<std::string> SplitLine(const std::string& line, char delimiter);
    Bool_t IsValidDelimiter(char c) const;

    ClassDef(CSVPreviewDialog, 1)
    
};

#endif