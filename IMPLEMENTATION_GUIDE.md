# Implementation Guide - Adding Documentation to Your Project

This guide walks you through integrating the documentation into your Advanced ROOT Plotting project.

## Prerequisites

- Git installed on your system
- Doxygen installed (`sudo apt-get install doxygen` on Ubuntu or `brew install doxygen` on macOS)
- GraphViz installed for diagrams (`sudo apt-get install graphviz` or `brew install graphviz`)
- Your project cloned locally

## Step 1: Integrate Doxygen Comments into Your Code

### 1.1 Locate Your Source Files

First, navigate to your project directory:

```bash
cd /path/to/Advance_ROOT_Plotting
```

Your project structure likely looks like:
```
Advance_ROOT_Plotting/
├── src/
│   └── AdvancedPlotGUI.cxx
├── include/
│   └── AdvancedPlotGUI.h
├── CMakeLists.txt
└── README.md
```

### 1.2 Add Doxygen Comments to Header Files

Open your actual `AdvancedPlotGUI.h` file and add documentation. Here's how:

**BEFORE (your current code):**
```cpp
class AdvancedPlotGUI : public TGMainFrame {
private:
    TRootEmbeddedCanvas *fCanvas;
    
public:
    AdvancedPlotGUI(const TGWindow *p, UInt_t w, UInt_t h);
    bool DoLoad();
};
```

**AFTER (with Doxygen comments):**
```cpp
/**
 * @class AdvancedPlotGUI
 * @brief Advanced plotting GUI for ROOT data visualization
 * 
 * @details
 * This class provides a comprehensive graphical user interface for loading,
 * visualizing, and analyzing data using ROOT.
 */
class AdvancedPlotGUI : public TGMainFrame {
private:
    TRootEmbeddedCanvas *fCanvas;  ///< Embedded ROOT canvas for plotting
    
public:
    /**
     * @brief Constructor for AdvancedPlotGUI
     * @param p Pointer to parent window
     * @param w Width of main window in pixels
     * @param h Height of main window in pixels
     */
    AdvancedPlotGUI(const TGWindow *p, UInt_t w, UInt_t h);
    
    /**
     * @brief Load data from file
     * @return true if successful, false otherwise
     */
    bool DoLoad();
};
```

### 1.3 Step-by-Step Documentation Process

For each file in your project:

#### A. Document the File Header

Add this at the very top of each `.h` file:

```cpp
/**
 * @file AdvancedPlotGUI.h
 * @brief Advanced plotting GUI for ROOT data visualization
 * @author Siddharth Parashri
 * @date 2026-02-04
 * @version 1.0
 */

#ifndef ADVANCEDPLOTGUI_H
#define ADVANCEDPLOTGUI_H
// ... rest of file
```

#### B. Document Each Class

Before each class definition:

```cpp
/**
 * @class ClassName
 * @brief One-line description
 * 
 * @details
 * Detailed multi-line description of what the class does.
 * Include usage examples if helpful.
 */
class ClassName {
    // ...
};
```

#### C. Document Public Methods

For every public method:

```cpp
/**
 * @brief What this method does
 * @param paramName Description of parameter
 * @return What it returns
 */
ReturnType MethodName(Type paramName);
```

#### D. Document Member Variables

For member variables, use inline comments:

```cpp
private:
    Int_t fCount;        ///< Number of items
    Double_t fValue;     ///< Current value
    TString fName;       ///< Object name
```

### 1.4 Quick Reference for Common Patterns

**Constructor:**
```cpp
/**
 * @brief Constructor
 * @param param1 Description
 * @param param2 Description
 */
ClassName(Type1 param1, Type2 param2);
```

**Destructor:**
```cpp
/**
 * @brief Destructor
 */
virtual ~ClassName();
```

**Getter:**
```cpp
/**
 * @brief Get the value
 * @return Current value
 */
Double_t GetValue() const { return fValue; }
```

**Setter:**
```cpp
/**
 * @brief Set the value
 * @param value New value to set
 */
void SetValue(Double_t value) { fValue = value; }
```

## Step 2: Generate HTML Documentation

### 2.1 Set Up Documentation Directory

Create a docs folder in your project:

```bash
cd /path/to/Advance_ROOT_Plotting
mkdir -p docs
```

### 2.2 Copy Documentation Files

Copy the files I created into your project:

```bash
# Copy the Doxyfile
cp /path/to/downloaded/Doxyfile ./

# Copy the documentation guides
cp /path/to/downloaded/docs/UserManual.md ./docs/
cp /path/to/downloaded/docs/DeveloperGuide.md ./docs/
cp /path/to/downloaded/QuickReference.md ./docs/
cp /path/to/downloaded/README.md ./docs/README_DOCS.md
```

### 2.3 Customize the Doxyfile

Open `Doxyfile` in a text editor and update these key settings:

```bash
# Option 1: Use a text editor
nano Doxyfile

# Option 2: Use sed to automate changes
sed -i 's/PROJECT_NAME           = "Advanced ROOT Plotting"/PROJECT_NAME           = "Your Project Name"/' Doxyfile
```

**Key settings to customize:**

```plaintext
Line 35: PROJECT_NAME = "Advanced ROOT Plotting"
         → Change to your project name

Line 38: OUTPUT_DIRECTORY = docs/doxygen
         → This is where HTML files will go (keep as is)

Line 930: INPUT = include/ src/ parsers/ plots/ fits/
          → Update to match YOUR directory structure
          → For basic setup, use: INPUT = include/ src/

Line 1006: USE_MDFILE_AS_MAINPAGE = README.md
           → This uses your README as the main page
```

**Quick customization script:**

Create a file `customize_doxyfile.sh`:

```bash
#!/bin/bash
# Customize Doxyfile for your project

# Update project name
sed -i 's/PROJECT_NAME           = ".*"/PROJECT_NAME           = "Advanced ROOT Plotting"/' Doxyfile

# Update input directories (adjust paths as needed)
sed -i 's|INPUT                  = .*|INPUT                  = include/ src/ README.md|' Doxyfile

# Update author in other docs
find docs/ -name "*.md" -exec sed -i 's/Your Name/Siddharth Parashri/g' {} \;

echo "Doxyfile customized!"
```

Make it executable and run:
```bash
chmod +x customize_doxyfile.sh
./customize_doxyfile.sh
```

### 2.4 Generate the Documentation

Now generate the HTML documentation:

```bash
# Make sure you're in the project root directory
cd /path/to/Advance_ROOT_Plotting

# Run Doxygen
doxygen Doxyfile
```

You should see output like:
```
Searching for include files...
Searching for files in directory /path/to/include
Searching for files in directory /path/to/src
...
Generating HTML output...
```

### 2.5 View the Generated Documentation

Open the documentation in your browser:

```bash
# On macOS:
open docs/doxygen/html/index.html

# On Linux:
xdg-open docs/doxygen/html/index.html

# On Windows:
start docs/doxygen/html/index.html
```

### 2.6 Troubleshooting Generation Issues

**Problem: "Doxygen: command not found"**
```bash
# Ubuntu/Debian:
sudo apt-get update
sudo apt-get install doxygen graphviz

# macOS:
brew install doxygen graphviz

# Check installation:
doxygen --version
```

**Problem: "Warning: documented function X not found"**
- Make sure your function names in comments match actual function names
- Check for typos in function signatures

**Problem: No graphs appear**
- Install GraphViz: `sudo apt-get install graphviz`
- In Doxyfile, ensure: `HAVE_DOT = YES`

**Problem: Files not found**
- Check the `INPUT` paths in Doxyfile match your actual directories
- Use absolute paths if relative paths don't work

## Step 3: Customize Documentation

### 3.1 Update Project Information

Edit key files to personalize:

**1. Update docs/UserManual.md:**
```bash
nano docs/UserManual.md
```

Change:
- Author name (line 6)
- Installation instructions (if different)
- Add/remove features based on your actual implementation

**2. Update docs/DeveloperGuide.md:**
```bash
nano docs/DeveloperGuide.md
```

Change:
- Contact information (bottom of file)
- Architecture diagram (if your structure differs)
- Add sections for your specific components

**3. Update main README.md:**
```bash
nano README.md
```

Add a documentation section:
```markdown
## 📚 Documentation

- [User Manual](docs/UserManual.md) - Complete usage guide
- [Developer Guide](docs/DeveloperGuide.md) - Architecture and extension guide
- [API Documentation](docs/doxygen/html/index.html) - Auto-generated from code
- [Quick Reference](docs/QuickReference.md) - Doxygen syntax cheat sheet
```

### 3.2 Add Custom Examples

Create an `examples/` directory with sample usage:

```bash
mkdir -p examples
```

Create `examples/basic_usage.C`:
```cpp
/**
 * @file basic_usage.C
 * @brief Basic usage example for Advanced ROOT Plotting
 * 
 * This example demonstrates:
 * - Loading data from a CSV file
 * - Creating a histogram
 * - Fitting with a Gaussian
 * - Exporting the plot
 * 
 * Usage:
 * @code
 * root -l basic_usage.C
 * @endcode
 */

void basic_usage() {
    // Create the GUI
    TApplication theApp("App", nullptr, nullptr);
    AdvancedPlotGUI gui(gClient->GetRoot(), 1200, 800);
    
    // Load data
    gui.DoLoad("data/sample.csv");
    
    // Create histogram
    gui.SetPlotType(0);  // Histogram
    gui.DoPlot();
    
    // Fit with Gaussian
    gui.DoFit();
    
    // Export
    gui.DoExport();
    
    theApp.Run();
}
```

Update Doxyfile to include examples:
```plaintext
EXAMPLE_PATH = examples/
EXAMPLE_PATTERNS = *.C *.cxx
```

### 3.3 Create a Documentation Logo (Optional)

Create a simple logo and add it:

```bash
# In Doxyfile, update:
PROJECT_LOGO = docs/images/logo.png
```

Then create `docs/images/logo.png` (200x55 pixels recommended).

## Step 4: Add to Repository

### 4.1 Check Current Git Status

```bash
cd /path/to/Advance_ROOT_Plotting
git status
```

### 4.2 Create .gitignore for Documentation

Create/update `.gitignore` to exclude generated files:

```bash
nano .gitignore
```

Add these lines:
```gitignore
# Doxygen generated files
docs/doxygen/html/
docs/doxygen/latex/
docs/doxygen/xml/

# Build directories
build/
*.o
*.so

# ROOT files
*.root

# Backup files
*~
*.bak
```

### 4.3 Add Documentation Files

```bash
# Add the Doxyfile
git add Doxyfile

# Add documentation directory
git add docs/

# Add examples (if you created them)
git add examples/

# Add updated .gitignore
git add .gitignore

# Add updated README if you modified it
git add README.md
```

### 4.4 Commit Changes

```bash
git commit -m "Add comprehensive documentation

- Add Doxygen configuration (Doxyfile)
- Add User Manual with quick start guide
- Add Developer Guide with architecture overview
- Add Quick Reference for Doxygen syntax
- Add examples demonstrating usage
- Update .gitignore to exclude generated docs"
```

### 4.5 Push to GitHub

```bash
# Push to your main branch
git push origin main

# Or if you're using master:
git push origin master
```

### 4.6 Set Up GitHub Pages (Optional)

To host documentation online:

**Method 1: Using gh-pages branch**

```bash
# Create a new branch for documentation
git checkout --orphan gh-pages

# Remove all files
git rm -rf .

# Copy generated HTML docs
cp -r docs/doxygen/html/* .

# Add a .nojekyll file (tells GitHub not to use Jekyll)
touch .nojekyll

# Commit and push
git add .
git commit -m "Initial documentation"
git push origin gh-pages

# Switch back to main branch
git checkout main
```

Then enable GitHub Pages:
1. Go to your repo on GitHub
2. Settings → Pages
3. Source: gh-pages branch
4. Your docs will be at: `https://yourusername.github.io/Advance_ROOT_Plotting/`

**Method 2: Using docs/ folder on main branch**

```bash
# Generate docs to docs/html instead of docs/doxygen/html
# Update Doxyfile:
# OUTPUT_DIRECTORY = docs
# HTML_OUTPUT = html
```

Then in GitHub:
1. Settings → Pages
2. Source: main branch, /docs folder

### 4.7 Add Documentation Badge to README

Add a badge showing docs are available:

```markdown
# Advanced ROOT Plotting

![Documentation](https://img.shields.io/badge/docs-doxygen-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

[📚 Documentation](https://yourusername.github.io/Advance_ROOT_Plotting/)
```

## Step 5: Maintain Documentation

### 5.1 Update When Code Changes

**Create a pre-commit hook:**

```bash
# Create hook file
nano .git/hooks/pre-commit
```

Add this script:
```bash
#!/bin/bash
# Pre-commit hook to regenerate documentation

# Only regenerate if header files changed
if git diff --cached --name-only | grep -q '\.h$'; then
    echo "Header files changed, regenerating documentation..."
    doxygen Doxyfile > /dev/null 2>&1
    
    if [ $? -eq 0 ]; then
        echo "✓ Documentation regenerated successfully"
    else
        echo "✗ Warning: Documentation generation had errors"
    fi
fi
```

Make it executable:
```bash
chmod +x .git/hooks/pre-commit
```

### 5.2 Regular Documentation Review

Set reminders to review:
- Monthly: Check for outdated information
- Each release: Update version numbers
- When adding features: Document in User Manual
- When changing architecture: Update Developer Guide

### 5.3 Documentation Checklist

Before each release:
- [ ] All new public APIs documented
- [ ] Examples updated
- [ ] User Manual reflects new features
- [ ] Developer Guide updated if architecture changed
- [ ] Doxygen generates without warnings
- [ ] Version numbers updated
- [ ] Changelog updated

## Quick Command Reference

```bash
# Navigate to project
cd /path/to/Advance_ROOT_Plotting

# Generate documentation
doxygen Doxyfile

# View documentation
open docs/doxygen/html/index.html

# Check for documentation warnings
doxygen Doxyfile 2>&1 | grep -i warning

# Count documented vs undocumented items
grep -r "@brief" include/ | wc -l

# Update and push to GitHub
git add .
git commit -m "Update documentation"
git push origin main

# Regenerate and deploy to GitHub Pages
doxygen Doxyfile
git checkout gh-pages
cp -r docs/doxygen/html/* .
git add .
git commit -m "Update docs"
git push origin gh-pages
git checkout main
```

## Common Issues and Solutions

### Issue: Doxygen shows warnings about undocumented members

**Solution:**
```bash
# Find undocumented members
doxygen Doxyfile 2>&1 | grep "not documented"

# Add @brief comments to all listed items
```

### Issue: Graphs not showing in generated docs

**Solution:**
```bash
# Install GraphViz
sudo apt-get install graphviz

# Verify in Doxyfile:
# HAVE_DOT = YES
# DOT_PATH = (should be empty, auto-detected)
```

### Issue: Documentation not updating after code changes

**Solution:**
```bash
# Clean and regenerate
rm -rf docs/doxygen/
doxygen Doxyfile
```

### Issue: Can't push to gh-pages

**Solution:**
```bash
# If branch doesn't exist
git checkout -b gh-pages
git push -u origin gh-pages

# If branch exists
git checkout gh-pages
git pull origin gh-pages
# ... make changes ...
git push origin gh-pages
```

## Next Steps After Implementation

1. **Share documentation link** with users and contributors
2. **Add to project description** on GitHub
3. **Include in README badges**
4. **Update regularly** as you add features
5. **Solicit feedback** from users on documentation clarity

## Resources

- [Doxygen Manual](https://www.doxygen.nl/manual/)
- [GitHub Pages Guide](https://pages.github.com/)
- [Markdown Guide](https://www.markdownguide.org/)
- [Git Documentation](https://git-scm.com/doc)

---

**Need help?** Open an issue on your GitHub repository or consult the QuickReference.md guide.

Good luck with your documentation! 🚀
