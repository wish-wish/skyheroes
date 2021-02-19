CompletionItemProvider
CompletionItem
SymbolInformation

ReferenceProvider
DefinitionProvider
CodeActionProvider
CodeLensProvider


ImplementationProvider
TypeDefinitionProvider
HoverProvider
DocumentHighlightProvider
DocumentSymbolProvider
WorkspaceSymbolProvider
RenameProvider
DocumentFormattingEditProvider
DocumentRangeFormattingEditProvider
OnTypeFormattingEditProvider
SignatureHelpProvider
DocumentLinkProvider
DocumentColorProvider
QuickDiffProvider

export enum CompletionItemKind {
    Text = 0,//文字
    Method = 1,//方法
    Function = 2,//函数
    Constructor = 3,//构造函数
    Field = 4,//字段
    Variable = 5,//变量
    Class = 6,//类
    Interface = 7,//接口
    Module = 8,//模块
    Property = 9,//属性
    Unit = 10,//单元
    Value = 11,//值
    Enum = 12,//枚举
    Keyword = 13,//关键字
    Snippet = 14,//程序段
    Color = 15,//颜色
    Reference = 17,//引用
    File = 16,//文件
    Folder = 18,//文件夹
    EnumMember = 19,//枚举成员
    Constant = 20,//常量
    Struct = 21,//结构
    Event = 22,//时间
    Operator = 23,//操作符
    TypeParameter = 24//类型参数
}
export enum SymbolKind {
    File = 0,
    Module = 1,
    Namespace = 2,
    Package = 3,
    Class = 4,
    Method = 5,
    Property = 6,
    Field = 7,
    Constructor = 8,
    Enum = 9,
    Interface = 10,
    Function = 11,
    Variable = 12,
    Constant = 13,
    String = 14,
    Number = 15,
    Boolean = 16,
    Array = 17,
    Object = 18,
    Key = 19,
    Null = 20,
    EnumMember = 21,
    Struct = 22,
    Event = 23,
    Operator = 24,
    TypeParameter = 25
}