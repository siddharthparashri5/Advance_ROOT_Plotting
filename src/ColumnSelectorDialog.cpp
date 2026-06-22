#include "ColumnSelectorDialog.h"

ClassImp(ColumnSelectorDialog)

ColumnSelectorDialog::ColumnSelectorDialog(const TGWindow* parent,
                                           const ColumnData* columnData,
                                           PlotConfig* plotConfig,
                                           bool* result)
: TGTransientFrame(gClient->GetRoot(), parent, 620, 560),
  data(columnData),
  config(plotConfig),
  dialogResult(result)
{
    *dialogResult = false;
    SetCleanup(kDeepCleanup);

    TGVerticalFrame* mainFrame = new TGVerticalFrame(this);

    // ── Title ──────────────────────────────────────────────────────────────
    TGLabel* titleLabel = new TGLabel(mainFrame,
        Form("Configure Plot  —  %d numeric, %d string columns available",
             data->GetNumColumns(), data->GetNumStringColumns()));
    mainFrame->AddFrame(titleLabel, new TGLayoutHints(kLHintsCenterX,5,5,10,10));

    // ── Plot type ──────────────────────────────────────────────────────────
    TGGroupFrame* typeGroup = new TGGroupFrame(mainFrame, "Plot Type");

    TGGroupFrame* graphGroup = new TGGroupFrame(typeGroup, "Graphs");
    radioTGraph       = new TGCheckButton(graphGroup, "TGraph (x vs y)", 1);
    radioTGraphErrors = new TGCheckButton(graphGroup, "TGraphErrors (x vs y with errors)", 2);
    graphGroup->AddFrame(radioTGraph,       new TGLayoutHints(kLHintsLeft,5,5,2,2));
    graphGroup->AddFrame(radioTGraphErrors, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    typeGroup->AddFrame(graphGroup, new TGLayoutHints(kLHintsExpandX,5,5,2,2));

    TGGroupFrame* h1Group = new TGGroupFrame(typeGroup, "1D Histograms");
    radioTH1D = new TGCheckButton(h1Group, "TH1D (double precision)", 3);
    radioTH1F = new TGCheckButton(h1Group, "TH1F (float precision)",  4);
    radioTH1I = new TGCheckButton(h1Group, "TH1I (integer bins)",      5);
    h1Group->AddFrame(radioTH1D, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    h1Group->AddFrame(radioTH1F, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    h1Group->AddFrame(radioTH1I, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    typeGroup->AddFrame(h1Group, new TGLayoutHints(kLHintsExpandX,5,5,2,2));

    TGGroupFrame* h2Group = new TGGroupFrame(typeGroup, "2D Histograms");
    radioTH2D = new TGCheckButton(h2Group, "TH2D (double precision)", 6);
    radioTH2F = new TGCheckButton(h2Group, "TH2F (float precision)",  7);
    radioTH2I = new TGCheckButton(h2Group, "TH2I (integer bins)",      8);
    h2Group->AddFrame(radioTH2D, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    h2Group->AddFrame(radioTH2F, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    h2Group->AddFrame(radioTH2I, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    typeGroup->AddFrame(h2Group, new TGLayoutHints(kLHintsExpandX,5,5,2,2));

    TGGroupFrame* h3Group = new TGGroupFrame(typeGroup, "3D Histograms");
    radioTH3D = new TGCheckButton(h3Group, "TH3D (double precision)",  9);
    radioTH3F = new TGCheckButton(h3Group, "TH3F (float precision)",  10);
    radioTH3I = new TGCheckButton(h3Group, "TH3I (integer bins)",      11);
    h3Group->AddFrame(radioTH3D, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    h3Group->AddFrame(radioTH3F, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    h3Group->AddFrame(radioTH3I, new TGLayoutHints(kLHintsLeft,5,5,2,2));
    typeGroup->AddFrame(h3Group, new TGLayoutHints(kLHintsExpandX,5,5,2,2));

    radioTGraph->SetOn();

    auto connectCB = [&](TGCheckButton* cb) {
        cb->Connect("Clicked()", "ColumnSelectorDialog", this, "UpdateColumnVisibility()");
    };
    connectCB(radioTGraph); connectCB(radioTGraphErrors);
    connectCB(radioTH1D);   connectCB(radioTH1F);   connectCB(radioTH1I);
    connectCB(radioTH2D);   connectCB(radioTH2F);   connectCB(radioTH2I);
    connectCB(radioTH3D);   connectCB(radioTH3F);   connectCB(radioTH3I);

    mainFrame->AddFrame(typeGroup, new TGLayoutHints(kLHintsExpandX,5,5,5,5));

    // ── Column selection ───────────────────────────────────────────────────
    TGGroupFrame* colGroup = new TGGroupFrame(mainFrame, "Column Selection");
    TGVerticalFrame* colFrame = new TGVerticalFrame(colGroup);

    auto addRow = [&](TGVerticalFrame* parent, const char* label,
                      TGComboBox*& combo, bool addNone = false) {
        TGHorizontalFrame* row = new TGHorizontalFrame(parent);
        TGLabel* lbl = new TGLabel(row, label);
        lbl->SetWidth(120);
        row->AddFrame(lbl, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,5,5,2,2));
        combo = new TGComboBox(row);
        if (addNone) combo->AddEntry("None", -1);
        for (int i = 0; i < data->GetNumColumns(); ++i)
            combo->AddEntry(data->headers[i].c_str(), i);
        combo->Resize(220, 20);
        row->AddFrame(combo, new TGLayoutHints(kLHintsLeft,5,5,2,2));
        parent->AddFrame(row, new TGLayoutHints(kLHintsExpandX,5,5,2,2));
    };

    addRow(colFrame, "X Column:",  xColumnCombo);
    xColumnCombo->Select(0);

    addRow(colFrame, "Y Column:",  yColumnCombo);
    if (data->GetNumColumns() > 1) yColumnCombo->Select(1);

    addRow(colFrame, "Z Column:",  zColumnCombo);
    zColumnCombo->Select(data->GetNumColumns() > 2 ? 2 : (data->GetNumColumns()-1));

    addRow(colFrame, "X Error:", xErrCombo, true);
    xErrCombo->Select(-1);

    addRow(colFrame, "Y Error:", yErrCombo, true);
    yErrCombo->Select(-1);

    // ── Label column (string columns) ──────────────────────────────────────
    {
        TGHorizontalFrame* row = new TGHorizontalFrame(colFrame);
        labelColumnLabel = new TGLabel(row, "Labels (string col):");
        labelColumnLabel->SetWidth(120);
        row->AddFrame(labelColumnLabel, new TGLayoutHints(kLHintsLeft|kLHintsCenterY,5,5,2,2));
        labelColumnCombo = new TGComboBox(row);
        labelColumnCombo->AddEntry("None", -1);
        for (int i = 0; i < data->GetNumStringColumns(); ++i)
            labelColumnCombo->AddEntry(data->stringHeaders[i].c_str(), i);
        labelColumnCombo->Select(-1);
        labelColumnCombo->Resize(220, 20);
        row->AddFrame(labelColumnCombo, new TGLayoutHints(kLHintsLeft,5,5,2,2));
        colFrame->AddFrame(row, new TGLayoutHints(kLHintsExpandX,5,5,2,2));

        if (data->GetNumStringColumns() == 0) {
            labelColumnCombo->SetEnabled(kFALSE);
        }
    }

    colGroup->AddFrame(colFrame, new TGLayoutHints(kLHintsExpandX,5,5,5,5));
    mainFrame->AddFrame(colGroup, new TGLayoutHints(kLHintsExpandX,5,5,5,5));

    // ── Buttons ────────────────────────────────────────────────────────────
    TGHorizontalFrame* buttonFrame = new TGHorizontalFrame(mainFrame);
    okButton     = new TGTextButton(buttonFrame, "OK",     1);
    cancelButton = new TGTextButton(buttonFrame, "Cancel", 2);
    okButton->Associate(this);
    cancelButton->Associate(this);
    buttonFrame->AddFrame(okButton,     new TGLayoutHints(kLHintsCenterX,5,5,5,5));
    buttonFrame->AddFrame(cancelButton, new TGLayoutHints(kLHintsCenterX,5,5,5,5));
    mainFrame->AddFrame(buttonFrame, new TGLayoutHints(kLHintsCenterX,5,5,10,5));

    AddFrame(mainFrame, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY));

    SetWindowName("Column Selector");
    MapSubwindows();
    Resize(GetDefaultSize());
    CenterOnParent();
    Layout();
    UpdateColumnVisibility();
    MapWindow();
}

// ── PopulateComboBox ────────────────────────────────────────────────────────
void ColumnSelectorDialog::PopulateComboBox(TGComboBox* combo, int /*startIdx*/) {
    for (int i = 0; i < data->GetNumColumns(); ++i)
        combo->AddEntry(data->headers[i].c_str(), i);
}

// ── DoOK ────────────────────────────────────────────────────────────────────
void ColumnSelectorDialog::DoOK() {
    config->xColumn    = xColumnCombo->GetSelected();
    config->yColumn    = yColumnCombo->GetSelected();
    config->zColumn    = zColumnCombo->GetSelected();
    config->xErrColumn = xErrCombo->GetSelected();
    config->yErrColumn = yErrCombo->GetSelected();
    config->labelColumn = labelColumnCombo->GetSelected();  // -1 if none

    if      (radioTGraph->IsOn())       config->type = PlotConfig::kTGraph;
    else if (radioTGraphErrors->IsOn()) config->type = PlotConfig::kTGraphErrors;
    else if (radioTH1D->IsOn())         config->type = PlotConfig::kTH1D;
    else if (radioTH1F->IsOn())         config->type = PlotConfig::kTH1F;
    else if (radioTH1I->IsOn())         config->type = PlotConfig::kTH1I;
    else if (radioTH2D->IsOn())         config->type = PlotConfig::kTH2D;
    else if (radioTH2F->IsOn())         config->type = PlotConfig::kTH2F;
    else if (radioTH2I->IsOn())         config->type = PlotConfig::kTH2I;
    else if (radioTH3D->IsOn())         config->type = PlotConfig::kTH3D;
    else if (radioTH3F->IsOn())         config->type = PlotConfig::kTH3F;
    else if (radioTH3I->IsOn())         config->type = PlotConfig::kTH3I;

    if (config->xColumn >= 0 && config->xColumn < (int)data->headers.size())
        config->xTitle = data->headers[config->xColumn];
    if (config->yColumn >= 0 && config->yColumn < (int)data->headers.size())
        config->yTitle = data->headers[config->yColumn];
    if (config->zColumn >= 0 && config->zColumn < (int)data->headers.size())
        config->zTitle = data->headers[config->zColumn];

    *dialogResult = true;
    DeleteWindow();
}

// ── DoCancel ────────────────────────────────────────────────────────────────
void ColumnSelectorDialog::DoCancel() {
    *dialogResult = false;
    DeleteWindow();
}

// ── UpdateColumnVisibility ──────────────────────────────────────────────────
void ColumnSelectorDialog::UpdateColumnVisibility()
{
    bool graph  = radioTGraph->IsOn();
    bool graphE = radioTGraphErrors->IsOn();
    bool h1d    = radioTH1D->IsOn();
    bool h1f    = radioTH1F->IsOn();
    bool h1i    = radioTH1I->IsOn();
    bool h2d    = radioTH2D->IsOn();
    bool h2f    = radioTH2F->IsOn();
    bool h2i    = radioTH2I->IsOn();
    bool h3d    = radioTH3D->IsOn();
    bool h3f    = radioTH3F->IsOn();
    bool h3i    = radioTH3I->IsOn();

    bool h1 = (h1d || h1f || h1i);
    bool h2 = (h2d || h2f || h2i);
    bool h3 = (h3d || h3f || h3i);

    // Enforce single selection
    if (graph) {
        radioTGraphErrors->SetOn(false);
        radioTH1D->SetOn(false); radioTH1F->SetOn(false); radioTH1I->SetOn(false);
        radioTH2D->SetOn(false); radioTH2F->SetOn(false); radioTH2I->SetOn(false);
        radioTH3D->SetOn(false); radioTH3F->SetOn(false); radioTH3I->SetOn(false);
    } else if (graphE) {
        radioTGraph->SetOn(false);
        radioTH1D->SetOn(false); radioTH1F->SetOn(false); radioTH1I->SetOn(false);
        radioTH2D->SetOn(false); radioTH2F->SetOn(false); radioTH2I->SetOn(false);
        radioTH3D->SetOn(false); radioTH3F->SetOn(false); radioTH3I->SetOn(false);
    } else if (h1) {
        radioTGraph->SetOn(false); radioTGraphErrors->SetOn(false);
        radioTH2D->SetOn(false); radioTH2F->SetOn(false); radioTH2I->SetOn(false);
        radioTH3D->SetOn(false); radioTH3F->SetOn(false); radioTH3I->SetOn(false);
        if (h1d) { radioTH1F->SetOn(false); radioTH1I->SetOn(false); }
        else if (h1f) { radioTH1D->SetOn(false); radioTH1I->SetOn(false); }
        else if (h1i) { radioTH1D->SetOn(false); radioTH1F->SetOn(false); }
    } else if (h2) {
        radioTGraph->SetOn(false); radioTGraphErrors->SetOn(false);
        radioTH1D->SetOn(false); radioTH1F->SetOn(false); radioTH1I->SetOn(false);
        radioTH3D->SetOn(false); radioTH3F->SetOn(false); radioTH3I->SetOn(false);
        if (h2d) { radioTH2F->SetOn(false); radioTH2I->SetOn(false); }
        else if (h2f) { radioTH2D->SetOn(false); radioTH2I->SetOn(false); }
        else if (h2i) { radioTH2D->SetOn(false); radioTH2F->SetOn(false); }
    } else if (h3) {
        radioTGraph->SetOn(false); radioTGraphErrors->SetOn(false);
        radioTH1D->SetOn(false); radioTH1F->SetOn(false); radioTH1I->SetOn(false);
        radioTH2D->SetOn(false); radioTH2F->SetOn(false); radioTH2I->SetOn(false);
        if (h3d) { radioTH3F->SetOn(false); radioTH3I->SetOn(false); }
        else if (h3f) { radioTH3D->SetOn(false); radioTH3I->SetOn(false); }
        else if (h3i) { radioTH3D->SetOn(false); radioTH3F->SetOn(false); }
    }

    // Column enable/disable
    xColumnCombo->SetEnabled(graph || graphE || h1 || h2 || h3);
    yColumnCombo->SetEnabled(graph || graphE || h2 || h3);
    zColumnCombo->SetEnabled(h3);
    xErrCombo->SetEnabled(graphE);
    yErrCombo->SetEnabled(graphE);

    // Labels: only useful for TGraph/TGraphErrors/TH2/TH3 and only when string columns exist
    bool labelsApplicable = (graph || graphE || h2 || h3);
    bool hasStringCols = data->GetNumStringColumns() > 0;
    labelColumnCombo->SetEnabled(labelsApplicable && hasStringCols);
    if (labelColumnLabel)
        gClient->NeedRedraw(labelColumnLabel);

    gClient->NeedRedraw(this);
}

// ── ProcessMessage ──────────────────────────────────────────────────────────
Bool_t ColumnSelectorDialog::ProcessMessage(Long_t msg, Long_t parm1, Long_t /*parm2*/) {
    switch (GET_MSG(msg)) {
        case kC_COMMAND:
            switch (GET_SUBMSG(msg)) {
                case kCM_BUTTON:
                    if (parm1 == 1) DoOK();
                    else if (parm1 == 2) DoCancel();
                    break;
            }
            break;
    }
    return kTRUE;
}
