import * as vscode from 'vscode';
import * as path from 'path';
import { PreviewServer } from './previewServer';

export class PreviewPanel {
    public static currentPanel: PreviewPanel | undefined;
    public static readonly viewType = 'chtlPreview';

    private readonly _panel: vscode.WebviewPanel;
    private readonly _extensionUri: vscode.Uri;
    private readonly _documentUri: vscode.Uri;
    private _disposables: vscode.Disposable[] = [];
    private _server: PreviewServer;

    public static createOrShow(extensionUri: vscode.Uri, documentUri: vscode.Uri) {
        const column = vscode.window.activeTextEditor
            ? vscode.window.activeTextEditor.viewColumn
            : undefined;

        // If we already have a panel, show it
        if (PreviewPanel.currentPanel) {
            PreviewPanel.currentPanel._panel.reveal(column);
            PreviewPanel.currentPanel.update(documentUri);
            return;
        }

        // Otherwise, create a new panel
        const panel = vscode.window.createWebviewPanel(
            PreviewPanel.viewType,
            'CHTL Preview',
            vscode.ViewColumn.Two,
            {
                enableScripts: true,
                localResourceRoots: [vscode.Uri.joinPath(extensionUri, 'media')]
            }
        );

        PreviewPanel.currentPanel = new PreviewPanel(panel, extensionUri, documentUri);
    }

    public static refresh() {
        if (PreviewPanel.currentPanel) {
            PreviewPanel.currentPanel._update();
        }
    }

    public static dispose() {
        if (PreviewPanel.currentPanel) {
            PreviewPanel.currentPanel.dispose();
        }
    }

    private constructor(panel: vscode.WebviewPanel, extensionUri: vscode.Uri, documentUri: vscode.Uri) {
        this._panel = panel;
        this._extensionUri = extensionUri;
        this._documentUri = documentUri;

        // Initialize preview server
        this._server = new PreviewServer();
        this._server.start();

        // Set the webview's initial html content
        this._update();

        // Listen for when the panel is disposed
        this._panel.onDidDispose(() => this.dispose(), null, this._disposables);

        // Update the content based on view changes
        this._panel.onDidChangeViewState(
            e => {
                if (this._panel.visible) {
                    this._update();
                }
            },
            null,
            this._disposables
        );

        // Handle messages from the webview
        this._panel.webview.onDidReceiveMessage(
            message => {
                switch (message.command) {
                    case 'refresh':
                        this._update();
                        return;
                }
            },
            null,
            this._disposables
        );
    }

    public update(documentUri: vscode.Uri) {
        this._documentUri = documentUri;
        this._update();
    }

    public dispose() {
        PreviewPanel.currentPanel = undefined;

        // Clean up server
        this._server.stop();

        // Clean up our resources
        this._panel.dispose();

        while (this._disposables.length) {
            const x = this._disposables.pop();
            if (x) {
                x.dispose();
            }
        }
    }

    private _update() {
        const webview = this._panel.webview;
        this._panel.title = `Preview: ${path.basename(this._documentUri.fsPath)}`;
        this._panel.webview.html = this._getHtmlForWebview(webview);
    }

    private _getHtmlForWebview(webview: vscode.Webview) {
        // Get the preview URL from the server
        const previewUrl = this._server.getPreviewUrl(this._documentUri);

        return `<!DOCTYPE html>
            <html lang="en">
            <head>
                <meta charset="UTF-8">
                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                <title>CHTL Preview</title>
                <style>
                    body {
                        margin: 0;
                        padding: 0;
                        overflow: hidden;
                    }
                    iframe {
                        width: 100vw;
                        height: 100vh;
                        border: none;
                    }
                    .toolbar {
                        position: fixed;
                        top: 10px;
                        right: 10px;
                        background: rgba(0, 0, 0, 0.8);
                        color: white;
                        padding: 5px 10px;
                        border-radius: 5px;
                        font-family: Arial, sans-serif;
                        font-size: 12px;
                        z-index: 1000;
                    }
                    .toolbar button {
                        background: #007ACC;
                        color: white;
                        border: none;
                        padding: 5px 10px;
                        margin-left: 5px;
                        border-radius: 3px;
                        cursor: pointer;
                    }
                    .toolbar button:hover {
                        background: #005a9e;
                    }
                    .error {
                        position: fixed;
                        top: 50%;
                        left: 50%;
                        transform: translate(-50%, -50%);
                        background: #f44336;
                        color: white;
                        padding: 20px;
                        border-radius: 5px;
                        font-family: Arial, sans-serif;
                        display: none;
                    }
                </style>
            </head>
            <body>
                <div class="toolbar">
                    <span>CHTL Preview</span>
                    <button onclick="refreshPreview()">Refresh</button>
                    <button onclick="openDevTools()">DevTools</button>
                </div>
                <iframe id="preview" src="${previewUrl}"></iframe>
                <div id="error" class="error"></div>
                
                <script>
                    const vscode = acquireVsCodeApi();
                    const iframe = document.getElementById('preview');
                    const errorDiv = document.getElementById('error');
                    
                    // Auto-refresh via WebSocket
                    const ws = new WebSocket('ws://localhost:3001');
                    
                    ws.onmessage = (event) => {
                        if (event.data === 'refresh') {
                            iframe.src = iframe.src;
                        }
                    };
                    
                    ws.onerror = (error) => {
                        console.error('WebSocket error:', error);
                    };
                    
                    function refreshPreview() {
                        iframe.src = iframe.src;
                        vscode.postMessage({ command: 'refresh' });
                    }
                    
                    function openDevTools() {
                        // This would typically open browser dev tools
                        console.log('DevTools requested');
                    }
                    
                    // Handle iframe errors
                    iframe.onerror = () => {
                        errorDiv.textContent = 'Failed to load preview';
                        errorDiv.style.display = 'block';
                    };
                    
                    iframe.onload = () => {
                        errorDiv.style.display = 'none';
                    };
                </script>
            </body>
            </html>`;
    }
}