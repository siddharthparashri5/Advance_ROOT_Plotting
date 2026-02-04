# Documentation Implementation Checklist

Use this checklist to track your progress in adding documentation to your project.

## ✅ Pre-Setup (5 minutes)

- [ ] Download all documentation files from this package
- [ ] Have Doxygen installed (`doxygen --version`)
- [ ] Have GraphViz installed (`dot -V`) - optional but recommended
- [ ] Have Git installed (`git --version`)
- [ ] Have your project cloned locally
- [ ] Know your GitHub username and repository name

## ✅ Quick Setup (10 minutes) - Option 1: Automated

- [ ] Copy all files to your project root directory
- [ ] Run the setup script: `chmod +x setup_docs.sh && ./setup_docs.sh`
- [ ] Answer the prompts (project name, author, etc.)
- [ ] Review the generated documentation
- [ ] Commit changes to git (script will prompt)

## ✅ Manual Setup (30 minutes) - Option 2: Step by Step

### Step 1: Copy Files
- [ ] Copy `Doxyfile` to project root
- [ ] Copy `docs/UserManual.md` to your `docs/` folder
- [ ] Copy `docs/DeveloperGuide.md` to your `docs/` folder
- [ ] Copy `QuickReference.md` to your `docs/` folder

### Step 2: Customize Doxyfile
- [ ] Open `Doxyfile` in a text editor
- [ ] Update `PROJECT_NAME` (line ~35)
- [ ] Update `INPUT` directories to match your structure (line ~930)
- [ ] Save the file

### Step 3: Update Documentation Files
- [ ] Open `docs/UserManual.md`
- [ ] Replace "Your Name" with your actual name
- [ ] Update GitHub links with your username/repo
- [ ] Repeat for `DeveloperGuide.md`

### Step 4: Set Up Git
- [ ] Create/update `.gitignore` with:
  ```
  docs/doxygen/html/
  docs/doxygen/latex/
  docs/doxygen/xml/
  ```
- [ ] Update `README.md` with documentation links

### Step 5: Generate Documentation
- [ ] Run: `doxygen Doxyfile`
- [ ] Check for errors/warnings
- [ ] Open `docs/doxygen/html/index.html` in browser
- [ ] Verify it looks correct

### Step 6: Commit to Git
- [ ] Run: `git add docs/ Doxyfile .gitignore README.md`
- [ ] Run: `git commit -m "Add documentation"`
- [ ] Run: `git push`

## ✅ Add Code Documentation (Ongoing)

### Document Your Header Files

For each `.h` file in your project:

- [ ] Add file header documentation
  ```cpp
  /**
   * @file YourFile.h
   * @brief Brief description
   * @author Your Name
   * @date 2026-02-04
   */
  ```

- [ ] Add class documentation
  ```cpp
  /**
   * @class YourClass
   * @brief Brief description
   * @details Detailed description
   */
  ```

- [ ] Add method documentation
  ```cpp
  /**
   * @brief What it does
   * @param name Parameter description
   * @return Return value description
   */
  ```

- [ ] Add member variable documentation
  ```cpp
  Int_t fCount;  ///< Description of member
  ```

### Track Your Progress

Track which files you've documented:

- [ ] `include/AdvancedPlotGUI.h`
- [ ] `include/DataManager.h`
- [ ] `include/PlotManager.h`
- [ ] `include/FitEngine.h`
- [ ] Other files: ________________

## ✅ Test Documentation (5 minutes)

- [ ] Run: `doxygen Doxyfile`
- [ ] Check for warnings: `doxygen Doxyfile 2>&1 | grep warning`
- [ ] Open HTML output in browser
- [ ] Verify all classes appear in class list
- [ ] Verify all methods are documented
- [ ] Check that graphs/diagrams appear (if using GraphViz)
- [ ] Test navigation between pages

## ✅ Optional: GitHub Pages Setup (15 minutes)

### Method 1: gh-pages branch
- [ ] Run: `git checkout -b gh-pages`
- [ ] Run: `git rm -rf .`
- [ ] Run: `cp -r docs/doxygen/html/* .`
- [ ] Run: `touch .nojekyll`
- [ ] Run: `git add .`
- [ ] Run: `git commit -m "Initial docs"`
- [ ] Run: `git push origin gh-pages`
- [ ] Go to GitHub repo → Settings → Pages
- [ ] Set source to `gh-pages` branch
- [ ] Wait 1-2 minutes
- [ ] Visit: `https://yourusername.github.io/yourrepo/`

### Method 2: docs folder on main
- [ ] Update Doxyfile: `OUTPUT_DIRECTORY = docs`
- [ ] Update Doxyfile: `HTML_OUTPUT = html`
- [ ] Run: `doxygen Doxyfile`
- [ ] Commit and push changes
- [ ] Go to GitHub repo → Settings → Pages
- [ ] Set source to `main` branch, `/docs` folder
- [ ] Visit: `https://yourusername.github.io/yourrepo/`

## ✅ Add Documentation Badge (2 minutes)

- [ ] Add to your `README.md`:
  ```markdown
  ![Documentation](https://img.shields.io/badge/docs-doxygen-blue.svg)
  [![GitHub Pages](https://img.shields.io/badge/docs-online-success.svg)](https://yourusername.github.io/yourrepo/)
  ```

## ✅ Maintenance Tasks

### Every Commit (if modifying public APIs)
- [ ] Update Doxygen comments in changed files
- [ ] Regenerate docs: `doxygen Doxyfile`
- [ ] Commit generated docs if using GitHub Pages

### Monthly Review
- [ ] Check for outdated information
- [ ] Review TODO items in documentation
- [ ] Update examples if APIs changed
- [ ] Check for broken links

### Every Release
- [ ] Update version number in Doxyfile
- [ ] Update version in UserManual.md
- [ ] Update version in DeveloperGuide.md
- [ ] Regenerate all documentation
- [ ] Tag release in git

## 📊 Documentation Quality Metrics

Track your documentation coverage:

```bash
# Count documented functions
grep -r "@brief" include/ | wc -l

# Find undocumented items
doxygen Doxyfile 2>&1 | grep "not documented" | wc -l

# Check file count
ls -1 include/*.h | wc -l
```

Goal: 100% of public APIs documented!

- Files documented: ____ / ____
- Classes documented: ____ / ____
- Methods documented: ____ / ____

## ❓ Having Issues?

### Doxygen not found
```bash
# Ubuntu/Debian
sudo apt-get install doxygen graphviz

# macOS
brew install doxygen graphviz
```

### No graphs in documentation
```bash
# Install GraphViz
sudo apt-get install graphviz  # Ubuntu
brew install graphviz          # macOS

# Check Doxyfile has:
# HAVE_DOT = YES
```

### Can't push to GitHub
```bash
# Check remote
git remote -v

# Add if missing
git remote add origin https://github.com/username/repo.git

# Push
git push -u origin main
```

### Documentation looks wrong
```bash
# Clean and regenerate
rm -rf docs/doxygen/
doxygen Doxyfile
```

## 🎯 Success Criteria

You'll know you're done when:

- ✅ `doxygen Doxyfile` runs without errors
- ✅ HTML documentation opens in browser
- ✅ All your classes appear in the class list
- ✅ Public methods have descriptions
- ✅ Examples compile and run
- ✅ Documentation is committed to git
- ✅ (Optional) Docs are live on GitHub Pages

## 📚 Quick Reference

| Task | Command |
|------|---------|
| Generate docs | `doxygen Doxyfile` |
| View docs | `open docs/doxygen/html/index.html` |
| Check warnings | `doxygen Doxyfile 2>&1 \| grep warning` |
| Clean docs | `rm -rf docs/doxygen/` |
| Commit docs | `git add docs/ && git commit -m "Update docs"` |

## 📞 Getting Help

If stuck:
1. Check `IMPLEMENTATION_GUIDE.md` for detailed instructions
2. Review `QuickReference.md` for Doxygen syntax
3. Consult Doxygen manual: https://www.doxygen.nl/manual/
4. Open issue on GitHub repository

---

**Remember**: Good documentation is written incrementally. Don't try to document everything at once!

**Pro tip**: Document as you code. It's easier than documenting everything later.

Good luck! 🚀
