# CHTL Language Support for VS Code

This extension provides comprehensive language support for CHTL (Concise HyperText Language) in Visual Studio Code.

## Features

### Syntax Highlighting
- Full syntax highlighting for all CHTL constructs
- Support for comments, strings, keywords, and special blocks
- CSS property highlighting in style blocks
- HTML element recognition

### Auto-completion
- HTML elements with snippets
- CHTL keywords and special blocks
- CSS properties and values
- Predefined styles and elements
- Context-aware suggestions

### Code Snippets
- HTML template boilerplate
- Common CHTL patterns
- Custom/Template definitions
- Style blocks and selectors
- Form elements and layouts

### Live Preview
- Real-time preview of compiled HTML
- Auto-refresh on save
- WebSocket-based live reload
- Integrated preview panel

### Diagnostics
- Syntax error detection
- Missing semicolon warnings
- Unclosed brace detection
- Duplicate ID warnings
- Import validation

### Other Features
- Hover documentation
- Go to definition for @Style and @Element references
- Document formatting
- Bracket matching and auto-closing

## Requirements

- CHTL compiler (`chtlc`) must be installed and accessible in PATH
- VS Code 1.74.0 or higher

## Installation

1. Install from VS Code Marketplace (search for "CHTL")
2. Or install from VSIX file: `code --install-extension chtl-1.0.0.vsix`

## Usage

### Basic Usage
1. Open any `.chtl` file
2. The extension will automatically activate
3. Use `Ctrl+Shift+V` (or `Cmd+Shift+V` on Mac) to open preview

### Commands
- **CHTL: Compile Current File** - Compile the active CHTL file to HTML
- **CHTL: Open Preview** - Open live preview in a side panel
- **CHTL: Refresh Preview** - Manually refresh the preview

### Configuration
```json
{
  "chtl.compiler.path": "chtlc",
  "chtl.preview.autoRefresh": true,
  "chtl.preview.port": 3000
}
```

## Example CHTL Code

```chtl
html {
    head {
        title { text { "My CHTL Page" } }
    }
    body {
        div {
            style {
                .container {
                    @Style FlexCenter;
                    padding: "20px";
                }
                &:hover {
                    background: "#f0f0f0";
                }
            }
            
            h1 { text { "Welcome to CHTL!" } }
            
            @Element Card {
                p { text { "This is a predefined card component." } }
            }
        }
    }
}
```

## Keyboard Shortcuts

| Command | Windows/Linux | Mac |
|---------|--------------|-----|
| Open Preview | `Ctrl+Shift+V` | `Cmd+Shift+V` |
| Format Document | `Shift+Alt+F` | `Shift+Option+F` |
| Go to Definition | `F12` | `F12` |
| Trigger Suggestions | `Ctrl+Space` | `Cmd+Space` |

## Known Issues

- Preview server requires ports 3000 and 3001 to be available
- Some complex CHTL configurations may not be fully supported in syntax highlighting

## Contributing

Report issues and contribute at: https://github.com/chtl-lang/vscode-chtl

## License

MIT License - see LICENSE file for details