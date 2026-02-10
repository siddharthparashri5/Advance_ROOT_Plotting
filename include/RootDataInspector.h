#ifndef ROOTDATAINSPECTOR_H
#define ROOTDATAINSPECTOR_H

#include <TGFrame.h>        // TGGroupFrame, TGVerticalFrame
#include <TGButton.h>       // TGTextButton
#include <TGComboBox.h>
#include <TGTextEntry.h>
#include <TGLabel.h>
#include <TGNumberEntry.h>

#include <TFile.h>
#include <TTree.h>

#include <string>

class RootDataInspector : public TGGroupFrame {
public:
    RootDataInspector(const TGWindow* p, const char* filepath);
    virtual ~RootDataInspector();

    bool LoadFile(const std::string& filename);

    std::string GetSelectedTree() const;
    std::string GetSelectedBranch() const;
    std::string GetFormula() const;
    double GetMinValue() const;
    double GetMaxValue() const;

protected:
    void PopulateFileContents();
    void PopulateBranches();
    void OnTreeChanged(Int_t id);
    void OnApplyFormula();

private:
    TFile* fFile {nullptr};
    TTree* fTree {nullptr};

    TGComboBox* treeCombo;
    TGComboBox* branchCombo;
    TGLabel* entriesLabel;
    TGNumberEntry* minEntry;
    TGNumberEntry* maxEntry;
    TGTextEntry* formulaEntry;
    TGTextButton* applyFormulaButton;

    //ClassDef(RootDataInspector, 0);
};

#endif
