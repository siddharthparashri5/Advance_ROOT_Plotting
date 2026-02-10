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
// PlotConfig — describes a single plot request
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
        kTH3D,   // comma was missing here in the original — now fixed
        kTH3F,
        kTH3I
    };

    PlotType    type      = kTH1D;
    int         xColumn   = 0;
    int         yColumn   = 1;
    int         zColumn   = 2;
    int         xErrColumn  = -1;   // x error column  (-1 = none)
    int         yErrColumn  = -1;   // y error column  (-1 = none)
    int         bins      = 100;
    int         binsY     = 100;
    int         binsZ     = 100;
    double      xMin      = 0.0;
    double      xMax      = 0.0;  // 0,0 = auto-range
    double      yMin      = 0.0;
    double      yMax      = 0.0;
    double      zMin      = 0.0;
    double      zMax      = 0.0;
    int         color     = 1;
    std::string title     = "";
    std::string xTitle    = "";
    std::string yTitle    = "";
    std::string zTitle    = "";

    // Human-readable label used in the plot list box
    std::string GetDescription() const {
        const char* typeNames[] = {
            "TGraph", "TGraphErrors",
            "TH1D", "TH1F", "TH1I",
            "TH2D", "TH2F", "TH2I",
            "TH3D", "TH3F", "TH3I"
        };
        std::string desc = typeNames[type];
        desc += " col[" + std::to_string(xColumn) + "]";
        if (type == kTGraph || type == kTGraphErrors ||
            type == kTH2D   || type == kTH2F || type == kTH2I ||
            type == kTH3D   || type == kTH3F || type == kTH3I) {
            desc += " vs col[" + std::to_string(yColumn) + "]";
        }
        return desc;
    }
};

// ============================================================================
// PlotCreator — factory functions that build ROOT objects from ColumnData.
// Declarations only; implementations live in PlotTypes.cpp (or wherever your
// repo currently defines them).
// ============================================================================
namespace PlotCreator {

    // 1-D histograms
    TH1*          CreateTH1  (const ColumnData& data, const PlotConfig& cfg);
    TH1D*         CreateTH1D (const ColumnData& data, const PlotConfig& cfg);
    TH1F*         CreateTH1F (const ColumnData& data, const PlotConfig& cfg);
    TH1I*         CreateTH1I (const ColumnData& data, const PlotConfig& cfg);

    // 2-D histograms
    TH2*          CreateTH2  (const ColumnData& data, const PlotConfig& cfg);
    TH2D*         CreateTH2D (const ColumnData& data, const PlotConfig& cfg);
    TH2F*         CreateTH2F (const ColumnData& data, const PlotConfig& cfg);

    // 3-D histograms
    TH3*          CreateTH3  (const ColumnData& data, const PlotConfig& cfg);
    TH3D*         CreateTH3D (const ColumnData& data, const PlotConfig& cfg);
    TH3F*         CreateTH3F (const ColumnData& data, const PlotConfig& cfg);

    // Graphs
    TGraph*       CreateTGraph       (const ColumnData& data, const PlotConfig& cfg);
    TGraphErrors* CreateTGraphErrors (const ColumnData& data, const PlotConfig& cfg);

} // namespace PlotCreator

#endif // PLOTTYPES_H