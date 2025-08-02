import * as vscode from 'vscode';

export class ChtlHoverProvider implements vscode.HoverProvider {
    private chtlDocs: { [key: string]: string } = {
        // CHTL keywords
        'text': 'Creates a text node. Usage: `text { "content" }`',
        'style': 'Creates a style block. Can be used globally or locally within elements.',
        'class': 'Sets the CSS class attribute. Usage: `class: "classname";`',
        'id': 'Sets the element ID. Usage: `id: "element-id";`',
        
        // Special blocks
        '[Custom]': 'Defines custom styles, elements, or variables that can be reused.',
        '[Template]': 'Defines template styles, elements, or variables for inheritance.',
        '[Configuration]': 'Configures CHTL compiler settings and custom keywords.',
        '[Import]': 'Imports other CHTL files. Usage: `[Import] "path/to/file.chtl"`',
        '[Namespace]': 'Creates a namespace scope. Usage: `[Namespace] name { ... }`',
        '[Origin]': 'Embeds raw HTML, CSS, or JavaScript code.',
        
        // Special tags
        '@Style': 'References or defines a style group.',
        '@Element': 'References or defines a custom element.',
        '@Var': 'References or defines a variable group.',
        '@Html': 'Embeds raw HTML in [Origin] blocks.',
        '@JavaScript': 'Embeds raw JavaScript in [Origin] blocks.',
        
        // Operators
        'add': 'Adds properties to a template. Usage: `add { ... }`',
        'delete': 'Removes properties from a template. Usage: `delete { prop1; prop2; }`',
        'inherit': 'Inherits from a template. Usage: `inherit TemplateName;`',
        'expect': 'Validates element structure. Usage: `expect precise <div>;`',
        'not': 'Negates an expectation. Usage: `not expect <script>;`'
    };

    public provideHover(
        document: vscode.TextDocument,
        position: vscode.Position,
        token: vscode.CancellationToken
    ): vscode.ProviderResult<vscode.Hover> {
        const wordRange = document.getWordRangeAtPosition(position);
        if (!wordRange) {
            return undefined;
        }

        const word = document.getText(wordRange);
        
        // Check for special blocks
        const line = document.lineAt(position.line).text;
        const blockMatch = line.match(/\[(Custom|Template|Configuration|Import|Namespace|Origin)\]/);
        if (blockMatch) {
            const blockType = `[${blockMatch[1]}]`;
            if (this.chtlDocs[blockType]) {
                return new vscode.Hover(this.chtlDocs[blockType]);
            }
        }

        // Check for documentation
        if (this.chtlDocs[word]) {
            return new vscode.Hover(this.chtlDocs[word]);
        }

        // Check for CSS properties
        if (this.isInStyleContext(document, position)) {
            const cssDoc = this.getCssPropertyDoc(word);
            if (cssDoc) {
                return new vscode.Hover(cssDoc);
            }
        }

        return undefined;
    }

    private isInStyleContext(document: vscode.TextDocument, position: vscode.Position): boolean {
        // Simple check - could be improved
        const text = document.getText(new vscode.Range(new vscode.Position(0, 0), position));
        const styleCount = (text.match(/\bstyle\s*{/g) || []).length;
        const closeBraceCount = (text.match(/}/g) || []).length;
        return styleCount > closeBraceCount;
    }

    private getCssPropertyDoc(property: string): string | undefined {
        const cssDocs: { [key: string]: string } = {
            'color': 'Sets the text color. Example: `color: "#333";`',
            'background': 'Sets the background. Example: `background: "#f0f0f0";`',
            'display': 'Sets the display type. Values: block, inline, flex, grid, none',
            'position': 'Sets positioning. Values: static, relative, absolute, fixed, sticky',
            'margin': 'Sets the margin. Example: `margin: "10px";`',
            'padding': 'Sets the padding. Example: `padding: "10px 20px";`',
            'width': 'Sets the width. Example: `width: "100%";`',
            'height': 'Sets the height. Example: `height: "50px";`'
        };

        return cssDocs[property];
    }
}