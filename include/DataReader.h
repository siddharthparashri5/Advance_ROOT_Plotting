#ifndef DATAREADER_H
#define DATAREADER_H

#include <TFile.h>
#include <TTree.h>
#include <TKey.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TList.h>
#include <TClass.h>
#include <TPad.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

//////////////////////////////
// Data structure to hold column data
//////////////////////////////
struct ColumnData {
    std::vector<std::string> headers;           // Numeric column names
    std::vector<std::vector<double>> data;      // Numeric column data

    // ── NEW: string columns (for labels / legend text) ──
    std::vector<std::string> stringHeaders;             // string column names
    std::vector<std::vector<std::string>> stringData;   // string column data

    std::string filename;
    std::string name;
    std::vector<double> xVals;
    std::vector<double> yVals;
    std::vector<std::string> labels;

    // Constructor
    ColumnData() = default;

    // Clear data
    void Clear() {
        xVals.clear();
        yVals.clear();
        labels.clear();
    }

    int GetNumColumns() const { return (int)headers.size(); }
    int GetNumRows() const {
        return data.empty() ? 0 : (int)data[0].size();
    }
    int GetNumStringColumns() const { return (int)stringHeaders.size(); }
};

//////////////////////////////
// DataReader class
//////////////////////////////
class DataReader {
public:
    enum FileType {
        kUnknown,
        kText,      // .txt, .dat
        kCSV,       // .csv
        kROOT       // .root
    };

    // Detect file type from extension
    static FileType GetFileType(const std::string& filename) {
        size_t dotPos = filename.find_last_of('.');
        if (dotPos == std::string::npos) return kText;

        std::string ext = filename.substr(dotPos + 1);
        for (char& c : ext) c = std::tolower(c);

        if (ext == "root") return kROOT;
        if (ext == "csv")  return kCSV;
        if (ext == "txt" || ext == "dat") return kText;
        return kText;
    }

    // ── helper: detect if a token is numeric ──────────────────────────────
    static bool IsNumeric(const std::string& token) {
        if (token.empty()) return false;
        try { std::stod(token); return true; }
        catch (...) { return false; }
    }

    // Read text/dat file (space or tab separated)
    static bool ReadTextFile(const std::string& filename, ColumnData& data) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Cannot open file: " << filename << std::endl;
            return false;
        }

        data.filename = filename;
        std::string line;
        bool firstLine = true;

        // We determine column types from the first data row
        std::vector<bool> colIsNumeric;  // true = numeric, false = string

        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;

            std::stringstream ss(line);
            std::vector<std::string> tokens;
            std::string token;
            while (ss >> token) tokens.push_back(token);
            if (tokens.empty()) continue;

            // First line — detect headers
            if (firstLine) {
                bool isHeader = !IsNumeric(tokens[0]);
                if (isHeader) {
                    // Store ALL header names; classify later
                    // We'll assign to numeric/string after seeing first data row
                    // For now store tentative header list
                    for (const auto& t : tokens) {
                        data.headers.push_back(t);   // placeholder; may move to stringHeaders
                    }
                    firstLine = false;
                    continue;
                }
                // No header row — auto-generate
                for (size_t i = 0; i < tokens.size(); ++i)
                    data.headers.push_back(Form("Col%zu", i));
            }

            // Classify columns on first data row
            if (colIsNumeric.empty()) {
                for (const auto& t : tokens)
                    colIsNumeric.push_back(IsNumeric(t));

                // Redistribute headers into numeric / string
                std::vector<std::string> savedHeaders = data.headers;
                data.headers.clear();
                data.stringHeaders.clear();

                for (size_t i = 0; i < colIsNumeric.size(); ++i) {
                    if (i < savedHeaders.size()) {
                        if (colIsNumeric[i]) data.headers.push_back(savedHeaders[i]);
                        else                 data.stringHeaders.push_back(savedHeaders[i]);
                    } else {
                        if (colIsNumeric[i]) data.headers.push_back(Form("Col%zu", i));
                        else                 data.stringHeaders.push_back(Form("SCol%zu", i));
                    }
                }
                data.data.resize(data.headers.size());
                data.stringData.resize(data.stringHeaders.size());
            }

            // Parse row
            int numIdx = 0, strIdx = 0;
            for (size_t i = 0; i < tokens.size() && i < colIsNumeric.size(); ++i) {
                if (colIsNumeric[i]) {
                    try { data.data[numIdx].push_back(std::stod(tokens[i])); }
                    catch (...) { data.data[numIdx].push_back(0.0); }
                    ++numIdx;
                } else {
                    data.stringData[strIdx].push_back(tokens[i]);
                    ++strIdx;
                }
            }
        }

        return !data.data.empty();
    }

    // Read CSV file (comma separated)
    static bool ReadCSVFile(const std::string& filename, ColumnData& data,
                            char delimiter = ',', int skipRows = 0,
                            bool useHeader = true) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Cannot open file: " << filename << std::endl;
            return false;
        }

        data.filename = filename;
        std::string line;
        bool firstLine = true;
        int lineNum = 0;
        std::vector<bool> colIsNumeric;

        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (lineNum++ < skipRows) continue;

            std::stringstream ss(line);
            std::vector<std::string> tokens;
            std::string token;
            while (std::getline(ss, token, delimiter)) {
                token.erase(0, token.find_first_not_of(" \t\r\n"));
                size_t last = token.find_last_not_of(" \t\r\n");
                token = (last != std::string::npos) ? token.substr(0, last+1) : "";
                tokens.push_back(token);
            }
            if (tokens.empty()) continue;

            // Header row
            if (firstLine && useHeader) {
                bool isHeader = !IsNumeric(tokens[0]);
                if (isHeader) {
                    for (const auto& t : tokens)
                        data.headers.push_back(t);
                    firstLine = false;
                    continue;
                }
                // Auto-generate headers
                for (size_t i = 0; i < tokens.size(); ++i)
                    data.headers.push_back(Form("Col%zu", i));
            }

            // Classify on first data row
            if (colIsNumeric.empty()) {
                for (const auto& t : tokens)
                    colIsNumeric.push_back(IsNumeric(t));

                std::vector<std::string> savedHeaders = data.headers;
                data.headers.clear();
                data.stringHeaders.clear();

                for (size_t i = 0; i < colIsNumeric.size(); ++i) {
                    std::string h = (i < savedHeaders.size()) ? savedHeaders[i]
                                                               : Form("Col%zu", i);
                    if (colIsNumeric[i]) data.headers.push_back(h);
                    else                 data.stringHeaders.push_back(h);
                }
                data.data.resize(data.headers.size());
                data.stringData.resize(data.stringHeaders.size());
            }

            int numIdx = 0, strIdx = 0;
            for (size_t i = 0; i < tokens.size() && i < colIsNumeric.size(); ++i) {
                if (colIsNumeric[i]) {
                    try { data.data[numIdx].push_back(std::stod(tokens[i])); }
                    catch (...) { data.data[numIdx].push_back(0.0); }
                    ++numIdx;
                } else {
                    data.stringData[strIdx].push_back(tokens[i]);
                    ++strIdx;
                }
            }
            firstLine = false;
        }

        return !data.data.empty();
    }

    // Helper function to extract data from TH1
    static bool ExtractFromTH1(TH1* hist, ColumnData& data) {
        if (!hist) return false;
        int nBins = hist->GetNbinsX();
        data.headers.push_back(Form("%s_BinCenter", hist->GetName()));
        data.headers.push_back(Form("%s_Content", hist->GetName()));
        data.data.resize(2);
        for (int i = 1; i <= nBins; ++i) {
            data.data[0].push_back(hist->GetBinCenter(i));
            data.data[1].push_back(hist->GetBinContent(i));
        }
        std::cout << "Extracted TH1: " << hist->GetName() << " (" << nBins << " bins)" << std::endl;
        return true;
    }

    // Helper function to extract data from TH2
    static bool ExtractFromTH2(TH2* hist, ColumnData& data) {
        if (!hist) return false;
        int nBinsX = hist->GetNbinsX();
        int nBinsY = hist->GetNbinsY();
        data.headers.push_back(Form("%s_X", hist->GetName()));
        data.headers.push_back(Form("%s_Y", hist->GetName()));
        data.headers.push_back(Form("%s_Content", hist->GetName()));
        data.data.resize(3);
        for (int i = 1; i <= nBinsX; ++i)
            for (int j = 1; j <= nBinsY; ++j) {
                data.data[0].push_back(hist->GetXaxis()->GetBinCenter(i));
                data.data[1].push_back(hist->GetYaxis()->GetBinCenter(j));
                data.data[2].push_back(hist->GetBinContent(i, j));
            }
        std::cout << "Extracted TH2: " << hist->GetName()
                  << " (" << nBinsX << "x" << nBinsY << " bins)" << std::endl;
        return true;
    }

    // Helper function to extract data from TH3
    static bool ExtractFromTH3(TH3* hist, ColumnData& data) {
        if (!hist) return false;
        int nBinsX = hist->GetNbinsX();
        int nBinsY = hist->GetNbinsY();
        int nBinsZ = hist->GetNbinsZ();
        data.headers.push_back(Form("%s_X", hist->GetName()));
        data.headers.push_back(Form("%s_Y", hist->GetName()));
        data.headers.push_back(Form("%s_Z", hist->GetName()));
        data.headers.push_back(Form("%s_Content", hist->GetName()));
        data.data.resize(4);
        for (int i = 1; i <= nBinsX; ++i)
            for (int j = 1; j <= nBinsY; ++j)
                for (int k = 1; k <= nBinsZ; ++k) {
                    data.data[0].push_back(hist->GetXaxis()->GetBinCenter(i));
                    data.data[1].push_back(hist->GetYaxis()->GetBinCenter(j));
                    data.data[2].push_back(hist->GetZaxis()->GetBinCenter(k));
                    data.data[3].push_back(hist->GetBinContent(i, j, k));
                }
        std::cout << "Extracted TH3: " << hist->GetName()
                  << " (" << nBinsX << "x" << nBinsY << "x" << nBinsZ << " bins)" << std::endl;
        return true;
    }

    // Recursive helper to search through lists and pads
    static TObject* FindHistogramInList(TList* list, int depth = 0) {
        if (!list || depth > 10) return nullptr;
        TIter next(list);
        TObject* obj;
        while ((obj = next())) {
            if (!obj) continue;
            std::cout << std::string(depth * 2, ' ') << "  -> " << obj->GetName()
                      << " (class: " << obj->ClassName() << ")" << std::endl;
            if (obj->InheritsFrom(TH3::Class())) return obj;
            else if (obj->InheritsFrom(TH2::Class())) return obj;
            else if (obj->InheritsFrom(TH1::Class())) return obj;
            else if (obj->InheritsFrom(TPad::Class())) {
                TPad* pad = (TPad*)obj;
                TList* pp = pad->GetListOfPrimitives();
                if (pp) {
                    TObject* found = FindHistogramInList(pp, depth + 1);
                    if (found) return found;
                }
            } else if (obj->InheritsFrom(TList::Class())) {
                TObject* found = FindHistogramInList((TList*)obj, depth + 1);
                if (found) return found;
            }
        }
        return nullptr;
    }

    // Helper function to extract histograms from a canvas
    static bool ExtractFromCanvas(TCanvas* canvas, ColumnData& data) {
        if (!canvas) return false;
        std::cout << "Analyzing canvas: " << canvas->GetName() << std::endl;
        TList* primitives = canvas->GetListOfPrimitives();
        if (!primitives) { std::cerr << "Canvas has no primitives\n"; return false; }
        TObject* foundObj = FindHistogramInList(primitives);
        if (!foundObj) { std::cerr << "No histograms found in canvas\n"; return false; }
        if (foundObj->InheritsFrom(TH3::Class())) return ExtractFromTH3((TH3*)foundObj, data);
        if (foundObj->InheritsFrom(TH2::Class())) return ExtractFromTH2((TH2*)foundObj, data);
        if (foundObj->InheritsFrom(TH1::Class())) return ExtractFromTH1((TH1*)foundObj, data);
        return false;
    }

    // Read ROOT file
    static bool ReadROOTFile(const std::string& filename, ColumnData& data) {
        TFile* file = TFile::Open(filename.c_str(), "READ");
        if (!file || file->IsZombie()) { std::cerr << "Cannot open ROOT file: " << filename << std::endl; return false; }
        data.filename = filename;
        bool success = false;
        TIter next(file->GetListOfKeys());
        TKey* key;
        std::cout << "=== Scanning ROOT file ===" << std::endl;
        while ((key = (TKey*)next())) {
            TObject* obj = key->ReadObj();
            if (!obj) continue;
            std::cout << "Found: " << obj->GetName() << " (" << obj->ClassName() << ")" << std::endl;
            if      (obj->InheritsFrom(TH3::Class()))    success = ExtractFromTH3((TH3*)obj, data);
            else if (obj->InheritsFrom(TH2::Class()))    success = ExtractFromTH2((TH2*)obj, data);
            else if (obj->InheritsFrom(TH1::Class()))    success = ExtractFromTH1((TH1*)obj, data);
            else if (obj->InheritsFrom(TCanvas::Class())) success = ExtractFromCanvas((TCanvas*)obj, data);
            else if (obj->InheritsFrom(TTree::Class()))  success = ReadROOTTree((TTree*)obj, data);
            if (success) break;
        }
        if (!success) std::cerr << "No compatible objects found in ROOT file\n";
        file->Close();
        return success;
    }

    // Helper to read TTree
    static bool ReadROOTTree(TTree* tree, ColumnData& data) {
        if (!tree) return false;
        TObjArray* branches = tree->GetListOfBranches();
        int nBranches = branches->GetEntries();
        if (nBranches == 0) return false;
        data.headers.resize(nBranches);
        data.data.resize(nBranches);
        std::vector<Double_t> branchValues(nBranches);
        for (int i = 0; i < nBranches; ++i) {
            TBranch* branch = (TBranch*)branches->At(i);
            data.headers[i] = branch->GetName();
            tree->SetBranchAddress(branch->GetName(), &branchValues[i]);
        }
        Long64_t nEntries = tree->GetEntries();
        for (Long64_t entry = 0; entry < nEntries; ++entry) {
            tree->GetEntry(entry);
            for (int i = 0; i < nBranches; ++i)
                data.data[i].push_back(branchValues[i]);
        }
        std::cout << "Extracted TTree: " << tree->GetName()
                  << " (" << nBranches << " branches, " << nEntries << " entries)" << std::endl;
        return true;
    }

    // Main read function
    static bool ReadFile(const std::string& filename, ColumnData& data) {
        FileType type = GetFileType(filename);
        switch (type) {
            case kCSV:  return ReadCSVFile(filename, data);
            case kROOT: return ReadROOTFile(filename, data);
            case kText:
            default:    return ReadTextFile(filename, data);
        }
    }
};

#endif // DATAREADER_H
