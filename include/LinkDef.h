#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// ── Classes that inherit from TObject and have ClassDef ─────────────────────
#pragma link C++ class AdvancedPlotGUI+;
#pragma link C++ class ColumnSelectorDialog+;
#pragma link C++ class RootDataInspector+;
#pragma link C++ class CSVPreviewDialog+;
#pragma link C++ class DropTextEntry+;
//#pragma link C++ class RootDataInspector+;

// NOTE: FileHandler, PlotManager, ScriptEngine are NOT listed here because
// they do not inherit from TObject and have no ClassDef. rootcling cannot
// generate a dictionary for plain C++ classes — listing them causes the
// "Unused class rule" warning seen in the build output.

#endif