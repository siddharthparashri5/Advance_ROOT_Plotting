#!/bin/bash

################################################################################
# Documentation Setup Script for Advanced ROOT Plotting
# 
# This script automates the setup of documentation in your project.
# It will:
#   1. Create necessary directories
#   2. Copy documentation files
#   3. Customize settings for your project
#   4. Generate initial documentation
#   5. Set up git integration
#
# Usage:
#   chmod +x setup_docs.sh
#   ./setup_docs.sh
#
# Author: Siddharth Parashri
# Date: 2026-02-04
################################################################################

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Helper functions
print_header() {
    echo -e "\n${BLUE}========================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}========================================${NC}\n"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_info() {
    echo -e "${BLUE}ℹ $1${NC}"
}

# Check if we're in a git repository
check_git_repo() {
    if ! git rev-parse --git-dir > /dev/null 2>&1; then
        print_error "Not in a git repository!"
        print_info "Please run this script from your project root directory."
        exit 1
    fi
    print_success "Git repository detected"
}

# Check if required tools are installed
check_dependencies() {
    print_header "Checking Dependencies"
    
    local all_installed=true
    
    # Check for doxygen
    if command -v doxygen &> /dev/null; then
        print_success "Doxygen is installed ($(doxygen --version))"
    else
        print_warning "Doxygen is NOT installed"
        echo "  Install with: sudo apt-get install doxygen (Ubuntu) or brew install doxygen (macOS)"
        all_installed=false
    fi
    
    # Check for dot (GraphViz)
    if command -v dot &> /dev/null; then
        print_success "GraphViz is installed"
    else
        print_warning "GraphViz is NOT installed (optional, for diagrams)"
        echo "  Install with: sudo apt-get install graphviz (Ubuntu) or brew install graphviz (macOS)"
    fi
    
    # Check for git
    if command -v git &> /dev/null; then
        print_success "Git is installed"
    else
        print_error "Git is NOT installed"
        all_installed=false
    fi
    
    if [ "$all_installed" = false ]; then
        print_error "Please install missing dependencies before continuing."
        exit 1
    fi
}

# Get user input for customization
get_user_input() {
    print_header "Project Customization"
    
    # Get project name
    echo -n "Enter your project name [Advanced ROOT Plotting]: "
    read PROJECT_NAME
    PROJECT_NAME=${PROJECT_NAME:-"Advanced ROOT Plotting"}
    
    # Get author name
    echo -n "Enter your name [Siddharth Parashri]: "
    read AUTHOR_NAME
    AUTHOR_NAME=${AUTHOR_NAME:-"Siddharth Parashri"}
    
    # Get author email
    echo -n "Enter your email (optional): "
    read AUTHOR_EMAIL
    
    # Get GitHub username
    echo -n "Enter your GitHub username (for links): "
    read GITHUB_USER
    
    # Get repository name
    REPO_NAME=$(basename `git rev-parse --show-toplevel`)
    echo -n "Repository name [$REPO_NAME]: "
    read TEMP_REPO_NAME
    REPO_NAME=${TEMP_REPO_NAME:-$REPO_NAME}
    
    print_info "Using project name: $PROJECT_NAME"
    print_info "Using author: $AUTHOR_NAME"
    if [ ! -z "$AUTHOR_EMAIL" ]; then
        print_info "Using email: $AUTHOR_EMAIL"
    fi
    print_info "Using GitHub: $GITHUB_USER/$REPO_NAME"
}

# Create directory structure
create_directories() {
    print_header "Creating Directory Structure"
    
    mkdir -p docs/images
    mkdir -p docs/doxygen
    mkdir -p examples
    
    print_success "Created docs/ directory"
    print_success "Created docs/images/ directory"
    print_success "Created docs/doxygen/ directory"
    print_success "Created examples/ directory"
}

# Check if documentation files are in current directory
check_doc_files() {
    print_header "Checking for Documentation Files"
    
    local files_found=true
    
    if [ ! -f "Doxyfile" ]; then
        print_warning "Doxyfile not found in current directory"
        files_found=false
    fi
    
    if [ ! -f "docs/UserManual.md" ] && [ ! -f "UserManual.md" ]; then
        print_warning "UserManual.md not found"
        files_found=false
    fi
    
    if [ "$files_found" = false ]; then
        print_error "Some documentation files are missing."
        print_info "Please ensure all documentation files are in the current directory."
        print_info "Expected files:"
        echo "  - Doxyfile"
        echo "  - docs/UserManual.md (or UserManual.md)"
        echo "  - docs/DeveloperGuide.md (or DeveloperGuide.md)"
        echo "  - QuickReference.md"
        exit 1
    fi
    
    print_success "All documentation files found"
}

# Move documentation files to correct locations
organize_files() {
    print_header "Organizing Documentation Files"
    
    # Move files if they're in current directory
    if [ -f "UserManual.md" ]; then
        mv UserManual.md docs/
        print_success "Moved UserManual.md to docs/"
    fi
    
    if [ -f "DeveloperGuide.md" ]; then
        mv DeveloperGuide.md docs/
        print_success "Moved DeveloperGuide.md to docs/"
    fi
    
    if [ -f "QuickReference.md" ]; then
        mv QuickReference.md docs/
        print_success "Moved QuickReference.md to docs/"
    fi
    
    if [ -f "README_DOCS.md" ]; then
        mv README_DOCS.md docs/
        print_success "Moved README_DOCS.md to docs/"
    fi
}

# Customize Doxyfile
customize_doxyfile() {
    print_header "Customizing Doxyfile"
    
    if [ ! -f "Doxyfile" ]; then
        print_error "Doxyfile not found!"
        return 1
    fi
    
    # Backup original
    cp Doxyfile Doxyfile.bak
    print_info "Created backup: Doxyfile.bak"
    
    # Update project name
    sed -i.tmp "s/PROJECT_NAME           = \".*\"/PROJECT_NAME           = \"$PROJECT_NAME\"/" Doxyfile
    print_success "Updated project name"
    
    # Update author in markdown files
    if [ ! -z "$AUTHOR_NAME" ]; then
        find docs/ -name "*.md" -type f -exec sed -i.tmp "s/Siddharth Parashri/$AUTHOR_NAME/g" {} \;
        print_success "Updated author name in documentation"
    fi
    
    # Update GitHub links if provided
    if [ ! -z "$GITHUB_USER" ] && [ ! -z "$REPO_NAME" ]; then
        find docs/ -name "*.md" -type f -exec sed -i.tmp "s|siddharthparashri5/Advance_ROOT_Plotting|$GITHUB_USER/$REPO_NAME|g" {} \;
        print_success "Updated GitHub repository links"
    fi
    
    # Clean up temporary files
    find . -name "*.tmp" -delete
    
    print_success "Doxyfile customization complete"
}

# Create .gitignore for documentation
create_gitignore() {
    print_header "Setting up .gitignore"
    
    # Check if .gitignore exists
    if [ -f ".gitignore" ]; then
        # Append to existing .gitignore
        if ! grep -q "docs/doxygen/html/" .gitignore; then
            cat >> .gitignore << EOF

# Documentation (auto-generated)
docs/doxygen/html/
docs/doxygen/latex/
docs/doxygen/xml/
*.tmp
*~

EOF
            print_success "Updated existing .gitignore"
        else
            print_info ".gitignore already contains documentation entries"
        fi
    else
        # Create new .gitignore
        cat > .gitignore << EOF
# Documentation (auto-generated)
docs/doxygen/html/
docs/doxygen/latex/
docs/doxygen/xml/

# Build files
build/
*.o
*.so

# ROOT files
*.root

# Backup and temporary files
*.bak
*.tmp
*~

# IDE files
.vscode/
.idea/
*.swp

EOF
        print_success "Created new .gitignore"
    fi
}

# Update README.md with documentation links
update_readme() {
    print_header "Updating README.md"
    
    if [ ! -f "README.md" ]; then
        print_warning "README.md not found, skipping update"
        return 0
    fi
    
    # Check if documentation section already exists
    if grep -q "## 📚 Documentation" README.md; then
        print_info "Documentation section already exists in README.md"
        return 0
    fi
    
    # Backup README
    cp README.md README.md.bak
    
    # Add documentation section
    cat >> README.md << EOF

## 📚 Documentation

Complete documentation is available in the \`docs/\` directory:

- **[User Manual](docs/UserManual.md)** - Complete guide for using the application
  - Quick start guide
  - Feature descriptions
  - Example workflows
  - Troubleshooting

- **[Developer Guide](docs/DeveloperGuide.md)** - Guide for extending the codebase
  - Architecture overview
  - How to add new plot types
  - How to add new file formats
  - How to add new fit functions

- **[API Documentation](docs/doxygen/html/index.html)** - Auto-generated from code comments
  - Generate with: \`doxygen Doxyfile\`
  - View in browser after generation

- **[Quick Reference](docs/QuickReference.md)** - Doxygen syntax cheat sheet

### Generating API Documentation

\`\`\`bash
# Install Doxygen (if not already installed)
sudo apt-get install doxygen graphviz  # Ubuntu/Debian
# or
brew install doxygen graphviz          # macOS

# Generate documentation
doxygen Doxyfile

# View documentation
open docs/doxygen/html/index.html      # macOS
xdg-open docs/doxygen/html/index.html  # Linux
\`\`\`

EOF
    
    print_success "Added documentation section to README.md"
    print_info "Backup saved as README.md.bak"
}

# Generate initial documentation
generate_docs() {
    print_header "Generating Initial Documentation"
    
    if ! command -v doxygen &> /dev/null; then
        print_warning "Doxygen not installed, skipping documentation generation"
        print_info "Install Doxygen and run: doxygen Doxyfile"
        return 0
    fi
    
    print_info "Running Doxygen (this may take a moment)..."
    
    if doxygen Doxyfile > /dev/null 2>&1; then
        print_success "Documentation generated successfully!"
        print_info "View at: docs/doxygen/html/index.html"
    else
        print_warning "Documentation generation completed with warnings"
        print_info "Check warnings with: doxygen Doxyfile"
    fi
}

# Create git commit
git_commit() {
    print_header "Git Integration"
    
    echo -n "Do you want to commit these changes to git? [y/N]: "
    read COMMIT_CHOICE
    
    if [[ "$COMMIT_CHOICE" =~ ^[Yy]$ ]]; then
        git add docs/ Doxyfile .gitignore README.md 2>/dev/null || true
        
        git commit -m "Add comprehensive documentation

- Add Doxygen configuration (Doxyfile)
- Add User Manual with quick start guide
- Add Developer Guide with architecture overview
- Add Quick Reference for Doxygen syntax
- Update .gitignore to exclude generated docs
- Update README.md with documentation links"
        
        print_success "Changes committed to git"
        
        echo -n "Do you want to push to GitHub? [y/N]: "
        read PUSH_CHOICE
        
        if [[ "$PUSH_CHOICE" =~ ^[Yy]$ ]]; then
            git push
            print_success "Changes pushed to GitHub"
        fi
    else
        print_info "Skipped git commit"
        print_info "You can commit manually with: git add . && git commit -m 'Add documentation'"
    fi
}

# Print next steps
print_next_steps() {
    print_header "Setup Complete! 🎉"
    
    echo -e "${GREEN}Documentation has been successfully set up!${NC}\n"
    
    echo "Next steps:"
    echo "  1. Add Doxygen comments to your source files (see docs/QuickReference.md)"
    echo "  2. Generate docs: doxygen Doxyfile"
    echo "  3. View docs: open docs/doxygen/html/index.html"
    echo "  4. Read the Implementation Guide for detailed instructions"
    echo ""
    echo "Documentation files:"
    echo "  ├── Doxyfile                    (Doxygen configuration)"
    echo "  ├── docs/"
    echo "  │   ├── UserManual.md          (User guide)"
    echo "  │   ├── DeveloperGuide.md      (Developer guide)"
    echo "  │   ├── QuickReference.md      (Doxygen cheat sheet)"
    echo "  │   └── doxygen/               (Generated HTML docs)"
    echo "  └── README.md                   (Updated with doc links)"
    echo ""
    echo "Useful commands:"
    echo "  doxygen Doxyfile                     # Generate documentation"
    echo "  doxygen Doxyfile 2>&1 | grep warning # Check for warnings"
    echo "  open docs/doxygen/html/index.html    # View documentation"
    echo ""
    
    if [ ! -z "$GITHUB_USER" ]; then
        echo -e "${BLUE}To set up GitHub Pages:${NC}"
        echo "  1. Push your changes: git push"
        echo "  2. Go to: https://github.com/$GITHUB_USER/$REPO_NAME/settings/pages"
        echo "  3. Set source to 'gh-pages' branch or 'main branch /docs folder'"
        echo "  4. Your docs will be at: https://$GITHUB_USER.github.io/$REPO_NAME/"
        echo ""
    fi
}

################################################################################
# Main Script
################################################################################

main() {
    echo -e "${BLUE}"
    echo "╔════════════════════════════════════════════════════════════╗"
    echo "║   Documentation Setup Script                               ║"
    echo "║   Advanced ROOT Plotting                                   ║"
    echo "╚════════════════════════════════════════════════════════════╝"
    echo -e "${NC}"
    
    check_git_repo
    check_dependencies
    get_user_input
    create_directories
    check_doc_files
    organize_files
    customize_doxyfile
    create_gitignore
    update_readme
    generate_docs
    git_commit
    print_next_steps
}

# Run main function
main "$@"
