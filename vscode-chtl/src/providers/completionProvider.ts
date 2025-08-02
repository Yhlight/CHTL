import * as vscode from 'vscode';
import { ConfigurationParser } from '../services/configurationParser';

export class ChtlCompletionProvider implements vscode.CompletionItemProvider {
    private htmlElements = [
        'html', 'head', 'body', 'div', 'span', 'p', 'a', 'img', 'ul', 'ol', 'li',
        'table', 'tr', 'td', 'th', 'form', 'input', 'button', 'select', 'option',
        'textarea', 'h1', 'h2', 'h3', 'h4', 'h5', 'h6', 'header', 'footer', 'nav',
        'main', 'section', 'article', 'aside', 'figure', 'figcaption', 'video',
        'audio', 'canvas', 'svg', 'script', 'link', 'meta', 'title'
    ];

    private attributes = [
        'class', 'id', 'href', 'src', 'alt', 'title', 'type', 'name', 'value',
        'placeholder', 'content', 'action', 'method', 'target', 'rel', 'style'
    ];

    private cssProperties = [
        'color', 'background', 'background-color', 'font-size', 'font-family',
        'font-weight', 'margin', 'padding', 'border', 'width', 'height', 'display',
        'position', 'top', 'right', 'bottom', 'left', 'flex', 'grid', 'transform',
        'transition', 'animation', 'opacity', 'visibility', 'z-index', 'overflow',
        'cursor', 'outline', 'box-shadow', 'text-align', 'text-decoration',
        'line-height', 'vertical-align', 'white-space', 'content'
    ];

    private chtlKeywords = [
        'text', 'style', 'add', 'delete', 'inherit', 'expect', 'not'
    ];

    private specialBlocks = [
        '[Custom]', '[Template]', '[Configuration]', '[Import]', '[Namespace]', '[Origin]'
    ];

    private specialTags = [
        '@Style', '@Element', '@Var', '@Html', '@JavaScript'
    ];

    private predefinedStyles = [
        'DefaultText', 'Heading1', 'PrimaryColor', 'FlexCenter', 'BorderDefault',
        'PaddingSmall', 'FadeIn', 'MobileOnly'
    ];

    private predefinedElements = [
        'Container', 'Card', 'Header', 'Navbar', 'FormGroup', 'Article', 'Alert',
        'Modal', 'MediaObject', 'ListGroup'
    ];

    public provideCompletionItems(
        document: vscode.TextDocument,
        position: vscode.Position,
        token: vscode.CancellationToken,
        context: vscode.CompletionContext
    ): vscode.ProviderResult<vscode.CompletionItem[] | vscode.CompletionList> {
        const line = document.lineAt(position);
        const lineText = line.text.substring(0, position.character);
        const items: vscode.CompletionItem[] = [];
        
        // Get custom keywords from configuration
        const customKeywords = ConfigurationParser.getAllCustomKeywords(document);

        // Check context
        if (context.triggerCharacter === '@') {
            // After @ symbol
            this.specialTags.forEach(tag => {
                const item = new vscode.CompletionItem(tag.substring(1), vscode.CompletionItemKind.Keyword);
                item.insertText = tag.substring(1);
                item.detail = `CHTL ${tag} tag`;
                items.push(item);
            });

            // Add predefined styles/elements after @
            if (lineText.endsWith('@')) {
                this.predefinedStyles.forEach(style => {
                    const item = new vscode.CompletionItem(`Style ${style}`, vscode.CompletionItemKind.Reference);
                    item.insertText = `Style ${style}`;
                    item.detail = 'Predefined style';
                    items.push(item);
                });

                this.predefinedElements.forEach(element => {
                    const item = new vscode.CompletionItem(`Element ${element}`, vscode.CompletionItemKind.Reference);
                    item.insertText = `Element ${element}`;
                    item.detail = 'Predefined element';
                    items.push(item);
                });
            }
        } else if (context.triggerCharacter === '[') {
            // After [ symbol
            this.specialBlocks.forEach(block => {
                const item = new vscode.CompletionItem(block.substring(1, block.length - 1), vscode.CompletionItemKind.Module);
                item.insertText = block.substring(1, block.length - 1) + ']';
                item.detail = `CHTL ${block} block`;
                items.push(item);
            });
        } else if (context.triggerCharacter === '.') {
            // Class selector
            const item = new vscode.CompletionItem('class-name', vscode.CompletionItemKind.Class);
            item.insertText = new vscode.SnippetString('${1:class-name}');
            item.detail = 'CSS class selector';
            items.push(item);
        } else if (context.triggerCharacter === '#') {
            // ID selector
            const item = new vscode.CompletionItem('id-name', vscode.CompletionItemKind.Reference);
            item.insertText = new vscode.SnippetString('${1:id-name}');
            item.detail = 'CSS ID selector';
            items.push(item);
        } else if (context.triggerCharacter === ':') {
            // Check if we're in a style context
            if (this.isInStyleContext(document, position)) {
                // CSS property values
                const property = this.getCurrentProperty(lineText);
                if (property) {
                    const values = this.getCssPropertyValues(property);
                    values.forEach(value => {
                        const item = new vscode.CompletionItem(value, vscode.CompletionItemKind.Value);
                        item.insertText = ` "${value}";`;
                        items.push(item);
                    });
                }
            } else {
                // Attribute values
                this.attributes.forEach(attr => {
                    const item = new vscode.CompletionItem(attr, vscode.CompletionItemKind.Property);
                    item.insertText = new vscode.SnippetString(' "${1:value}";');
                    item.detail = `${attr} attribute`;
                    items.push(item);
                });
            }
        } else {
            // General context
            // HTML elements
            this.htmlElements.forEach(element => {
                const item = new vscode.CompletionItem(element, vscode.CompletionItemKind.Class);
                item.insertText = new vscode.SnippetString(`${element} {\n\t$0\n}`);
                item.detail = `HTML ${element} element`;
                item.documentation = `Creates a ${element} element`;
                items.push(item);
            });

            // CHTL keywords (including custom keywords)
            this.chtlKeywords.forEach(keyword => {
                // Get the effective keyword (might be customized)
                const effectiveKeyword = ConfigurationParser.getEffectiveKeyword(document, keyword);
                
                const item = new vscode.CompletionItem(effectiveKeyword, vscode.CompletionItemKind.Keyword);
                if (keyword === 'text') {
                    item.insertText = new vscode.SnippetString(`${effectiveKeyword} { "$1" }`);
                } else if (keyword === 'style') {
                    item.insertText = new vscode.SnippetString(`${effectiveKeyword} {\n\t$0\n}`);
                }
                item.detail = `CHTL ${keyword} keyword${effectiveKeyword !== keyword ? ' (customized)' : ''}`;
                items.push(item);
            });
            
            // Add custom keywords that don't override defaults
            customKeywords.forEach(customKeyword => {
                const originalKeyword = ConfigurationParser.getOriginalKeyword(document, customKeyword);
                if (!originalKeyword || !this.chtlKeywords.includes(originalKeyword)) {
                    const item = new vscode.CompletionItem(customKeyword, vscode.CompletionItemKind.Keyword);
                    item.detail = 'Custom keyword';
                    items.push(item);
                }
            });

            // CSS properties (if in style context)
            if (this.isInStyleContext(document, position)) {
                this.cssProperties.forEach(prop => {
                    const item = new vscode.CompletionItem(prop, vscode.CompletionItemKind.Property);
                    item.insertText = new vscode.SnippetString(`${prop}: "$1";`);
                    item.detail = `CSS ${prop} property`;
                    items.push(item);
                });
            }
        }

        return items;
    }

    private isInStyleContext(document: vscode.TextDocument, position: vscode.Position): boolean {
        let braceCount = 0;
        let inStyle = false;

        for (let i = 0; i <= position.line; i++) {
            const line = document.lineAt(i).text;
            const endChar = i === position.line ? position.character : line.length;

            for (let j = 0; j < endChar; j++) {
                if (line[j] === '{') {
                    braceCount++;
                } else if (line[j] === '}') {
                    braceCount--;
                    if (braceCount === 0) inStyle = false;
                }

                if (line.substring(j).startsWith('style') && j + 5 < line.length) {
                    const nextChar = line[j + 5];
                    if (nextChar === ' ' || nextChar === '{') {
                        inStyle = true;
                    }
                }
            }
        }

        return inStyle && braceCount > 0;
    }

    private getCurrentProperty(lineText: string): string | null {
        const match = lineText.match(/(\w+)\s*:\s*$/);
        return match ? match[1] : null;
    }

    private getCssPropertyValues(property: string): string[] {
        const valueMap: { [key: string]: string[] } = {
            'display': ['none', 'block', 'inline', 'inline-block', 'flex', 'grid', 'table'],
            'position': ['static', 'relative', 'absolute', 'fixed', 'sticky'],
            'text-align': ['left', 'center', 'right', 'justify'],
            'font-weight': ['normal', 'bold', 'lighter', 'bolder', '100', '200', '300', '400', '500', '600', '700', '800', '900'],
            'cursor': ['pointer', 'default', 'text', 'wait', 'help', 'move', 'not-allowed'],
            'overflow': ['visible', 'hidden', 'scroll', 'auto'],
            'visibility': ['visible', 'hidden', 'collapse']
        };

        return valueMap[property] || [];
    }
}