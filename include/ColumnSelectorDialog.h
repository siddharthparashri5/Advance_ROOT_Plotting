#ifndef COLUMNSELECTORDIALOG_H
#define COLUMNSELECTORDIALOG_H

#include <TGFrame.h>
#include <TGButton.h>
#include <TGComboBox.h>
#include <TGNumberEntry.h>
#include <TGLabel.h>
#include <RQ_OBJECT.h>

#include "DataReader.h"
#include "PlotTypes.h"

class ColumnSelectorDialog : public TGTransientFrame {
    ClassDefOverride(ColumnSelectorDialog, 0)

public:
    ColumnSelectorDialog(const TGWindow* parent,
                         const ColumnData* columnData,
                         PlotConfig*       plotConfig,
                         bool*             result);
    virtual ~ColumnSelectorDialog() {}

    Int_t DoModal() {
        MapWindow();
        gClient->WaitFor(this);
        return (*dialogResult) ? 1 : 0;
    }

    void CloseWindow() override { UnmapWindow(); }
    Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2) override;

    void UpdateColumnVisibility();

private:
    const ColumnData* data;
    PlotConfig*       config;
    bool*             dialogResult;

    // Plot-type checkboxes
    TGCheckButton* radioTGraph;
    TGCheckButton* radioTGraphErrors;
    TGCheckButton* radioTH1D;
    TGCheckButton* radioTH1F;
    TGCheckButton* radioTH1I;
    TGCheckButton* radioTH2D;
    TGCheckButton* radioTH2F;
    TGCheckButton* radioTH2I;
    TGCheckButton* radioTH3D;
    TGCheckButton* radioTH3F;
    TGCheckButton* radioTH3I;

    // Numeric column combos
    TGComboBox* xColumnCombo;
    TGComboBox* yColumnCombo;
    TGComboBox* zColumnCombo;
    TGComboBox* xErrCombo;
    TGComboBox* yErrCombo;

    // ── NEW: label column from string columns ──
    TGComboBox* labelColumnCombo;
    TGLabel*    labelColumnLabel;

    // ── NEW: categorical X-axis (bar chart of a string column), TH1 only ──
    TGComboBox* categoryColumnCombo;      // "None" + string headers
    TGComboBox* categoryValueCombo;       // "None (count)" + numeric headers
    TGLabel*    categoryColumnLabel;
    TGLabel*    categoryValueLabel;

    // ── NEW: bin counts for 1D/2D/3D histograms ──
    TGNumberEntry* binsXEntry;
    TGNumberEntry* binsYEntry;
    TGNumberEntry* binsZEntry;
    TGLabel*       binsXLabel;
    TGLabel*       binsYLabel;
    TGLabel*       binsZLabel;

    // Buttons
    TGTextButton* okButton;
    TGTextButton* cancelButton;

    void PopulateComboBox(TGComboBox* combo, int startIdx);
    void DoOK();
    void DoCancel();
};

// ============================================================================
// ColumnSelector wrapper
// ============================================================================
class ColumnSelector {
private:
    PlotConfig fConfig;
    bool       fAccepted = false;
    const TGWindow* fParent;
    const ColumnData& fData;

public:
    ColumnSelector(const TGWindow* p, const ColumnData& data)
        : fParent(p), fData(data) {}

    Int_t DoModal() {
        ColumnSelectorDialog* diag = new ColumnSelectorDialog(fParent, &fData, &fConfig, &fAccepted);
        return diag->DoModal();
    }

    PlotConfig* GetPlotConfig() {
        if (!fAccepted) return nullptr;
        return new PlotConfig(fConfig);
    }
};

#endif // COLUMNSELECTORDIALOG_H
