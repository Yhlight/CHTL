import * as vscode from 'vscode';
import * as child_process from 'child_process';
import * as path from 'path';
import * as fs from 'fs';
import { PreviewPanel } from '../preview/previewPanel';

export class CompilerService {
    private context: vscode.ExtensionContext;
    private outputChannel: vscode.OutputChannel;

    constructor(context: vscode.ExtensionContext) {
        this.context = context;
        this.outputChannel = vscode.window.createOutputChannel('CHTL Compiler');
    }

    public async compileFile(fileUri: vscode.Uri): Promise<string> {
        const config = vscode.workspace.getConfiguration('chtl');
        const compilerPath = config.get<string>('compiler.path') || 'chtlc';
        
        const inputPath = fileUri.fsPath;
        const outputPath = this.getOutputPath(inputPath);
        
        return new Promise((resolve, reject) => {
            const args = [inputPath, outputPath];
            
            this.outputChannel.clear();
            this.outputChannel.appendLine(`Compiling ${path.basename(inputPath)}...`);
            this.outputChannel.appendLine(`Command: ${compilerPath} ${args.join(' ')}`);
            
            const process = child_process.spawn(compilerPath, args);
            
            let stdout = '';
            let stderr = '';
            
            process.stdout.on('data', (data) => {
                const text = data.toString();
                stdout += text;
                this.outputChannel.append(text);
            });
            
            process.stderr.on('data', (data) => {
                const text = data.toString();
                stderr += text;
                this.outputChannel.append(text);
            });
            
            process.on('close', (code) => {
                if (code === 0) {
                    this.outputChannel.appendLine('\nCompilation successful!');
                    
                    // Update preview if available
                    if (PreviewPanel.currentPanel) {
                        const server = (PreviewPanel.currentPanel as any)._server;
                        if (server) {
                            server.updateFromFile(inputPath, outputPath);
                        }
                    }
                    
                    resolve(outputPath);
                } else {
                    this.outputChannel.appendLine(`\nCompilation failed with code ${code}`);
                    this.outputChannel.show();
                    reject(new Error(stderr || `Compilation failed with code ${code}`));
                }
            });
            
            process.on('error', (error) => {
                this.outputChannel.appendLine(`\nError: ${error.message}`);
                this.outputChannel.show();
                reject(error);
            });
        });
    }

    private getOutputPath(inputPath: string): string {
        const dir = path.dirname(inputPath);
        const basename = path.basename(inputPath, '.chtl');
        return path.join(dir, `${basename}.html`);
    }

    public async compileAndPreview(fileUri: vscode.Uri) {
        try {
            const outputPath = await this.compileFile(fileUri);
            PreviewPanel.createOrShow(this.context.extensionUri, fileUri);
        } catch (error) {
            vscode.window.showErrorMessage(`Compilation failed: ${error}`);
        }
    }

    public dispose() {
        this.outputChannel.dispose();
    }
}