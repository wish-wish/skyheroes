//RAD动态完成
import vscode = require('vscode');
import { CompletionItem, CompletionItemKind,CompletionList,TextDocument
    ,Position,CompletionContext,CancellationToken } from "vscode";

export class RadCompletionItemProvider implements vscode.CompletionItemProvider {//: ProviderResult<CompletionItem[] | CompletionList>;
    provideCompletionItems(document: TextDocument, position: Position
        , token: CancellationToken, context: CompletionContext){
            let ci:Array<CompletionItem>=new Array<CompletionItem>();
            let cl:CompletionList=new CompletionList(ci,false);
            
        return cl;
    }
    resolveCompletionItem(item: CompletionItem, token: CancellationToken){//:ProviderResult<CompletionItem>
        return item;
    }
}