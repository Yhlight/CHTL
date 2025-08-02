import * as vscode from 'vscode';
import { ChtlCompletionProvider } from './providers/completionProvider';
import { ChtlHoverProvider } from './providers/hoverProvider';
import { ChtlDefinitionProvider } from './providers/definitionProvider';
import { ChtlDiagnosticProvider } from './providers/diagnosticProvider';
import { ChtlFormattingProvider } from './providers/formattingProvider';
import { PreviewPanel } from './preview/previewPanel';
import { CompilerService } from './services/compilerService';

export function activate(context: vscode.ExtensionContext) {
    console.log('CHTL extension is now active!');

    // Register language providers
    const selector = { language: 'chtl', scheme: 'file' };
    
    // Auto-completion provider
    context.subscriptions.push(
        vscode.languages.registerCompletionItemProvider(
            selector,
            new ChtlCompletionProvider(),
            '.', '@', '[', '#', '&', ':'
        )
    );

    // Hover provider
    context.subscriptions.push(
        vscode.languages.registerHoverProvider(
            selector,
            new ChtlHoverProvider()
        )
    );

    // Go to definition provider
    context.subscriptions.push(
        vscode.languages.registerDefinitionProvider(
            selector,
            new ChtlDefinitionProvider()
        )
    );

    // Diagnostics provider
    const diagnosticProvider = new ChtlDiagnosticProvider();
    context.subscriptions.push(diagnosticProvider);

    // Formatting provider
    context.subscriptions.push(
        vscode.languages.registerDocumentFormattingEditProvider(
            selector,
            new ChtlFormattingProvider()
        )
    );

    // Compiler service
    const compilerService = new CompilerService(context);

    // Register commands
    context.subscriptions.push(
        vscode.commands.registerCommand('chtl.compile', async () => {
            const editor = vscode.window.activeTextEditor;
            if (!editor) {
                vscode.window.showErrorMessage('No active CHTL file');
                return;
            }

            try {
                await compilerService.compileFile(editor.document.uri);
                vscode.window.showInformationMessage('CHTL compilation successful!');
            } catch (error) {
                vscode.window.showErrorMessage(`Compilation failed: ${error}`);
            }
        })
    );

    context.subscriptions.push(
        vscode.commands.registerCommand('chtl.preview', () => {
            const editor = vscode.window.activeTextEditor;
            if (!editor) {
                vscode.window.showErrorMessage('No active CHTL file');
                return;
            }

            PreviewPanel.createOrShow(context.extensionUri, editor.document.uri);
        })
    );

    context.subscriptions.push(
        vscode.commands.registerCommand('chtl.refreshPreview', () => {
            PreviewPanel.refresh();
        })
    );

    // Auto-compile on save
    context.subscriptions.push(
        vscode.workspace.onDidSaveTextDocument(async (document) => {
            if (document.languageId === 'chtl') {
                const config = vscode.workspace.getConfiguration('chtl');
                if (config.get<boolean>('preview.autoRefresh')) {
                    try {
                        await compilerService.compileFile(document.uri);
                        PreviewPanel.refresh();
                    } catch (error) {
                        console.error('Auto-compile error:', error);
                    }
                }
            }
        })
    );

    // Watch for active editor changes
    context.subscriptions.push(
        vscode.window.onDidChangeActiveTextEditor((editor) => {
            if (editor && editor.document.languageId === 'chtl') {
                diagnosticProvider.updateDiagnostics(editor.document);
            }
        })
    );
}

export function deactivate() {
    PreviewPanel.dispose();
}