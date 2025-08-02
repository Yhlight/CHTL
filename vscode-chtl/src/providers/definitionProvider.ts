import * as vscode from 'vscode';

export class ChtlDefinitionProvider implements vscode.DefinitionProvider {
    public provideDefinition(
        document: vscode.TextDocument,
        position: vscode.Position,
        token: vscode.CancellationToken
    ): vscode.ProviderResult<vscode.Definition | vscode.LocationLink[]> {
        const wordRange = document.getWordRangeAtPosition(position);
        if (!wordRange) {
            return undefined;
        }

        const word = document.getText(wordRange);
        const line = document.lineAt(position.line).text;

        // Check for @Style references
        if (line.includes('@Style') && word !== 'Style') {
            return this.findStyleDefinition(document, word);
        }

        // Check for @Element references
        if (line.includes('@Element') && word !== 'Element') {
            return this.findElementDefinition(document, word);
        }

        // Check for [Import] statements
        if (line.includes('[Import]')) {
            return this.findImportedFile(document, position);
        }

        return undefined;
    }

    private findStyleDefinition(document: vscode.TextDocument, styleName: string): vscode.Location[] {
        const locations: vscode.Location[] = [];
        const text = document.getText();
        
        // Look for [Custom] @Style definitions
        const customStyleRegex = new RegExp(`\\[Custom\\]\\s*@Style\\s+${styleName}\\s*\\{`, 'g');
        let match;
        
        while ((match = customStyleRegex.exec(text)) !== null) {
            const pos = document.positionAt(match.index);
            locations.push(new vscode.Location(document.uri, pos));
        }

        // Look for [Template] @Style definitions
        const templateStyleRegex = new RegExp(`\\[Template\\]\\s*@Style\\s+${styleName}\\s*\\{`, 'g');
        
        while ((match = templateStyleRegex.exec(text)) !== null) {
            const pos = document.positionAt(match.index);
            locations.push(new vscode.Location(document.uri, pos));
        }

        return locations;
    }

    private findElementDefinition(document: vscode.TextDocument, elementName: string): vscode.Location[] {
        const locations: vscode.Location[] = [];
        const text = document.getText();
        
        // Look for [Custom] @Element definitions
        const customElementRegex = new RegExp(`\\[Custom\\]\\s*@Element\\s+${elementName}\\s*\\{`, 'g');
        let match;
        
        while ((match = customElementRegex.exec(text)) !== null) {
            const pos = document.positionAt(match.index);
            locations.push(new vscode.Location(document.uri, pos));
        }

        // Look for [Template] @Element definitions
        const templateElementRegex = new RegExp(`\\[Template\\]\\s*@Element\\s+${elementName}\\s*\\{`, 'g');
        
        while ((match = templateElementRegex.exec(text)) !== null) {
            const pos = document.positionAt(match.index);
            locations.push(new vscode.Location(document.uri, pos));
        }

        return locations;
    }

    private async findImportedFile(document: vscode.TextDocument, position: vscode.Position): Promise<vscode.Location | undefined> {
        const line = document.lineAt(position.line).text;
        const importMatch = line.match(/\[Import\]\s*"([^"]+)"/);
        
        if (importMatch && importMatch[1]) {
            const importPath = importMatch[1];
            const currentDir = vscode.Uri.joinPath(document.uri, '..');
            const importUri = vscode.Uri.joinPath(currentDir, importPath);
            
            try {
                await vscode.workspace.fs.stat(importUri);
                return new vscode.Location(importUri, new vscode.Position(0, 0));
            } catch {
                // File doesn't exist
                return undefined;
            }
        }
        
        return undefined;
    }
}