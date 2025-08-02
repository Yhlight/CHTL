import * as vscode from 'vscode';

export class ChtlFormattingProvider implements vscode.DocumentFormattingEditProvider {
    public provideDocumentFormattingEdits(
        document: vscode.TextDocument,
        options: vscode.FormattingOptions,
        token: vscode.CancellationToken
    ): vscode.TextEdit[] {
        const edits: vscode.TextEdit[] = [];
        const text = document.getText();
        const formatted = this.formatChtl(text, options);
        
        const fullRange = new vscode.Range(
            document.positionAt(0),
            document.positionAt(text.length)
        );
        
        edits.push(vscode.TextEdit.replace(fullRange, formatted));
        return edits;
    }

    private formatChtl(text: string, options: vscode.FormattingOptions): string {
        const lines = text.split('\n');
        const formattedLines: string[] = [];
        let indentLevel = 0;
        const indent = options.insertSpaces ? ' '.repeat(options.tabSize) : '\t';

        for (let line of lines) {
            const trimmedLine = line.trim();
            
            // Skip empty lines
            if (trimmedLine.length === 0) {
                formattedLines.push('');
                continue;
            }

            // Decrease indent for closing braces
            if (trimmedLine.startsWith('}')) {
                indentLevel = Math.max(0, indentLevel - 1);
            }

            // Apply indentation
            const indentedLine = indent.repeat(indentLevel) + trimmedLine;
            formattedLines.push(indentedLine);

            // Increase indent for opening braces
            if (trimmedLine.endsWith('{')) {
                indentLevel++;
            }

            // Handle single-line blocks
            if (trimmedLine.includes('{') && trimmedLine.includes('}')) {
                const openCount = (trimmedLine.match(/{/g) || []).length;
                const closeCount = (trimmedLine.match(/}/g) || []).length;
                indentLevel += openCount - closeCount;
            }
        }

        return formattedLines.join('\n');
    }
}