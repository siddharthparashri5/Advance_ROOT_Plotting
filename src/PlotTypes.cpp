#include "PlotTypes.h"
#include <stdexcept>

namespace PlotCreator {

// =========================
// 1D Histograms
// =========================
TH1* CreateTH1(const ColumnData& data, const PlotConfig& cfg)
{
    switch (cfg.type) {
        case PlotConfig::kTH1D: return CreateTH1D(data, cfg);
        case PlotConfig::kTH1F: return CreateTH1F(data, cfg);
        case PlotConfig::kTH1I: return CreateTH1I(data, cfg);
        default: return nullptr;
    }
}

TH1D* CreateTH1D(const ColumnData& data, const PlotConfig& cfg)
{
    auto* h = new TH1D("h1", cfg.title.c_str(),
                       cfg.bins, cfg.xMin, cfg.xMax);

    for (double v : data.GetColumn(cfg.xColumn))
        h->Fill(v);

    h->SetLineColor(cfg.color);
    h->GetXaxis()->SetTitle(cfg.xTitle.c_str());
    h->GetYaxis()->SetTitle(cfg.yTitle.c_str());
    return h;
}

TH1F* CreateTH1F(const ColumnData& data, const PlotConfig& cfg)
{
    auto* h = new TH1F("h1f", cfg.title.c_str(),
                       cfg.bins, cfg.xMin, cfg.xMax);

    for (double v : data.GetColumn(cfg.xColumn))
        h->Fill(v);

    h->SetLineColor(cfg.color);
    return h;
}

TH1I* CreateTH1I(const ColumnData& data, const PlotConfig& cfg)
{
    auto* h = new TH1I("h1i", cfg.title.c_str(),
                       cfg.bins, cfg.xMin, cfg.xMax);

    for (double v : data.GetColumn(cfg.xColumn))
        h->Fill(v);

    h->SetLineColor(cfg.color);
    return h;
}

// =========================
// 2D Histograms
// =========================
TH2* CreateTH2(const ColumnData& data, const PlotConfig& cfg)
{
    switch (cfg.type) {
        case PlotConfig::kTH2D: return CreateTH2D(data, cfg);
        case PlotConfig::kTH2F: return CreateTH2F(data, cfg);
        default: return nullptr;
    }
}

TH2D* CreateTH2D(const ColumnData& data, const PlotConfig& cfg)
{
    auto* h = new TH2D("h2", cfg.title.c_str(),
                       cfg.bins, cfg.xMin, cfg.xMax,
                       cfg.binsY, cfg.yMin, cfg.yMax);

    const auto& x = data.GetColumn(cfg.xColumn);
    const auto& y = data.GetColumn(cfg.yColumn);

    for (size_t i = 0; i < x.size() && i < y.size(); ++i)
        h->Fill(x[i], y[i]);

    return h;
}

// =========================
// Graphs
// =========================
TGraph* CreateTGraph(const ColumnData& data, const PlotConfig& cfg)
{
    const auto& x = data.GetColumn(cfg.xColumn);
    const auto& y = data.GetColumn(cfg.yColumn);

    auto* g = new TGraph(x.size());
    for (int i = 0; i < (int)x.size(); ++i)
        g->SetPoint(i, x[i], y[i]);

    g->SetLineColor(cfg.color);
    return g;
}

TGraphErrors* CreateTGraphErrors(const ColumnData& data, const PlotConfig& cfg)
{
    const auto& x  = data.GetColumn(cfg.xColumn);
    const auto& y  = data.GetColumn(cfg.yColumn);
    const auto& ex = data.GetColumn(cfg.xErrColumn);
    const auto& ey = data.GetColumn(cfg.yErrColumn);

    auto* g = new TGraphErrors(x.size());
    for (int i = 0; i < (int)x.size(); ++i)
        g->SetPoint(i, x[i], y[i]),
        g->SetPointError(i, ex[i], ey[i]);

    g->SetLineColor(cfg.color);
    return g;
}

} // namespace PlotCreator
