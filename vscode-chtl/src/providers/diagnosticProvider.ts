import * as vscode from 'vscode';
import { ConfigurationParser } from '../services/configurationParser';

export class ChtlDiagnosticProvider {
    private diagnosticCollection: vscode.DiagnosticCollection;

    constructor() {
        this.diagnosticCollection = vscode.languages.createDiagnosticCollection('chtl');
    }

    public updateDiagnostics(document: vscode.TextDocument): void {
        if (document.languageId !== 'chtl') {
            return;
        }

        const diagnostics: vscode.Diagnostic[] = [];
        const text = document.getText();
        const lines = text.split('\n');

        for (let i = 0; i < lines.length; i++) {
            const line = lines[i];
            
            // Check for unclosed braces
            this.checkUnclosedBraces(line, i, diagnostics);
            
            // Check for missing semicolons
            this.checkMissingSemicolons(line, i, diagnostics);
            
            // Check for invalid imports
            this.checkInvalidImports(line, i, diagnostics);
            
            // Check for duplicate IDs
            this.checkDuplicateIds(text, line, i, diagnostics);
            
            // Check custom keyword usage
            this.checkCustomKeywordUsage(document, line, i, diagnostics);
        }

        // Check overall structure
        this.checkOverallStructure(text, diagnostics, document);

        this.diagnosticCollection.set(document.uri, diagnostics);
    }

    private checkUnclosedBraces(line: string, lineNumber: number, diagnostics: vscode.Diagnostic[]): void {
        let braceCount = 0;
        for (const char of line) {
            if (char === '{') braceCount++;
            else if (char === '}') braceCount--;
        }

        if (braceCount > 0) {
            const range = new vscode.Range(lineNumber, line.length - 1, lineNumber, line.length);
            const diagnostic = new vscode.Diagnostic(
                range,
                'Unclosed brace',
                vscode.DiagnosticSeverity.Error
            );
            diagnostics.push(diagnostic);
        }
    }

    private checkMissingSemicolons(line: string, lineNumber: number, diagnostics: vscode.Diagnostic[]): void {
        // Check for attribute assignments without semicolons
        const attributePattern = /^\s*\w+\s*:\s*"[^"]*"\s*$/;
        if (attributePattern.test(line.trim())) {
            const range = new vscode.Range(lineNumber, line.length - 1, lineNumber, line.length);
            const diagnostic = new vscode.Diagnostic(
                range,
                'Missing semicolon after attribute',
                vscode.DiagnosticSeverity.Error
            );
            diagnostics.push(diagnostic);
        }
    }

    private checkInvalidImports(line: string, lineNumber: number, diagnostics: vscode.Diagnostic[]): void {
        if (line.includes('[Import]')) {
            const importPattern = /\[Import\]\s*"([^"]+)"/;
            const match = line.match(importPattern);
            
            if (!match) {
                const range = new vscode.Range(lineNumber, 0, lineNumber, line.length);
                const diagnostic = new vscode.Diagnostic(
                    range,
                    'Invalid import syntax. Use: [Import] "path/to/file.chtl"',
                    vscode.DiagnosticSeverity.Error
                );
                diagnostics.push(diagnostic);
            }
        }
    }

    private checkDuplicateIds(text: string, line: string, lineNumber: number, diagnostics: vscode.Diagnostic[]): void {
        const idPattern = /id\s*:\s*"([^"]+)"/;
        const match = line.match(idPattern);
        
        if (match && match[1]) {
            const id = match[1];
            const allMatches = text.match(new RegExp(`id\\s*:\\s*"${id}"`, 'g'));
            
            if (allMatches && allMatches.length > 1) {
                const range = new vscode.Range(lineNumber, match.index || 0, lineNumber, (match.index || 0) + match[0].length);
                const diagnostic = new vscode.Diagnostic(
                    range,
                    `Duplicate ID: "${id}"`,
                    vscode.DiagnosticSeverity.Warning
                );
                diagnostics.push(diagnostic);
            }
        }
    }

    private checkOverallStructure(text: string, diagnostics: vscode.Diagnostic[], document: vscode.TextDocument): void {
        // Check for balanced braces
        let braceCount = 0;
        for (const char of text) {
            if (char === '{') braceCount++;
            else if (char === '}') braceCount--;
        }

        if (braceCount !== 0) {
            const lastLine = document.lineCount - 1;
            const range = new vscode.Range(lastLine, 0, lastLine, document.lineAt(lastLine).text.length);
            const diagnostic = new vscode.Diagnostic(
                range,
                `Unbalanced braces: ${braceCount > 0 ? 'missing closing brace' : 'extra closing brace'}`,
                vscode.DiagnosticSeverity.Error
            );
            diagnostics.push(diagnostic);
        }
    }

    private checkCustomKeywordUsage(document: vscode.TextDocument, line: string, lineNumber: number, diagnostics: vscode.Diagnostic[]): void {
        // Check if configuration block exists
        const text = document.getText();
        const hasConfig = text.includes('[Configuration]');
        
        if (!hasConfig) {
            // Check if any words look like custom keywords (non-standard identifiers)
            const unusualKeywords = /\b(texto|estilos?|clase|текст|样式|スタイル)\b/gi;
            let match;
            
            while ((match = unusualKeywords.exec(line)) !== null) {
                const range = new vscode.Range(lineNumber, match.index, lineNumber, match.index + match[0].length);
                const diagnostic = new vscode.Diagnostic(
                    range,
                    `"${match[0]}" might be a custom keyword. Consider adding a [Configuration] block to define custom keywords.`,
                    vscode.DiagnosticSeverity.Information
                );
                diagnostic.code = 'custom-keyword-hint';
                diagnostics.push(diagnostic);
            }
        }
    }

    public dispose(): void {
        this.diagnosticCollection.dispose();
    }
}