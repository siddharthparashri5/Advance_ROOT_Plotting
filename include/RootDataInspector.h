#ifndef ROOTDATAINSPECTOR_H
#define ROOTDATAINSPECTOR_H

#include <TGFrame.h>
#include <TGButton.h>
#include <TGComboBox.h>
#include <TGTextEntry.h>
#include <TGLabel.h>
#include <TGNumberEntry.h>
#include <TFile.h>
#include <TTree.h>
#include <string>

// ============================================================================
// RootDataInspector
// A TGGroupFrame widget that embeds inside a transient window.
// Lets the user pick a TTree, branch, and selection formula from a ROOT file.
// ============================================================================
class RootDataInspector : public TGGroupFrame {
public:
    RootDataInspector(const TGWindow* p, const char* filepath);
    virtual ~RootDataInspector();

    bool        LoadFile(const std::string& filename);

    // Accessors
    std::string GetSelectedTree()   const;
    std::string GetSelectedBranch() const;
    std::string GetFormula()        const;
    double      GetMinValue()       const;
    double      GetMaxValue()       const;

    // Slots — must be public for ROOT CINT/signal-slot to connect them
    void OnTreeChanged(Int_t id);
    void OnApplyFormula();

protected:
    void PopulateFileContents();
    void PopulateBranches();

private:
    TFile*         fFile            {nullptr};
    TTree*         fTree            {nullptr};

    TGComboBox*    treeCombo;
    TGComboBox*    branchCombo;
    TGLabel*       entriesLabel;
    TGNumberEntry* minEntry;
    TGNumberEntry* maxEntry;
    TGTextEntry*   formulaEntry;
    TGTextButton*  applyFormulaButton;

    // ClassDef needed because TGGroupFrame inherits TObject.
    // Version 0 avoids -Winconsistent-missing-override with ROOT 6.26.
    ClassDef(RootDataInspector, 1)
};

#endif // ROOTDATAINSPECTOR_H