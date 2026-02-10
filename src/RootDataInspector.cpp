#include "RootDataInspector.h"

#include <TGLayout.h>
#include <TGMsgBox.h>

#include <TKey.h>
#include <TBranch.h>
#include <TROOT.h>

RootDataInspector::RootDataInspector(const TGWindow* p, const char* filepath)
    : TGGroupFrame(p, "ROOT File Inspector")
{
    SetLayoutManager(new TGVerticalLayout(this));

    // ─── TREE SELECTION ──────────────────────────────
    AddFrame(new TGLabel(this, "Tree:"), new TGLayoutHints(kLHintsLeft,5,5,5,2));

    treeCombo = new TGComboBox(this);
    treeCombo->Associate(this);
    AddFrame(treeCombo, new TGLayoutHints(kLHintsExpandX,5,5,2,5));

    // ─── BRANCH SELECTION ────────────────────────────
    AddFrame(new TGLabel(this, "Branch:"), new TGLayoutHints(kLHintsLeft,5,5,5,2));

    branchCombo = new TGComboBox(this);
    AddFrame(branchCombo, new TGLayoutHints(kLHintsExpandX,5,5,2,5));

    // ─── ENTRIES INFO ────────────────────────────────
    entriesLabel = new TGLabel(this, "Entries: -");
    AddFrame(entriesLabel, new TGLayoutHints(kLHintsLeft,5,5,5,5));

    // ─── VALUE WINDOW ────────────────────────────────
    TGHorizontalFrame* rangeFrame = new TGHorizontalFrame(this);

    rangeFrame->AddFrame(new TGLabel(rangeFrame, "Min"),
        new TGLayoutHints(kLHintsLeft,5,2,2,2));
    minEntry = new TGNumberEntry(rangeFrame, 0);
    rangeFrame->AddFrame(minEntry, new TGLayoutHints(kLHintsLeft,5,5,2,2));

    rangeFrame->AddFrame(new TGLabel(rangeFrame, "Max"),
        new TGLayoutHints(kLHintsLeft,5,2,2,2));
    maxEntry = new TGNumberEntry(rangeFrame, 0);
    rangeFrame->AddFrame(maxEntry, new TGLayoutHints(kLHintsLeft,5,5,2,2));

    AddFrame(rangeFrame, new TGLayoutHints(kLHintsLeft,5,5,5,5));

    // ─── FORMULA BAR ─────────────────────────────────
    AddFrame(new TGLabel(this, "Selection Formula:"),
        new TGLayoutHints(kLHintsLeft,5,5,5,2));

    formulaEntry = new TGTextEntry(this);
    formulaEntry->SetToolTipText("Example: Energy > 200 && abs(theta) < 0.3");
    AddFrame(formulaEntry, new TGLayoutHints(kLHintsExpandX,5,5,2,5));

    applyFormulaButton = new TGTextButton(this, "Apply Selection");
    applyFormulaButton->Connect(
        "Clicked()", "RootDataInspector", this, "OnApplyFormula()");
    AddFrame(applyFormulaButton, new TGLayoutHints(kLHintsLeft,5,5,5,5));
}

RootDataInspector::~RootDataInspector()
{
    if (fFile) {
        fFile->Close();
        delete fFile;
    }
}

bool RootDataInspector::LoadFile(const std::string& filename)
{
    if (fFile) {
        fFile->Close();
        delete fFile;
    }

    fFile = TFile::Open(filename.c_str(), "READ");
    if (!fFile || fFile->IsZombie()) {
        fFile = nullptr;
        return false;
    }

    PopulateFileContents();
    return true;
}

void RootDataInspector::PopulateFileContents()
{
    treeCombo->RemoveAll();

    TIter next(fFile->GetListOfKeys());
    TKey* key;
    int id = 0;

    while ((key = (TKey*)next())) {
        if (std::string(key->GetClassName()) == "TTree") {
            treeCombo->AddEntry(key->GetName(), id++);
        }
    }

    if (id > 0) {
        treeCombo->Select(0);
        OnTreeChanged(0);
    }
}

void RootDataInspector::PopulateBranches()
{
    branchCombo->RemoveAll();
    if (!fTree) return;

    int id = 0;
    for (auto* br : *fTree->GetListOfBranches()) {
        branchCombo->AddEntry(br->GetName(), id++);
    }

    branchCombo->Select(0);
    entriesLabel->SetText(
        Form("Entries: %lld", fTree->GetEntries()));
}

void RootDataInspector::OnTreeChanged(Int_t)
{
    TString name = treeCombo->GetTextEntry()->GetText();
    fTree = (TTree*)fFile->Get(name);
    PopulateBranches();
}

void RootDataInspector::OnApplyFormula()
{
    TString formula = formulaEntry->GetText();
    if (formula.IsNull() || formula.IsWhitespace()) {
        new TGMsgBox(gClient->GetRoot(), this,
            "Empty Formula", "Please enter a selection expression.",
            kMBIconExclamation, kMBOk);
    }
}

// ─── ACCESSORS FOR GUI INTEGRATION ─────────────────────
std::string RootDataInspector::GetSelectedTree() const
{
    return treeCombo->GetTextEntry()->GetText();
}

std::string RootDataInspector::GetSelectedBranch() const
{
    return branchCombo->GetTextEntry()->GetText();
}

std::string RootDataInspector::GetFormula() const
{
    return formulaEntry->GetText();
}

double RootDataInspector::GetMinValue() const
{
    return minEntry->GetNumber();
}

double RootDataInspector::GetMaxValue() const
{
    return maxEntry->GetNumber();
}
