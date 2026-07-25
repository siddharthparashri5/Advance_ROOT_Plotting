#ifndef PLOTTYPES_H
#define PLOTTYPES_H

#include <string>
#include <vector>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include "DataReader.h"

// ============================================================================
// PlotConfig
// ============================================================================
struct PlotConfig {
    enum PlotType {
        kTGraph = 0,
        kTGraphErrors,
        kTH1D,
        kTH1F,
        kTH1I,
        kTH2D,
        kTH2F,
        kTH2I,
        kTH3D,
        kTH3F,
        kTH3I
    };

    PlotType    type         = kTH1D;
    int         xColumn      = 0;
    int         yColumn      = 1;
    int         zColumn      = 2;
    int         xErrColumn   = -1;
    int         yErrColumn   = -1;
    // ── NEW: index into ColumnData::stringData (-1 = none) ──
    int         labelColumn  = -1;

    // ── NEW: categorical X-axis support for TH1 (bar chart of a string column) ──
    // categoryColumn: index into ColumnData::stringData supplying the category
    //   names for the X axis (-1 = use the normal numeric xColumn binning instead).
    // categoryValueColumn: optional index into ColumnData::data (numeric) whose
    //   values get summed per category (-1 = just count occurrences per category).
    int         categoryColumn      = -1;
    int         categoryValueColumn = -1;

    int         bins         = 100;
    int         binsY        = 100;
    int         binsZ        = 100;
    double      xMin         = 0.0;
    double      xMax         = 0.0;
    double      yMin         = 0.0;
    double      yMax         = 0.0;
    double      zMin         = 0.0;
    double      zMax         = 0.0;
    int         color        = 1;
    std::string title        = "";
    std::string xTitle       = "";
    std::string yTitle       = "";
    std::string zTitle       = "";

    std::string GetDescription() const {
        const char* typeNames[] = {
            "TGraph", "TGraphErrors",
            "TH1D", "TH1F", "TH1I",
            "TH2D", "TH2F", "TH2I",
            "TH3D", "TH3F", "TH3I"
        };
        std::string desc = typeNames[type];
        if (categoryColumn >= 0) {
            desc += " [categorical X]";
        } else {
            desc += " col[" + std::to_string(xColumn) + "]";
        }
        if (type == kTGraph || type == kTGraphErrors ||
            type == kTH2D   || type == kTH2F || type == kTH2I ||
            type == kTH3D   || type == kTH3F || type == kTH3I) {
            desc += " vs col[" + std::to_string(yColumn) + "]";
        }
        if (labelColumn >= 0) desc += " (labeled)";
        return desc;
    }
};

// ============================================================================
// PlotCreator
// ============================================================================
namespace PlotCreator {

    TH1*          CreateTH1  (const ColumnData& data, const PlotConfig& cfg);
    TH1D*         CreateTH1D (const ColumnData& data, const PlotConfig& cfg);
    TH1F*         CreateTH1F (const ColumnData& data, const PlotConfig& cfg);
    TH1I*         CreateTH1I (const ColumnData& data, const PlotConfig& cfg);

    // Bar chart of a string column: one bin per unique category, labeled on
    // the X axis via TAxis::SetBinLabel. Counts occurrences per category, or
    // sums cfg.categoryValueColumn's numeric values per category if set.
    TH1D*         CreateTH1Categorical(const ColumnData& data, const PlotConfig& cfg);

    TH2*          CreateTH2  (const ColumnData& data, const PlotConfig& cfg);
    TH2D*         CreateTH2D (const ColumnData& data, const PlotConfig& cfg);
    TH2F*         CreateTH2F (const ColumnData& data, const PlotConfig& cfg);

    TH3*          CreateTH3  (const ColumnData& data, const PlotConfig& cfg);
    TH3D*         CreateTH3D (const ColumnData& data, const PlotConfig& cfg);
    TH3F*         CreateTH3F (const ColumnData& data, const PlotConfig& cfg);

    TGraph*       CreateTGraph       (const ColumnData& data, const PlotConfig& cfg);
    TGraphErrors* CreateTGraphErrors (const ColumnData& data, const PlotConfig& cfg);

} // namespace PlotCreator

#endif // PLOTTYPES_H
