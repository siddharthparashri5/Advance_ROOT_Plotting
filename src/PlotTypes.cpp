#include "PlotTypes.h"

#include <TH1D.h>
#include <TH1F.h>
#include <TH1I.h>
#include <TH2D.h>
#include <TH2F.h>
#include <TH3D.h>
#include <TH3F.h>
#include <TGraph.h>
#include <TGraphErrors.h>

#include <iostream>
#include <string>

// ============================================================================
// Helper: unique histogram name counter so ROOT never complains about
// duplicate names when multiple plots are created in the same session.
// ============================================================================
static int gPlotCount = 0;
static std::string UniqueName(const char* prefix)
{
    return std::string(prefix) + "_" + std::to_string(++gPlotCount);
}

namespace PlotCreator {

// ============================================================================
// 1-D Histograms
// ============================================================================
TH1* CreateTH1(const ColumnData& data, const PlotConfig& cfg)
{
    return CreateTH1D(data, cfg);
}

TH1D* CreateTH1D(const ColumnData& data, const PlotConfig& cfg)
{
    if (cfg.xColumn < 0 || cfg.xColumn >= (int)data.data.size()) {
        std::cerr << "[PlotCreator] CreateTH1D: xColumn " << cfg.xColumn
                  << " out of range (" << data.data.size() << " columns)\n";
        return nullptr;
    }

    std::string name  = UniqueName("h1d");
    std::string title = cfg.title.empty()
        ? data.headers[cfg.xColumn]
        : cfg.title;

    double xmin = cfg.xMin;
    double xmax = cfg.xMax;
    if (xmin == xmax) {   // auto-range
        for (double v : data.data[cfg.xColumn]) {
            if (xmin == xmax) { xmin = xmax = v; }
            else { xmin = std::min(xmin, v); xmax = std::max(xmax, v); }
        }
        double margin = (xmax - xmin) * 0.05;
        xmin -= margin;  xmax += margin;
    }

    TH1D* h = new TH1D(name.c_str(),
                        (title + ";" + data.headers[cfg.xColumn] + ";Counts").c_str(),
                        cfg.bins, xmin, xmax);
    h->SetLineColor(cfg.color);
    h->SetLineWidth(2);

    for (double v : data.data[cfg.xColumn])
        h->Fill(v);

    return h;
}

TH1F* CreateTH1F(const ColumnData& data, const PlotConfig& cfg)
{
    if (cfg.xColumn < 0 || cfg.xColumn >= (int)data.data.size()) return nullptr;

    std::string name  = UniqueName("h1f");
    std::string title = cfg.title.empty() ? data.headers[cfg.xColumn] : cfg.title;

    double xmin = cfg.xMin, xmax = cfg.xMax;
    if (xmin == xmax) {
        for (double v : data.data[cfg.xColumn]) {
            if (xmin == xmax) { xmin = xmax = v; }
            else { xmin = std::min(xmin, v); xmax = std::max(xmax, v); }
        }
        double m = (xmax - xmin) * 0.05; xmin -= m; xmax += m;
    }

    TH1F* h = new TH1F(name.c_str(),
                        (title + ";" + data.headers[cfg.xColumn] + ";Counts").c_str(),
                        cfg.bins, (float)xmin, (float)xmax);
    h->SetLineColor(cfg.color);
    h->SetLineWidth(2);
    for (double v : data.data[cfg.xColumn]) h->Fill((float)v);
    return h;
}

TH1I* CreateTH1I(const ColumnData& data, const PlotConfig& cfg)
{
    if (cfg.xColumn < 0 || cfg.xColumn >= (int)data.data.size()) return nullptr;

    std::string name  = UniqueName("h1i");
    std::string title = cfg.title.empty() ? data.headers[cfg.xColumn] : cfg.title;

    double xmin = cfg.xMin, xmax = cfg.xMax;
    if (xmin == xmax) {
        for (double v : data.data[cfg.xColumn]) {
            if (xmin == xmax) { xmin = xmax = v; }
            else { xmin = std::min(xmin, v); xmax = std::max(xmax, v); }
        }
        double m = (xmax - xmin) * 0.05; xmin -= m; xmax += m;
    }

    TH1I* h = new TH1I(name.c_str(),
                        (title + ";" + data.headers[cfg.xColumn] + ";Counts").c_str(),
                        cfg.bins, (int)xmin, (int)xmax);
    h->SetLineColor(cfg.color);
    h->SetLineWidth(2);
    for (double v : data.data[cfg.xColumn]) h->Fill((int)v);
    return h;
}

// ============================================================================
// 2-D Histograms
// ============================================================================
TH2* CreateTH2(const ColumnData& data, const PlotConfig& cfg)
{
    return CreateTH2D(data, cfg);
}

TH2D* CreateTH2D(const ColumnData& data, const PlotConfig& cfg)
{
    int nc = (int)data.data.size();
    if (cfg.xColumn < 0 || cfg.xColumn >= nc ||
        cfg.yColumn < 0 || cfg.yColumn >= nc) {
        std::cerr << "[PlotCreator] CreateTH2D: column index out of range\n";
        return nullptr;
    }

    const auto& xv = data.data[cfg.xColumn];
    const auto& yv = data.data[cfg.yColumn];
    int n = (int)std::min(xv.size(), yv.size());

    auto autoRange = [](const std::vector<double>& v, double& lo, double& hi) {
        lo = hi = v[0];
        for (double x : v) { lo = std::min(lo, x); hi = std::max(hi, x); }
        double m = (hi - lo) * 0.05; lo -= m; hi += m;
    };

    double xmin = cfg.xMin, xmax = cfg.xMax;
    double ymin = cfg.yMin, ymax = cfg.yMax;
    if (xmin == xmax) autoRange(xv, xmin, xmax);
    if (ymin == ymax) autoRange(yv, ymin, ymax);

    std::string name  = UniqueName("h2d");
    std::string title = cfg.title.empty()
        ? (data.headers[cfg.yColumn] + " vs " + data.headers[cfg.xColumn])
        : cfg.title;

    TH2D* h = new TH2D(name.c_str(),
                        (title + ";" + data.headers[cfg.xColumn] + ";" +
                         data.headers[cfg.yColumn]).c_str(),
                        cfg.bins, xmin, xmax,
                        cfg.binsY, ymin, ymax);

    for (int i = 0; i < n; ++i) h->Fill(xv[i], yv[i]);
    return h;
}

TH2F* CreateTH2F(const ColumnData& data, const PlotConfig& cfg)
{
    // Delegate through TH2D logic for simplicity
    TH2D* tmp = CreateTH2D(data, cfg);
    if (!tmp) return nullptr;
    std::string name = UniqueName("h2f");
    TH2F* h = new TH2F(name.c_str(), tmp->GetTitle(),
                        tmp->GetNbinsX(), tmp->GetXaxis()->GetXmin(), tmp->GetXaxis()->GetXmax(),
                        tmp->GetNbinsY(), tmp->GetYaxis()->GetXmin(), tmp->GetYaxis()->GetXmax());
    // Copy bin contents
    for (int ix = 1; ix <= tmp->GetNbinsX(); ++ix)
        for (int iy = 1; iy <= tmp->GetNbinsY(); ++iy)
            h->SetBinContent(ix, iy, (float)tmp->GetBinContent(ix, iy));
    delete tmp;
    return h;
}

// ============================================================================
// 3-D Histograms
// ============================================================================
TH3* CreateTH3(const ColumnData& data, const PlotConfig& cfg)
{
    return CreateTH3D(data, cfg);
}

TH3D* CreateTH3D(const ColumnData& data, const PlotConfig& cfg)
{
    int nc = (int)data.data.size();
    if (cfg.xColumn < 0 || cfg.xColumn >= nc ||
        cfg.yColumn < 0 || cfg.yColumn >= nc ||
        cfg.zColumn < 0 || cfg.zColumn >= nc) {
        std::cerr << "[PlotCreator] CreateTH3D: column index out of range\n";
        return nullptr;
    }

    const auto& xv = data.data[cfg.xColumn];
    const auto& yv = data.data[cfg.yColumn];
    const auto& zv = data.data[cfg.zColumn];
    int n = (int)std::min({xv.size(), yv.size(), zv.size()});

    auto autoRange = [](const std::vector<double>& v, double& lo, double& hi) {
        lo = hi = v[0];
        for (double x : v) { lo = std::min(lo, x); hi = std::max(hi, x); }
        double m = (hi - lo) * 0.05; lo -= m; hi += m;
    };

    double xmin = cfg.xMin, xmax = cfg.xMax;
    double ymin = cfg.yMin, ymax = cfg.yMax;
    double zmin = cfg.zMin, zmax = cfg.zMax;
    if (xmin == xmax) autoRange(xv, xmin, xmax);
    if (ymin == ymax) autoRange(yv, ymin, ymax);
    if (zmin == zmax) autoRange(zv, zmin, zmax);

    std::string name  = UniqueName("h3d");
    std::string title = cfg.title.empty()
        ? (data.headers[cfg.xColumn] + " vs " + data.headers[cfg.yColumn] +
           " vs " + data.headers[cfg.zColumn])
        : cfg.title;

    TH3D* h = new TH3D(name.c_str(),
                        (title + ";" + data.headers[cfg.xColumn] + ";" +
                         data.headers[cfg.yColumn] + ";" +
                         data.headers[cfg.zColumn]).c_str(),
                        cfg.bins,  xmin, xmax,
                        cfg.binsY, ymin, ymax,
                        cfg.binsZ, zmin, zmax);

    for (int i = 0; i < n; ++i) h->Fill(xv[i], yv[i], zv[i]);
    return h;
}

TH3F* CreateTH3F(const ColumnData& data, const PlotConfig& cfg)
{
    TH3D* tmp = CreateTH3D(data, cfg);
    if (!tmp) return nullptr;
    std::string name = UniqueName("h3f");
    TH3F* h = new TH3F(name.c_str(), tmp->GetTitle(),
                        tmp->GetNbinsX(), tmp->GetXaxis()->GetXmin(), tmp->GetXaxis()->GetXmax(),
                        tmp->GetNbinsY(), tmp->GetYaxis()->GetXmin(), tmp->GetYaxis()->GetXmax(),
                        tmp->GetNbinsZ(), tmp->GetZaxis()->GetXmin(), tmp->GetZaxis()->GetXmax());
    for (int ix = 1; ix <= tmp->GetNbinsX(); ++ix)
        for (int iy = 1; iy <= tmp->GetNbinsY(); ++iy)
            for (int iz = 1; iz <= tmp->GetNbinsZ(); ++iz)
                h->SetBinContent(ix, iy, iz, (float)tmp->GetBinContent(ix, iy, iz));
    delete tmp;
    return h;
}

// ============================================================================
// Graphs
// ============================================================================
TGraph* CreateTGraph(const ColumnData& data, const PlotConfig& cfg)
{
    int nc = (int)data.data.size();
    if (cfg.xColumn < 0 || cfg.xColumn >= nc ||
        cfg.yColumn < 0 || cfg.yColumn >= nc) {
        std::cerr << "[PlotCreator] CreateTGraph: column index out of range\n";
        return nullptr;
    }

    const auto& xv = data.data[cfg.xColumn];
    const auto& yv = data.data[cfg.yColumn];
    int n = (int)std::min(xv.size(), yv.size());

    TGraph* g = new TGraph(n, xv.data(), yv.data());

    std::string title = cfg.title.empty()
        ? (data.headers[cfg.yColumn] + " vs " + data.headers[cfg.xColumn])
        : cfg.title;
    g->SetTitle((title + ";" + data.headers[cfg.xColumn] + ";" +
                 data.headers[cfg.yColumn]).c_str());

    g->SetLineColor(cfg.color);
    g->SetMarkerColor(cfg.color);
    g->SetMarkerStyle(21);
    g->SetLineWidth(2);
    return g;
}

TGraphErrors* CreateTGraphErrors(const ColumnData& data, const PlotConfig& cfg)
{
    int nc = (int)data.data.size();
    if (cfg.xColumn < 0 || cfg.xColumn >= nc ||
        cfg.yColumn < 0 || cfg.yColumn >= nc) {
        std::cerr << "[PlotCreator] CreateTGraphErrors: column index out of range\n";
        return nullptr;
    }

    const auto& xv = data.data[cfg.xColumn];
    const auto& yv = data.data[cfg.yColumn];
    int n = (int)std::min(xv.size(), yv.size());

    // Error columns — fall back to zeros if not specified
    std::vector<double> exv(n, 0.0), eyv(n, 0.0);
    if (cfg.xErrColumn >= 0 && cfg.xErrColumn < nc)
        for (int i = 0; i < n; ++i) exv[i] = data.data[cfg.xErrColumn][i];
    if (cfg.yErrColumn >= 0 && cfg.yErrColumn < nc)
        for (int i = 0; i < n; ++i) eyv[i] = data.data[cfg.yErrColumn][i];

    TGraphErrors* g = new TGraphErrors(n,
                                       xv.data(), yv.data(),
                                       exv.data(), eyv.data());

    std::string title = cfg.title.empty()
        ? (data.headers[cfg.yColumn] + " vs " + data.headers[cfg.xColumn])
        : cfg.title;
    g->SetTitle((title + ";" + data.headers[cfg.xColumn] + ";" +
                 data.headers[cfg.yColumn]).c_str());

    g->SetLineColor(cfg.color);
    g->SetMarkerColor(cfg.color);
    g->SetMarkerStyle(21);
    g->SetLineWidth(2);
    return g;
}

} // namespace PlotCreator