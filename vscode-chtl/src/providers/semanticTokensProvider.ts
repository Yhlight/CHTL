import * as vscode from 'vscode';
import { ConfigurationParser } from '../services/configurationParser';

export class ChtlSemanticTokensProvider implements vscode.DocumentSemanticTokensProvider {
    private static readonly tokenTypes = ['keyword', 'class', 'function', 'variable', 'string', 'number', 'operator'];
    private static readonly tokenModifiers = ['declaration', 'definition', 'readonly', 'static', 'deprecated'];
    
    public static readonly legend = new vscode.SemanticTokensLegend(
        ChtlSemanticTokensProvider.tokenTypes,
        ChtlSemanticTokensProvider.tokenModifiers
    );

    public provideDocumentSemanticTokens(
        document: vscode.TextDocument,
        token: vscode.CancellationToken
    ): vscode.ProviderResult<vscode.SemanticTokens> {
        const tokensBuilder = new vscode.SemanticTokensBuilder(ChtlSemanticTokensProvider.legend);
        
        // Get custom keywords from configuration
        const customKeywords = ConfigurationParser.getAllCustomKeywords(document);
        const text = document.getText();
        const lines = text.split('\n');
        
        for (let lineIndex = 0; lineIndex < lines.length; lineIndex++) {
            const line = lines[lineIndex];
            
            // Find custom keywords in the line
            customKeywords.forEach(keyword => {
                const regex = new RegExp(`\\b${keyword}\\b`, 'g');
                let match;
                
                while ((match = regex.exec(line)) !== null) {
                    const startIndex = match.index;
                    const length = keyword.length;
                    
                    // Check if this is really a keyword usage (not inside string or comment)
                    if (this.isKeywordContext(line, startIndex)) {
                        // Add semantic token
                        tokensBuilder.push(
                            lineIndex,
                            startIndex,
                            length,
                            0, // keyword token type
                            0  // no modifiers
                        );
                    }
                }
            });
            
            // Also highlight configuration block keywords
            if (line.includes('[Configuration]')) {
                const match = line.match(/\[Configuration\]/);
                if (match && match.index !== undefined) {
                    tokensBuilder.push(
                        lineIndex,
                        match.index,
                        match[0].length,
                        2, // function token type
                        1  // definition modifier
                    );
                }
            }
            
            // Highlight [Name] blocks
            if (line.includes('[Name]')) {
                const match = line.match(/\[Name\]/);
                if (match && match.index !== undefined) {
                    tokensBuilder.push(
                        lineIndex,
                        match.index,
                        match[0].length,
                        2, // function token type
                        1  // definition modifier
                    );
                }
            }
        }
        
        return tokensBuilder.build();
    }

    private isKeywordContext(line: string, index: number): boolean {
        // Simple check to avoid highlighting inside strings or comments
        const beforeIndex = line.substring(0, index);
        
        // Check if we're inside a string
        const doubleQuotes = (beforeIndex.match(/"/g) || []).length;
        const singleQuotes = (beforeIndex.match(/'/g) || []).length;
        if (doubleQuotes % 2 !== 0 || singleQuotes % 2 !== 0) {
            return false;
        }
        
        // Check if we're inside a comment
        if (beforeIndex.includes('//') || beforeIndex.includes('/*')) {
            return false;
        }
        
        return true;
    }
}