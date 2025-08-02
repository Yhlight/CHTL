import * as vscode from 'vscode';

export interface CustomKeywords {
    text?: string;
    style?: string;
    class?: string;
    id?: string;
    [key: string]: string | undefined;
}

export interface ConfigurationData {
    customKeywords: CustomKeywords;
    nameBlock: CustomKeywords;
}

export class ConfigurationParser {
    private static configCache = new Map<string, ConfigurationData>();

    /**
     * Parse configuration block from CHTL document
     */
    public static parseConfiguration(document: vscode.TextDocument): ConfigurationData {
        const uri = document.uri.toString();
        const text = document.getText();
        
        // Check cache first
        const cached = this.configCache.get(uri);
        if (cached && this.isCacheValid(document, cached)) {
            return cached;
        }

        const config: ConfigurationData = {
            customKeywords: {},
            nameBlock: {}
        };

        // Find [Configuration] block
        const configMatch = text.match(/\[Configuration\]\s*\{([^}]+)\}/s);
        if (configMatch) {
            const configContent = configMatch[1];
            
            // Parse custom keywords
            this.parseKeywordMappings(configContent, config.customKeywords);
            
            // Parse [Name] block
            const nameMatch = configContent.match(/\[Name\]\s*\{([^}]+)\}/s);
            if (nameMatch) {
                this.parseKeywordMappings(nameMatch[1], config.nameBlock);
            }
        }

        // Cache the result
        this.configCache.set(uri, config);
        return config;
    }

    /**
     * Parse keyword mappings from configuration content
     */
    private static parseKeywordMappings(content: string, target: CustomKeywords): void {
        // Match pattern: keyword = value;
        const keywordPattern = /(\w+)\s*=\s*([^;]+);/g;
        let match;
        
        while ((match = keywordPattern.exec(content)) !== null) {
            const keyword = match[1].trim();
            const value = match[2].trim();
            
            // Remove quotes if present
            const cleanValue = value.replace(/^["']|["']$/g, '');
            target[keyword] = cleanValue;
        }

        // Also parse group options like (opt1, opt2, opt3)
        const groupPattern = /(\w+)\s*\(([^)]+)\)/g;
        while ((match = groupPattern.exec(content)) !== null) {
            const keyword = match[1].trim();
            const options = match[2].split(',').map(opt => opt.trim());
            // Store first option as default
            if (options.length > 0) {
                target[keyword] = options[0];
            }
        }
    }

    /**
     * Check if cached configuration is still valid
     */
    private static isCacheValid(document: vscode.TextDocument, cached: ConfigurationData): boolean {
        // For now, we'll invalidate cache on any document change
        // In production, we'd check if configuration block specifically changed
        return false;
    }

    /**
     * Clear cache for a specific document
     */
    public static clearCache(uri: vscode.Uri): void {
        this.configCache.delete(uri.toString());
    }

    /**
     * Clear all cached configurations
     */
    public static clearAllCache(): void {
        this.configCache.clear();
    }

    /**
     * Get effective keyword for a given default keyword
     */
    public static getEffectiveKeyword(document: vscode.TextDocument, defaultKeyword: string): string {
        const config = this.parseConfiguration(document);
        
        // Check custom keywords first
        for (const [key, value] of Object.entries(config.customKeywords)) {
            if (key === defaultKeyword && value) {
                return value;
            }
        }
        
        // Check name block
        for (const [key, value] of Object.entries(config.nameBlock)) {
            if (key === defaultKeyword && value) {
                return value;
            }
        }
        
        // Return default if no custom mapping found
        return defaultKeyword;
    }

    /**
     * Get all custom keywords for a document
     */
    public static getAllCustomKeywords(document: vscode.TextDocument): string[] {
        const config = this.parseConfiguration(document);
        const keywords = new Set<string>();
        
        // Add custom keyword values
        Object.values(config.customKeywords).forEach(value => {
            if (value) keywords.add(value);
        });
        
        // Add name block values
        Object.values(config.nameBlock).forEach(value => {
            if (value) keywords.add(value);
        });
        
        return Array.from(keywords);
    }

    /**
     * Check if a word is a custom keyword
     */
    public static isCustomKeyword(document: vscode.TextDocument, word: string): boolean {
        const customKeywords = this.getAllCustomKeywords(document);
        return customKeywords.includes(word);
    }

    /**
     * Get the original keyword for a custom keyword
     */
    public static getOriginalKeyword(document: vscode.TextDocument, customKeyword: string): string | null {
        const config = this.parseConfiguration(document);
        
        // Search in custom keywords
        for (const [key, value] of Object.entries(config.customKeywords)) {
            if (value === customKeyword) {
                return key;
            }
        }
        
        // Search in name block
        for (const [key, value] of Object.entries(config.nameBlock)) {
            if (value === customKeyword) {
                return key;
            }
        }
        
        return null;
    }
}