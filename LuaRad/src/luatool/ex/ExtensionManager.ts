/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */
'use strict';

import * as path from 'path';
import { ConstInfo } from "../../ConstInfo";
import { getConfigDir } from "../../Common"
var fs = require('fs');

import { workspace, Disposable, ExtensionContext, window, TextDocumentChangeEvent } from 'vscode';
import vscode = require('vscode');
import * as cce from '../ex/ChangeCaseExtension';
import * as cmf from '../ex/CreateMoudleFunction';
import * as CreateFunction from '../ex/CreateFunction';
import { LuaRadConfigManager } from '../ex/LuaRadConfigManager';
import { TemplateManager } from '../ex/Template/TemplateManager';
import { CreateTemplateFile } from '../ex/Template/CreateTemplateFile'
import { OpenLuaLuaScriptText } from '../ex/LoadLuaScript'
import { httpRequest } from "../../httpClient";


export class ExtensionManager {
    public static em: ExtensionManager;
    // public outPutChannel:vscode.OutputChannel;
    public  barItem:vscode.StatusBarItem;
    constructor(context: ExtensionContext) {
        ExtensionManager.em = this;
        this.InitEx(context)
    }
    public golbal = { context: null }
    public COMMAND_LABELS = {
        toUpperCase: 'toUpperCase',
        toLowerCase: 'toLowerCase',
        createModuleFunction: 'createModuleFunction',
        createFunction: "createFunction",
        createTemplateFile: "createTemplateFile",
        LoadLuaScript: "LoadLuaScript",
        chatShow: "chatShow",
        donate:"donate"

    };
    public COMMAND_DEFINITIONS = [
        { label: this.COMMAND_LABELS.toUpperCase, description: '转换为大写', func: cce.toUpperCase },
        { label: this.COMMAND_LABELS.toLowerCase, description: '转换为小写', func: cce.toLowerCase },
        { label: this.COMMAND_LABELS.createModuleFunction, description: '创建模块方法', func: cmf.createModuleFunction },
        { label: this.COMMAND_LABELS.createFunction, description: '创建方法', func: CreateFunction.createFunction },
        { label: this.COMMAND_LABELS.createTemplateFile, description: '创建模板文件', func: CreateTemplateFile.run },
        { label: this.COMMAND_LABELS.LoadLuaScript, description: '加载lua字符串', func: OpenLuaLuaScriptText },
        { label: this.COMMAND_LABELS.chatShow, description: '闲聊小功能', func: this.showChat },
        { label: this.COMMAND_LABELS.donate, description: '赞助', func: this.showdoNate },
    ];
    public showChat(e)
    {
        //  this.outPutChannel.show();
         
    }
    public showdoNate(e)
    {
        var extensionPath = ExtensionManager.em.luaRadConfigManager.extensionPath
        extensionPath = path.join(extensionPath, "images", "support.html")
        var previewUri = vscode.Uri.file(extensionPath);
        vscode.commands.executeCommand('vscode.previewHtml', previewUri, vscode.ViewColumn.One, "谢谢您的支持").then(value => {
    //    this.statisticsMain.sendMsg(StatisticsEvent.C2S_OpenRechrage)
        })
    }
    public TemplatePath = {
        CreateModuleFunctionTemplate: "Template\\CreateModuleFunctionTemplate.lua",
        CreateFunctionTemplate: "Template\\CreateFunctionTemplate.lua",
    }
    public luaRadConfigManager: LuaRadConfigManager;
    public templateManager: TemplateManager;
    public InitEx(context: ExtensionContext) {
        this.golbal.context = context;
        this.luaRadConfigManager = new LuaRadConfigManager();
        this.templateManager = new TemplateManager();
        this.luaRadConfigManager.showRecharge();

        vscode.commands.registerCommand('luarad.changecase.toLowerCase', (e) => { this.RunCommand(this.COMMAND_LABELS.toLowerCase,e) });
        vscode.commands.registerCommand('luarad.changecase.toUpperCase', (e) => { this.RunCommand(this.COMMAND_LABELS.toUpperCase,e) });
        vscode.commands.registerCommand('luarad.utils.createModuleFunction', (e) => { this.RunCommand(this.COMMAND_LABELS.createModuleFunction,e) });
        vscode.commands.registerCommand('luarad.utils.createFunction', (e) => { this.RunCommand(this.COMMAND_LABELS.createFunction,e) });
        vscode.commands.registerCommand('luarad.utils.createTemplateFile', (e) => { this.RunCommand(this.COMMAND_LABELS.createTemplateFile,e) });
        vscode.commands.registerCommand('luarad.utils.LoadLuaScript', (e) => { this.RunCommand(this.COMMAND_LABELS.LoadLuaScript,e) });
        vscode.commands.registerCommand('luarad.donate', (e) => { this.RunCommand(this.COMMAND_LABELS.donate,e) });
        
        this.barItem =	vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Left)
        // this.outPutChannel  = vscode.window.createOutputChannel("闲聊")
        // this.outPutChannel.show(true)
        // this.outPutChannel.appendLine("做着玩的一个小功能")
        context.subscriptions.push(this.barItem);
        // context.subscriptions.push(this.outPutChannel);
        this.barItem.tooltip = "为了luarad 更好的发展,请支持luarad."
        this.barItem.command = "luarad.donate"
        this.barItem.text = "赞助(luarad)"
        this.barItem.show();
        
        //httpRequest();
    }
    private RunCommand(cmd,e) {
        for (var i = 0; i < this.COMMAND_DEFINITIONS.length; i++) {
            if (this.COMMAND_DEFINITIONS[i].label == cmd) {
                this.COMMAND_DEFINITIONS[i].func(e)
                break;
            }
        }
        // getTemplateText(TemplatePath.CreateModuleFunctionTemplate)
    }
}