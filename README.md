# Advanced ROOT Plotting - Documentation

Complete documentation package for the Advanced ROOT Plotting GUI application.

## 📚 Documentation Overview

This package contains comprehensive documentation for developers and users of the Advanced ROOT Plotting application.

### For Users
- **[User Manual](docs/UserManual.md)** - Complete guide for using the application
  - Quick start guide
  - Feature descriptions  
  - Example workflows
  - Keyboard shortcuts
  - Troubleshooting

### For Developers
- **[Developer Guide](docs/DeveloperGuide.md)** - Guide for extending the codebase
  - Architecture overview
  - How to add new plot types
  - How to add new file formats
  - How to add new fit functions
  - Testing and contributing

### Code Documentation
- **[Doxygen-commented Header](AdvancedPlotGUI.h)** - Fully documented example header
  - Class documentation
  - Method documentation
  - Parameter descriptions
  - Usage examples

## 🚀 Quick Start

### Generating API Documentation

Use Doxygen to generate HTML documentation from code comments:

```bash
# Generate documentation
doxygen Doxyfile

# View documentation
cd docs/doxygen/html
open index.html  # macOS
xdg-open index.html  # Linux
```

### Using the Header Template

The `AdvancedPlotGUI.h` file serves as a template for documenting your classes:

1. **Copy the documentation style** for your own classes
2. **Follow the Doxygen comment format** shown in the examples
3. **Include all required tags**: `@brief`, `@param`, `@return`, `@details`

### Documentation Workflow

```
┌─────────────────┐
│  Write Code     │
│  with Doxygen   │
│  Comments       │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Run Doxygen    │
│  (HTML/PDF)     │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Review Docs    │
│  in Browser     │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Publish to     │
│  GitHub Pages   │
└─────────────────┘
```

## 📋 Doxygen Comment Examples

### Class Documentation

```cpp
/**
 * @class MyClass
 * @brief Short one-line description
 * 
 * @details
 * Detailed multi-line description of what the class does,
 * its purpose, and how to use it.
 * 
 * Example usage:
 * @code
 * MyClass obj(param1, param2);
 * obj.DoSomething();
 * @endcode
 */
class MyClass {
    // ...
};
```

### Method Documentation

```cpp
/**
 * @brief Load data from file
 * @param filename Path to the data file
 * @return true if successful, false otherwise
 * 
 * @details
 * Supports ROOT (.root), CSV (.csv), and text (.txt) formats.
 * File format is auto-detected from extension.
 * 
 * @note The file must be readable and in a supported format
 * @warning Large files may take significant time to load
 * 
 * @see DoPlot(), DoFit()
 */
bool DoLoad(const char* filename);
```

### Member Variable Documentation

```cpp
TRootEmbeddedCanvas *fCanvas;  ///< Main plotting canvas
Int_t fLineColor;              ///< Line color for plots (1-50)
TH1D *fHistogram;              ///< Current histogram object
```

### File Documentation

```cpp
/**
 * @file MyFile.h
 * @brief Short description of file contents
 * @author Your Name
 * @date 2026-02-04
 * @version 1.0
 * 
 * Detailed description of what this file contains,
 * its purpose in the project, and dependencies.
 */
```

## 🔧 Doxygen Tags Reference

### Common Tags

| Tag | Purpose | Example |
|-----|---------|---------|
| `@brief` | Short description | `@brief Creates a new plot` |
| `@details` | Detailed description | `@details This function...` |
| `@param` | Parameter description | `@param x The x coordinate` |
| `@return` | Return value | `@return True if successful` |
| `@note` | Additional notes | `@note Thread-safe` |
| `@warning` | Warnings | `@warning May throw exception` |
| `@see` | Cross-references | `@see DoLoad(), DoPlot()` |
| `@code` | Code examples | `@code MyClass obj; @endcode` |
| `@deprecated` | Deprecated items | `@deprecated Use NewFunc() instead` |
| `@todo` | TODO items | `@todo Add error checking` |
| `@bug` | Known bugs | `@bug Crashes with null input` |
| `@author` | Author name | `@author John Doe` |
| `@date` | Date | `@date 2026-02-04` |
| `@version` | Version number | `@version 1.0` |

### Special Documentation

```cpp
/**
 * @defgroup plotting Plotting Functions
 * @brief Functions related to creating and managing plots
 * @{
 */

/// Function 1
void PlotHistogram();

/// Function 2  
void PlotScatter();

/** @} */ // end of plotting group
```

## 📁 Documentation Structure

```
ROOT_docs/
├── README.md                   # This file
├── AdvancedPlotGUI.h          # Example documented header
├── Doxyfile                   # Doxygen configuration
│
├── docs/
│   ├── UserManual.md          # User documentation
│   ├── DeveloperGuide.md      # Developer documentation
│   │
│   └── doxygen/               # Generated by Doxygen
│       ├── html/              # HTML documentation
│       │   ├── index.html     # Main page
│       │   ├── classes.html   # Class list
│       │   └── ...
│       └── latex/             # LaTeX/PDF (if enabled)
│
└── examples/                  # Example code (optional)
    ├── basic_usage.C
    └── advanced_usage.C
```

## 🎯 Documentation Best Practices

### 1. Be Consistent
- Use the same style throughout your codebase
- Follow naming conventions
- Keep format uniform

### 2. Be Clear
- Write for your audience (users vs. developers)
- Use simple language
- Include examples

### 3. Be Complete
- Document all public APIs
- Include parameter descriptions
- Explain return values and exceptions

### 4. Be Current
- Update docs when code changes
- Mark deprecated functions
- Version your documentation

### 5. Be Helpful
- Add usage examples
- Include common pitfalls
- Provide troubleshooting tips

## 🔍 Advanced Doxygen Features

### Graphs and Diagrams

Doxygen can generate various diagrams automatically:

- **Class hierarchy diagrams** - Shows inheritance relationships
- **Collaboration diagrams** - Shows class dependencies
- **Call graphs** - Shows function call relationships
- **Include graphs** - Shows header dependencies

Enable in Doxyfile:
```
HAVE_DOT = YES
CLASS_GRAPH = YES
COLLABORATION_GRAPH = YES
CALL_GRAPH = YES
```

### Mathematical Formulas

Use LaTeX notation for formulas:

```cpp
/**
 * @brief Calculate Gaussian function
 * 
 * Formula: \f$ f(x) = A \cdot e^{-\frac{(x-\mu)^2}{2\sigma^2}} \f$
 * 
 * Where:
 * - \f$ A \f$ is the amplitude
 * - \f$ \mu \f$ is the mean
 * - \f$ \sigma \f$ is the standard deviation
 */
double Gaussian(double x, double A, double mu, double sigma);
```

### Custom Pages

Create custom documentation pages:

```cpp
/**
 * @page installation Installation Guide
 * 
 * @section requirements Requirements
 * - ROOT 6.x or later
 * - C++11 compiler
 * - CMake 3.10+
 * 
 * @section build Building
 * @code
 * mkdir build && cd build
 * cmake ..
 * make
 * @endcode
 */
```

## 📖 Learning Resources

### Doxygen
- [Official Manual](https://www.doxygen.nl/manual/)
- [Documenting Code](https://www.doxygen.nl/manual/docblocks.html)
- [Special Commands](https://www.doxygen.nl/manual/commands.html)

### ROOT Documentation
- [ROOT Manual](https://root.cern.ch/doc/master/)
- [ROOT Tutorials](https://root.cern.ch/doc/master/group__Tutorials.html)
- [ROOT Forum](https://root-forum.cern.ch/)

### Writing Good Documentation
- [Google Developer Documentation Style Guide](https://developers.google.com/style)
- [Write the Docs](https://www.writethedocs.org/)

## 🛠️ Customization

### Modify Doxygen Output

Edit `Doxyfile` to customize:

```bash
# Project information
PROJECT_NAME = "Your Project Name"
PROJECT_BRIEF = "Your project description"

# Output options
GENERATE_HTML = YES
GENERATE_LATEX = NO
GENERATE_RTF = NO

# Include/exclude patterns
INPUT = include/ src/
EXCLUDE_PATTERNS = */test/* */.git/*

# Appearance
HTML_COLORSTYLE_HUE = 220  # Blue theme
HTML_COLORSTYLE_SAT = 100
```

### Custom Stylesheet

Create custom CSS for HTML output:

```bash
# In Doxyfile
HTML_EXTRA_STYLESHEET = custom.css
```

Example `custom.css`:
```css
/* Custom header color */
div.header {
    background-color: #2c3e50;
    color: white;
}

/* Custom code blocks */
div.fragment {
    background-color: #f8f9fa;
    border-left: 4px solid #007bff;
}
```

## 📊 Documentation Metrics

Track documentation quality:

```bash
# Count documented vs undocumented items
grep -r "@brief" include/ | wc -l

# Check for missing documentation
doxygen -w 2>&1 | grep "not documented"

# Generate coverage report
doxygen -d Markdown Doxyfile | grep "Documented"
```

## 🤝 Contributing to Documentation

### For Users
- Report unclear sections
- Suggest improvements
- Share usage examples
- Fix typos and errors

### For Developers
- Keep code comments up to date
- Add examples for new features
- Update guides when architecture changes
- Review documentation in pull requests

### Contribution Workflow

1. **Identify Issue**: Find missing or unclear documentation
2. **Make Changes**: Edit markdown or add Doxygen comments
3. **Test**: Generate docs and verify appearance
4. **Submit PR**: Create pull request with changes
5. **Review**: Address feedback from maintainers

## 📝 Checklist for Complete Documentation

- [ ] All public classes have `@brief` and `@details`
- [ ] All public methods documented with parameters and return values
- [ ] Usage examples provided for main features
- [ ] User Manual covers all features
- [ ] Developer Guide explains architecture
- [ ] Troubleshooting section addresses common issues
- [ ] Code examples compile and run
- [ ] Diagrams are clear and helpful
- [ ] Links between documents work
- [ ] Doxygen generates without warnings

## 🆘 Getting Help

If you have questions about documentation:

1. **Check existing docs** - Search User Manual and Developer Guide
2. **Review examples** - Look at AdvancedPlotGUI.h for patterns
3. **Doxygen Manual** - Consult official documentation
4. **GitHub Issues** - Ask on the project issue tracker
5. **ROOT Forum** - For ROOT-specific questions

## 📜 License

Documentation is released under the same license as the main project.

---

**Documentation Version**: 1.0  
**Last Updated**: February 4, 2026  
**Maintained by**: Siddharth Parashri

For questions or suggestions, please open an issue on GitHub.
