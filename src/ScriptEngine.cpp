#include "ScriptEngine.h"
#include "AdvancedPlotGUI.h"

#include <TGFileDialog.h>
#include <TGClient.h>
#include <TPython.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TCanvas.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TGMsgBox.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

// ============================================================================
// Constructor
// ============================================================================
ScriptEngine::ScriptEngine(AdvancedPlotGUI* mainGUI)
    : fMainGUI(mainGUI),
      fCoutStream(nullptr),
      fCerrStream(nullptr),
      fOldCout(nullptr),
      fOldCerr(nullptr)
{
}

// ============================================================================
// Destructor
// ============================================================================
ScriptEngine::~ScriptEngine()
{
    RestoreStreams();
    delete fCoutStream;
    delete fCerrStream;
}

// ============================================================================
// Helper: append a line to the output view and also print to terminal
// ============================================================================
static void AppendOutput(TGTextView* out, const char* line)
{
    // Always print to real terminal so user can see output there too
    printf("%s\n", line);
    fflush(stdout);
    if (out) {
        out->AddLine(line);
        out->ShowBottom();
    }
}

// ============================================================================
// RunScript
// Strategy:
//   C++ mode  – write the script to a tmp .C file exactly as typed,
//               then execute with ROOT's ".x file.C" command.
//               This handles both bare-statement scripts and
//               scripts with a named void function correctly.
//   Python    – execute the whole string via TPython::Exec.
// ============================================================================
void ScriptEngine::RunScript(Int_t language)
{
    TGTextEdit* editor = fMainGUI->GetScriptEditor();
    TGTextView* output = fMainGUI->GetScriptOutput();
    if (!editor) return;

    TString code = editor->GetText()->AsString();
    if (code.IsNull() || code.IsWhitespace()) {
        AppendOutput(output, ">>> (script editor is empty)");
        return;
    }

    AppendOutput(output, ">>> Running script...");
    gSystem->ProcessEvents();

    if (language == 0) {  // ROOT / C++
        // ------------------------------------------------------------------
        // Write exactly what the user typed into a temp .C file.
        // ROOT's ".x file.C" will:
        //   - If the file has a void funcname(){} matching the filename,
        //     it compiles and calls it.
        //   - Otherwise it interprets the file top-to-bottom.
        // Either way it is correct, natural ROOT behaviour.
        // ------------------------------------------------------------------
        TString tmpFile = Form("/tmp/apg_script_%d.C", (int)gSystem->GetPid());

        {
            std::ofstream f(tmpFile.Data());
            if (!f.is_open()) {
                AppendOutput(output, ">>> ERROR: Cannot write temp file");
                return;
            }
            f << code.Data();
        }

        // Capture stdout/stderr so we can mirror them in the output view
        RedirectStreams();

        Int_t err = 0;
        gROOT->ProcessLine(Form(".x %s", tmpFile.Data()), &err);

        gSystem->ProcessEvents();
        FlushStreams(output);
        RestoreStreams();

        if (err) {
            AppendOutput(output, ">>> Script finished with errors (see above).");
        } else {
            AppendOutput(output, ">>> Script finished OK.");
        }

        gSystem->Unlink(tmpFile.Data());

    } else if (language == 1) {  // Python
        RedirectStreams();

        Bool_t ok = TPython::Exec(code.Data());

        gSystem->ProcessEvents();
        FlushStreams(output);
        RestoreStreams();

        if (!ok) {
            AppendOutput(output, ">>> ERROR: Python execution failed (check syntax).");
        } else {
            AppendOutput(output, ">>> Python script finished OK.");
        }
    }

    // Refresh all canvases that may have been created/modified
    TIter nextCanvas(gROOT->GetListOfCanvases());
    TCanvas* c;
    while ((c = (TCanvas*)nextCanvas())) {
        c->Modified();
        c->Update();
    }
    gSystem->ProcessEvents();
}

// ============================================================================
// RunCommand  – execute a single line from the command entry
// ============================================================================
void ScriptEngine::RunCommand(Int_t language)
{
    TGTextEntry* cmdEntry = fMainGUI->GetCommandEntry();
    TGTextView*  output   = fMainGUI->GetScriptOutput();
    if (!cmdEntry) return;

    TString cmd = cmdEntry->GetText();
    cmd.Strip(TString::kBoth);
    if (cmd.IsNull()) return;

    AppendOutput(output, Form("root [>] %s", cmd.Data()));

    RedirectStreams();

    Int_t err = 0;
    if (language == 0) {
        gROOT->ProcessLine(cmd.Data(), &err);
        if (err) AppendOutput(output, "  *** error (see terminal) ***");
        gSystem->ProcessEvents();
    } else {
        TPython::Exec(cmd.Data());
    }

    FlushStreams(output);
    RestoreStreams();

    // Refresh canvases
    TIter nc(gROOT->GetListOfCanvases());
    TCanvas* c;
    while ((c = (TCanvas*)nc())) { c->Modified(); c->Update(); }
    gSystem->ProcessEvents();

    if (output) output->ShowBottom();
    cmdEntry->Clear();
}

// ============================================================================
// LoadScript – load a file into the editor
// ============================================================================
void ScriptEngine::LoadScript()
{
    const char* filetypes[] = {
        "ROOT macros",    "*.C",
        "C++ source",     "*.cpp",
        "C++ source",     "*.cxx",
        "Python scripts", "*.py",
        "All files",      "*",
        nullptr, nullptr
    };

    TGFileInfo fileInfo;
    fileInfo.fFileTypes = filetypes;
    fileInfo.fIniDir    = StrDup(".");

    new TGFileDialog(gClient->GetRoot(), fMainGUI, kFDOpen, &fileInfo);
    if (!fileInfo.fFilename) return;

    std::ifstream file(fileInfo.fFilename);
    if (!file.is_open()) {
        new TGMsgBox(gClient->GetRoot(), fMainGUI,
            "Error", Form("Cannot open: %s", fileInfo.fFilename),
            kMBIconStop, kMBOk);
        return;
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    file.close();

    TGTextEdit* editor = fMainGUI->GetScriptEditor();
    if (editor) {
        editor->Clear();
        editor->LoadBuffer(content.c_str());
    }

    TGTextView* output = fMainGUI->GetScriptOutput();
    TString fname(fileInfo.fFilename);
    if (output) {
        AppendOutput(output, Form(">>> Loaded: %s", gSystem->BaseName(fileInfo.fFilename)));
        if (fname.EndsWith(".py")) {
            AppendOutput(output, ">>> (switch language dropdown to 'Python' before running)");
        }
    }
}

// ============================================================================
// SaveScript
// ============================================================================
void ScriptEngine::SaveScript()
{
    const char* filetypes[] = {
        "ROOT macros",    "*.C",
        "Python scripts", "*.py",
        "C++ source",     "*.cpp",
        "All files",      "*",
        nullptr, nullptr
    };

    TGFileInfo fileInfo;
    fileInfo.fFileTypes = filetypes;
    fileInfo.fIniDir    = StrDup(".");

    new TGFileDialog(gClient->GetRoot(), fMainGUI, kFDSave, &fileInfo);
    if (!fileInfo.fFilename) return;

    TGTextEdit* editor = fMainGUI->GetScriptEditor();
    if (!editor) return;

    std::ofstream file(fileInfo.fFilename);
    if (!file.is_open()) {
        new TGMsgBox(gClient->GetRoot(), fMainGUI,
            "Error", Form("Cannot write: %s", fileInfo.fFilename),
            kMBIconStop, kMBOk);
        return;
    }

    TString content = editor->GetText()->AsString();
    file << content.Data();
    file.close();

    TGTextView* output = fMainGUI->GetScriptOutput();
    if (output) AppendOutput(output, Form(">>> Saved: %s", fileInfo.fFilename));
}

// ============================================================================
// ClearEditor
// ============================================================================
void ScriptEngine::ClearEditor()
{
    TGTextEdit* editor = fMainGUI->GetScriptEditor();
    if (editor) editor->Clear();
}

// ============================================================================
// ClearOutput
// ============================================================================
void ScriptEngine::ClearOutput()
{
    TGTextView* output = fMainGUI->GetScriptOutput();
    if (output) output->Clear();
}

// ============================================================================
// RedirectStreams  – capture cout/cerr into internal string buffers
// ============================================================================
void ScriptEngine::RedirectStreams()
{
    if (!fCoutStream) fCoutStream = new std::ostringstream();
    if (!fCerrStream) fCerrStream = new std::ostringstream();

    fCoutStream->str("");
    fCerrStream->str("");

    fOldCout = std::cout.rdbuf(fCoutStream->rdbuf());
    fOldCerr = std::cerr.rdbuf(fCerrStream->rdbuf());
}

// ============================================================================
// RestoreStreams
// ============================================================================
void ScriptEngine::RestoreStreams()
{
    if (fOldCout) { std::cout.rdbuf(fOldCout); fOldCout = nullptr; }
    if (fOldCerr) { std::cerr.rdbuf(fOldCerr); fOldCerr = nullptr; }
}

// ============================================================================
// FlushStreams – push captured output to the GUI text view
// Also mirrors to terminal via printf (streams are restored first).
// ============================================================================
void ScriptEngine::FlushStreams()
{
    // Legacy overload – forward to the version that takes the output widget
    FlushStreams(fMainGUI ? fMainGUI->GetScriptOutput() : nullptr);
}

void ScriptEngine::FlushStreams(TGTextView* output)
{
    std::cout.flush();
    std::cerr.flush();

    auto dumpLines = [&](std::ostringstream* oss, const char* prefix) {
        if (!oss) return;
        std::string all = oss->str();
        if (all.empty()) return;

        // Print raw to real terminal now that streams are still redirected
        // (we'll restore AFTER this call, so use fOldCout directly)
        if (fOldCout) {
            std::ostream realOut(fOldCout);
            realOut << all;
            realOut.flush();
        }

        // Also push to the GUI output view, line by line
        if (output) {
            std::istringstream iss(all);
            std::string line;
            while (std::getline(iss, line)) {
                if (prefix && prefix[0] && !line.empty())
                    output->AddLine(Form("%s%s", prefix, line.c_str()));
                else if (!line.empty())
                    output->AddLine(line.c_str());
            }
        }
        oss->str("");
    };

    dumpLines(fCoutStream, "");
    dumpLines(fCerrStream, "ERR: ");

    if (output) output->ShowBottom();
}