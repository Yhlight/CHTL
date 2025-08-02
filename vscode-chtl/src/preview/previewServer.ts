import * as express from 'express';
import * as http from 'http';
import * as WebSocket from 'ws';
import * as path from 'path';
import * as fs from 'fs';
import * as vscode from 'vscode';

export class PreviewServer {
    private app: express.Express;
    private server: http.Server | null = null;
    private wss: WebSocket.Server | null = null;
    private port: number = 3000;
    private wsPort: number = 3001;
    private compiledFiles: Map<string, string> = new Map();

    constructor() {
        this.app = express();
        this.setupRoutes();
    }

    private setupRoutes() {
        // Serve compiled HTML files
        this.app.get('/preview/*', (req, res) => {
            const filePath = req.params[0];
            const compiledHtml = this.compiledFiles.get(filePath);
            
            if (compiledHtml) {
                res.setHeader('Content-Type', 'text/html');
                res.send(compiledHtml);
            } else {
                res.status(404).send(`
                    <html>
                        <body style="font-family: Arial, sans-serif; text-align: center; padding: 50px;">
                            <h1>CHTL Preview Not Available</h1>
                            <p>Please compile the CHTL file first.</p>
                        </body>
                    </html>
                `);
            }
        });

        // Health check
        this.app.get('/health', (req, res) => {
            res.json({ status: 'ok' });
        });
    }

    public start(): Promise<void> {
        return new Promise((resolve, reject) => {
            try {
                // Start HTTP server
                this.server = this.app.listen(this.port, () => {
                    console.log(`CHTL Preview server running on http://localhost:${this.port}`);
                });

                // Start WebSocket server for live reload
                this.wss = new WebSocket.Server({ port: this.wsPort });
                
                this.wss.on('connection', (ws) => {
                    console.log('WebSocket client connected');
                    
                    ws.on('close', () => {
                        console.log('WebSocket client disconnected');
                    });
                });

                resolve();
            } catch (error) {
                reject(error);
            }
        });
    }

    public stop() {
        if (this.server) {
            this.server.close();
            this.server = null;
        }
        
        if (this.wss) {
            this.wss.close();
            this.wss = null;
        }
    }

    public updateCompiledFile(chtlPath: string, htmlContent: string) {
        const key = this.getFileKey(chtlPath);
        this.compiledFiles.set(key, htmlContent);
        
        // Notify all connected clients to refresh
        if (this.wss) {
            this.wss.clients.forEach((client) => {
                if (client.readyState === WebSocket.OPEN) {
                    client.send('refresh');
                }
            });
        }
    }

    public getPreviewUrl(documentUri: vscode.Uri): string {
        const key = this.getFileKey(documentUri.fsPath);
        return `http://localhost:${this.port}/preview/${key}`;
    }

    private getFileKey(filePath: string): string {
        // Create a normalized key for the file
        return filePath.replace(/\\/g, '/').replace(/^\//, '');
    }

    public clearCache() {
        this.compiledFiles.clear();
    }

    public getPort(): number {
        return this.port;
    }

    public getWebSocketPort(): number {
        return this.wsPort;
    }

    public isRunning(): boolean {
        return this.server !== null;
    }

    public async updateFromFile(chtlPath: string, htmlPath: string) {
        try {
            const htmlContent = await fs.promises.readFile(htmlPath, 'utf-8');
            this.updateCompiledFile(chtlPath, htmlContent);
        } catch (error) {
            console.error('Failed to read compiled HTML file:', error);
        }
    }
}