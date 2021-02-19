#coding=utf-8
#__author__ = 'cclin'
#write by cclin 2015.8.27

import sys
import os,os.path
import re
import codecs
import xml.dom.minidom as minidom
from xml.etree import ElementTree as ET

class Stack():
    def __init__(self,size):
        self.size=size;
        self.stack=[];
        self.topi=-1;
    def push(self,ele):        #入栈之前检查栈是否已满
        if self.isfull():
            raise exception("out of range");
        else:
            self.stack.append(ele);
            self.topi=self.topi+1;
    def pop(self):             # 出栈之前检查栈是否为空
        if self.isempty():
            raise exception("stack is empty");
        else:
            self.topi=self.topi-1;
            return self.stack.pop();
    def top(self):
        if self.isempty():
            return "";
        else:
            return self.stack[len(self.stack)-1];
    def isfull(self):
        return self.topi+1==self.size;
    def isempty(self):
        return self.topi==-1;
    def count(self):
        return self.topi+1;
    def list(self):
        for struct in self.stack:
            print struct.sfile+","+struct.sname+","+struct.scmd+","+struct.stype;
    

class Queue():
    def __init__(self,size):
        self.size=size;
        self.front=-1;
        self.rear=-1;
        self.queue=[];
    def enqueue(self,ele):  #入队操作
        if self.isfull():
            raise exception("queue is full");
        else:
            self.queue.append(ele);
            self.rear=self.rear+1;
    def dequeue(self):      #出队操作
        if self.isempty():
            raise exception("queue is empty");
        else:
            self.front=self.front+1;
            return self.queue[self.front];
    def isfull(self):
        return self.rear-self.front+1==self.size;
    def isempty(self):
        return self.front==self.rear;
        
        
def fixed_writexml(self, writer, indent="", addindent="", newl=""):
    # indent = current indentation
    # addindent = indentation to add to higher levels
    # newl = newline string
    writer.write(indent+"<" + self.tagName)

    attrs = self._get_attributes()
    a_names = attrs.keys()
    a_names.sort()

    for a_name in a_names:
        writer.write(" %s=\"" % a_name)
        minidom._write_data(writer, attrs[a_name].value)
        writer.write("\"")
    if self.childNodes:
        if len(self.childNodes) == 1 \
          and self.childNodes[0].nodeType == minidom.Node.TEXT_NODE:
            writer.write(">")
            self.childNodes[0].writexml(writer, "", "", "")
            writer.write("</%s>%s" % (self.tagName, newl))
            return
        writer.write(">%s"%(newl))
        for node in self.childNodes:
            if node.nodeType is not minidom.Node.TEXT_NODE:
                node.writexml(writer,indent+addindent,addindent,newl)
        writer.write("%s</%s>%s" % (indent,self.tagName,newl))
    else:
        writer.write("/>%s"%(newl))

minidom.Element.writexml = fixed_writexml

def walk_dirs(dir,list,topdown=True):       
    for root, dirs, files in os.walk(dir, topdown):        
        for name in dirs:            
            path=os.path.join(root,name)+r"/"
            path=path.replace("\\","/")
            #pa=path.lower()
            list.append(path)
            print path

def walk_files(dir,list,topdown=True):
    for root, dirs, files in os.walk(dir, topdown):
        for name in files:
            ext=os.path.splitext(name)[1]
            file=os.path.join(root,name)     
            file=file.replace("\\","/")
            print file
            if (ext == '.lua') and  file.find(".svn")==-1 and file not in list:                
                list.append(file)
                #print file;
    
def getCurrentPath():
    return os.path.split(os.path.realpath(__file__))[0]+r"/"


def appendArrayToFile(rlist,rfile):        
    f=open(rfile,"a")    
    for line in rlist:
        f.write(line)
    f.flush()
    f.close()
    
def writeArrayToFile(rlist,rfile):        
    f=open(rfile,"w")    
    for line in rlist:
        f.write(line)
    f.flush()
    f.close()
    
def nappendArrayToFile(rlist,rfile):        
    f=open(rfile,"a")    
    for line in rlist:
        f.write(line+"\n")
    f.flush()
    f.close()
    
def nwriteArrayToFile(rlist,rfile):        
    f=open(rfile,"w")    
    for line in rlist:
        f.write(line+"\n")
    f.flush()
    f.close()


def writeLineToFile(line,rfile):
    f=open(rfile,"a")        
    f.write(line+"\n")
    f.flush()
    f.close()

def deleteFile(rfile):
    if os.path.exists(rfile):
        os.remove(rfile)
        
def deleteEmptyLine(rfile):
     f=open(rfile,"r")
     newlist=[]
     for line in f:        
        if line.strip()!="":            
            newlist.append(line)            
     wf=open(rfile,"w")     
     wf.truncate()
     wf.writelines(newlist)
    
def myhex(intid):    
    return '0x%08X'%intid;
    
def ishexstr(str):
    if str.find("0x")!=-1 or str.find("0X")!=-1:
        return True;
    return False;
    
def ConvertCmdID(infile,outfile):
    protocol=open(getCurrentPath()+infile,"r")    
    pattern=re.compile(r"\s*(?P<cmdstr>\w*)\s*=*\s*(?P<cmdid>\w*)\s*,\s*/*(?P<cmdmemo>[\s\S]*)")
    protocnt=[]
    protocnt.append("///tool generate, do not modify by you own except you know what to do!\n");
    protocnt.append("///tool author cclin 2015.08.24\n");
    protocnt.append("\n");
    cmdindex=0;
    for line in protocol:
        match=pattern.match(line)
        if match:
            '''
            if match.group("cmdstr")!="":
                print match.group("cmdstr")
            if match.group("cmdmemo")!="":
                print match.group("cmdmemo")
            '''
            isHex=True;
            if match.group("cmdid")!="":
                if ishexstr(match.group("cmdid")):
                    cmdid=int(match.group("cmdid"),16);
                    cmdindex=cmdid;
                    isHex=True;
                    cmdid_str=str(cmdid);
                else:
                    cmdid=int(match.group("cmdid"),10);
                    cmdindex=cmdid;
                    isHex=False;
                #print str(cmdid)+"="+myhex(cmdid);
                line=line.replace("=","\t\t=\t");
            else:
                cmdindex=cmdindex+1;
                line=line.replace(match.group("cmdstr"),match.group("cmdstr")+"\t = \t"+myhex(cmdindex));
            if isHex:
                cmdid_str=str(cmdindex);
            else:
                cmdid_str=myhex(cmdindex);
            if match.group("cmdmemo").strip()!="":
                line=line.replace("\n","//--"+cmdid_str+"\n");
            else:
                line=line.replace("\n","\t\t//--"+cmdid_str+"\n");
        protocnt.append(line);
    writeArrayToFile(protocnt,getCurrentPath()+outfile);
    print "convert done in="+infile+";out="+outfile;
    protocol.close();

def CommandToLua(infile,outfile):
    #convert skydream command
    protocol=open(getCurrentPath()+infile,"r")    
    pattern=re.compile(r"\s*(?P<cmdstr>\w*)\s*=*\s*(?P<cmdid>\w*)\s*,\s*/*(?P<cmdmemo>[\s\S]*)")
    pattern_line=re.compile(r"\s*//+(?P<cmdmemo>[\s\S]*)")
    protocnt=[]    
    protocnt.append("--tool generate, do not modify by you own except you know what to do \n");
    protocnt.append("--tool author cclin 2015.08.24\n");
    protocnt.append("\n");
    protocnt.append("module(...,package.seeall);\n");

    protodoc=[]
    out=os.path.split(outfile)[1];
    modulename=out.replace(os.path.splitext(outfile)[1],"");
    protodoc.append("\n");
    protodoc.append("--- @module "+modulename+"\n");
    protodoc.append("--- @field [parent=#global] #"+modulename+" "+modulename+"\n");
    protodoc.append("\n");

    cmdindex=0;
    for line in protocol:
        match=pattern.match(line);
        if match:
            if match.group("cmdid")!="":
                if ishexstr(match.group("cmdid")):
                    cmdid=int(match.group("cmdid"),16);
                    cmdindex=cmdid;
                    cmdid_str=str(cmdid);
                else:
                    cmdid=int(match.group("cmdid"),10);
                    cmdindex=cmdid;
            else:
                cmdindex=cmdindex+1;
            cmdid_str=str(cmdindex);
            protocnt.append(match.group("cmdstr")+"\t=\t"+myhex(cmdindex)+"\t--"+cmdid_str+"\t--"+match.group("cmdmemo").strip()+"\n");
            protodoc.append("-- @field [parent=#"+modulename+"] #"+match.group("cmdstr")+" "+match.group("cmdstr")+"\n");
        else:
            if line.strip()!="":
                mat=pattern_line.match(line);
                if mat and mat.group("cmdmemo").strip()!="":
                    protocnt.append("--"+mat.group("cmdmemo"));
            else:
                protocnt.append("\n");
    writeArrayToFile(protocnt,getCurrentPath()+outfile);
    print "convert command done in="+infile+";out="+outfile;
    appendArrayToFile(protodoc,getCurrentPath()+outfile);
    protocol.close();

class Field:
    def __init__(self):
        self.itype="";
        self.ftype="";
        self.fname="";
        self.fvalue="";
        self.fmemo="";
        self.fcount=0;
        self.isarray=False;
        self.isstruct=False;
        self.flen=0;
        self.fblen=0;
    itype="";
    ftype="";
    fname="";
    fvalue="";
    fmemo="";
    fcount=0;
    fblen="";
    flen=0;
    isarray=False;
    isstruct=False;
    flen=0;
    
class Struct:
    def __init__(self):
        self.sfile="";
        self.stype="";
        self.sname="";
        self.scmd="";
        self.wgcmd1="";
        self.wgcmd2="";
        self.sparent="";
        self.scmdid=0;
        self.sfields=[];
    sfile="";
    stype="";
    sname="";
    scmd="";
    wgcmd1="";
    wgcmd2="";
    sparent="";
    scmdid=0;
    sfields=[];

class ConvertFile:
    def __init__(self):
        self.cppfile="";
        self.luafile="";
        self.jsfile="";
        self.pyfile="";    
        self.structs=[];
        self.consts=[];
        self.defines=[];
    cppfile="";
    luafile="";
    jsfile="";
    pyfile="";
    structs=[];
    consts=[];
    defines=[];
    
def isConsts(match):
    vfilter=[];
    vfilter.append("");
    vfilter.append("a");    
    vfilter.append("dest");
    vfilter.append("CommandName");
    
    nfilter=[];
    nfilter.append("");
    nfilter.append("pragma");
    nfilter.append("defined");
    nfilter.append("lif");
    nfilter.append("VOID");
    nfilter.append("TRUE");
    nfilter.append("FALSE");
    nfilter.append("__ENTER_FUNCTION_FOXNET");
    nfilter.append("_MY_TRY");
    nfilter.append("_MY_CATCH");
    nfilter.append("SAFE_DELETE");
    nfilter.append("SAFE_DELETE_ARRAY");
    nfilter.append("SAFE_FREE");
    nfilter.append("SAFE_RELEASE");
    nfilter.append("ENCRYPT");
    nfilter.append("ENCRYPT_HEAD");
    nfilter.append("clude");
    nfilter.append("DEFINE_JSQY_COMMAND_BEGIN");
    nfilter.append("MAX");
    nfilter.append("MIN");
    nfilter.append("_bstrncpy");    
    nfilter.append("lse");
    nfilter.append("WriteBitToByte");
    nfilter.append("ReadBitFromByte");
    nfilter.append("WriteBitToBuf");
    nfilter.append("ReadBitFromBuf");
    nfilter.append("WriteBitToInt");
    nfilter.append("ReadBitFromInt");
    nfilter.append("WriteBitToWord");
    nfilter.append("ReadBitFromWord");
    
    if not match.group("cvalue").strip() in vfilter and not match.group("cname").strip().startswith("ISSET") and not match.group("cname") in nfilter:
        return True;
    return False;
    
def ConvertProtocol():
    protocols=[];    
    
    protocols.append(r"base/const.h");
    #protocols.append(r"base/protocol.h");
    protocols.append(r"protocol/PublicData.h");
    protocols.append(r"protocol/GateProtocol.h");
    protocols.append(r"protocol/CardProtocol.h");
    protocols.append(r"protocol/WGProtocol.h");
    protocols.append(r"protocol/GHProtocol.h");
    protocols.append(r"protocol/WG_Brick_Protocol.h");
    protocols.append(r"protocol/WG_Craft_Protocol.h");
    protocols.append(r"protocol/WG_FastFood_Protocol.h");
    protocols.append(r"protocol/WG_GodWar_Protocol.h");
    protocols.append(r"protocol/WG_Guess_Protocol.h");
    protocols.append(r"protocol/WG_Pairs_Protocol.h");
    protocols.append(r"protocol/WG_SeaBattle_Protocol.h");
    protocols.append(r"protocol/WG_Snake_Protocol.h");
    protocols.append(r"protocol/WG_TalkToTalk_Protocol.h");
    protocols.append(r"protocol/WG_TD_Protocol.h");
    protocols.append(r"protocol/WG_UFO_Protocol.h");    
    
    protocnt=[];
    protocnt.append("--tool generate, do not modify by you own except you know what to do");
    protocnt.append("--tool author cclin 2015.08.26");
    protocnt.append("");
    protocnt.append("module(...,package.seeall);");
    
    protodoc=[];
    modulename="skydream_protocol";
    protodoc.append("module(...,package.seeall);");
    protodoc.append("");
    protodoc.append("--- @module "+modulename+"");
    protodoc.append("--- @field [parent=#global] #"+modulename+" "+modulename+"");
    protodoc.append("");
    
    protoerror=[];    
    bprotocnt=[];
    enumerr=[];
    protocoltypes=[];
    protocoltypes_cnt=[];
    
    allenums=[];
    allconst=[];
    publicfile=[];
    for line in protocols:
        publicfile.append(line);
    publicfile.append(r"base/BaseType.h");
    publicfile.append(r"base/Type.h");
    publicfile.append(r"protocol/protocol.h");
    #publicfile.append(r"protocol/PublicData.h");
    publicfile.append(r"protocol/Notify.h");
    publicfile.append(r"protocol/TDDBC_Struct.h");
    
    #xml process
    xmlfile=getCurrentPath()+r"xml/skydream_protocol.xml";
    protocol_dom=minidom.parse(xmlfile);
    
    #types
    newTypes=False;
    types=protocol_dom.getElementsByTagName("Types")    
    if len(types)==0:
        types=protocol_dom.createElement("Types");
        protocol_dom.documentElement.appendChild(types);
        newTypes=True;
    
    #types;
    nodes=protocol_dom.getElementsByTagName("PropTypes")
    for node in nodes:
        protocol_dom.documentElement.removeChild(node);
    proptypes=protocol_dom.createElement("PropTypes");
    protocol_dom.documentElement.appendChild(proptypes);
    
    #defines;
    nodes=protocol_dom.getElementsByTagName("Defines")
    for node in nodes:
        protocol_dom.documentElement.removeChild(node);
    defines=protocol_dom.createElement("Defines");
    protocol_dom.documentElement.appendChild(defines);
    
    #structs;
    nodes=protocol_dom.getElementsByTagName("Structs")    
    for node in nodes:
        protocol_dom.documentElement.removeChild(node);
    xml_structs=protocol_dom.createElement("Structs");
    protocol_dom.documentElement.appendChild(xml_structs);    
    
    #commands;
    nodes=protocol_dom.getElementsByTagName("Commands")    
    for node in nodes:
        protocol_dom.documentElement.removeChild(node);
    commands=protocol_dom.createElement("Commands");
    protocol_dom.documentElement.appendChild(commands);
    #xml process end
    
    #const search & convert
    for fconst in publicfile:
        cntf=open(getCurrentPath()+fconst,"r");
        for line_cnt in cntf:
            if line_cnt.strip()=="":
                continue;
            cname=ConstMatch(line_cnt);
            if cname!="":
                regex=getRegexPattern(cname);
                pattern=re.compile(regex);
                match = pattern.match(line_cnt);
                if isConsts(match):
                    const_str=match.group("cname").strip()+"="+match.group("cvalue").strip();
                    #if match.group("cmemo").strip()!="":
                    const_str = const_str+"\t--"+match.group("cmemo").strip();
                    const_str = const_str.replace("///<","--");
                    allconst.append(const_str);
        cntf.close();
    
    txtfile=getCurrentPath()+r"lua/"+modulename+"_consts.lua";
    deleteFile(txtfile);
    if len(allconst)>0:
        nwriteArrayToFile(allconst,txtfile);
    
    nallconst=[];
    for line_cnt in allconst:
        regex=getRegexPattern("lua_const");
        pattern=re.compile(regex);
        match = pattern.match(line_cnt);
        if match:
            const_val=SearchConsts(match.group("cname").strip(),allconst);
            const_str=match.group("cname").strip()+"="+str(const_val);
            #if match.group("cmemo").strip()!="":
            const_memo="\t--"+myhex(const_val)+"\t--"+match.group("cmemo").strip();
            const_str = const_str+const_memo;
            nallconst.append(const_str);
            '''
            #xmlwriter #all const
            define=protocol_dom.createElement("Define");
            define.setAttribute("name",match.group("cname").strip());
            define.setAttribute("value",str(const_val));
            if const_memo!="":
                #print const_memo;                
                ffmemo=const_memo.replace("<","");
                ffmemo=ffmemo.replace("&","");
                enumerr.append(ffmemo+"\n");
                ffmemo=ffmemo.replace("\t","");
                define.setAttribute("zcomment_cjoy_",ffmemo.strip());
            defines.appendChild(define);
            '''
    txtfile=getCurrentPath()+r"lua/"+modulename+"_consts_n.lua";
    deleteFile(txtfile);
    if len(nallconst)>0:
        nwriteArrayToFile(nallconst,txtfile);        
        
    #enum search & convert
    estack=Stack(50);
    enumcnt=[];
    enumdoc=[];    
    cmdindex=0;
    for fconst in publicfile:
        cntf=open(getCurrentPath()+fconst,"r");
        lcount=0;
        for line_cnt in cntf:
            if line_cnt.strip()=="" or line_cnt.strip().startswith("/*") or line_cnt.strip().startswith("*") or line_cnt.strip().startswith("//") or line_cnt.endswith("\\\n"):
                continue;
            name,match=EnumMatch(line_cnt);
            lcount=lcount+1;
            if name=="enum_start":
                struct=Struct();
                struct.stype="enum";
                struct.sname=match.group("ename");
                estack.push(struct);
                cmdindex=0;
                enumcnt.append("-------"+struct.sname+"-----BEGIN\n");
            elif name=="enum_field":
                if match.group("cmdeq").strip()=="" and match.group("cmdid").strip()!="":
                    continue;
                if not estack.isempty():
                    curr_enum=estack.top();
                    field=Field();
                    if match.group("cmdid")!="":
                        if ishexstr(match.group("cmdid")):
                            cmdid=int(match.group("cmdid"),16);
                            cmdindex=cmdid;
                            cmdid_str=str(cmdid);
                        else:
                            #print match.group("cmdid");
                            try:
                                cmdid=int(match.group("cmdid"),10);
                            except:
                                cmdid=SearchConsts(match.group("cmdid"),allconst)
                                #print line_cnt+","+match.group("cmdid")+"="+str(cmdid);
                            cmdindex=cmdid;
                    else:
                        cmdindex=cmdindex+1;
                    field.fname=match.group("cmdstr");
                    field.fvalue=cmdindex;
                    cmdid_str=str(cmdindex);
                    ffmemo="\t--"+cmdid_str+"\t--"+match.group("cmdmemo").strip();
                    field.fmemo=ffmemo;
                    curr_enum.sfields.append(field);
                    if match.group("cmdstr").strip()!="":
                        allconst.append(match.group("cmdstr")+"\t=\t"+myhex(cmdindex)+"\t--");
                    enumcnt.append(match.group("cmdstr")+"\t=\t"+myhex(cmdindex)+ffmemo+"\n");
                    enumdoc.append("-- @field [parent=#"+modulename+"] #"+match.group("cmdstr")+" "+match.group("cmdstr")+ffmemo+"\n");
                    
                    #xml writer
                    if curr_enum.sname=="JSQYMSG":
                        command=protocol_dom.createElement("Command");
                        command.setAttribute("name",match.group("cmdstr"));
                        command.setAttribute("aid_cjoy_",myhex(cmdindex));
                        command.setAttribute("type",QryCommandType(match.group("cmdstr"),xml_structs));#TODO:search struct name
                        if ffmemo!="":
                            #print ffmemo;
                            ffmemo=ffmemo.replace("<","");
                            ffmemo=ffmemo.replace("&","");
                            #enumerr.append(ffmemo+"\n");
                            ffmemo=ffmemo.replace("\t","");
                            command.setAttribute("zcomment_cjoy_",ffmemo.strip());
                        commands.appendChild(command);
                    #lua writer
                   
                #else:
                    #errtxt="<"+fconst+"><"+str(lcount)+">"+match.group().strip();
                    #enumerr.append(errtxt+"\n");
                    
            elif name=="struct_end":
                if not estack.isempty():
                    #TODO:save parse
                    estack.pop();
                    enumcnt.append("-------"+struct.sname+"-----END\n");
                    if not struct.sname in allenums:
                        allenums.append(struct.sname);
        cntf.close();
    
    txtfile=getCurrentPath()+r"lua/"+modulename+"_enums.lua";
    deleteFile(txtfile);
    if len(enumcnt)>0:
        writeArrayToFile(enumcnt,txtfile);
        
    txtfile=getCurrentPath()+r"lua/"+modulename+"_enums_list.lua";
    deleteFile(txtfile);
    if len(allenums)>0:
        nwriteArrayToFile(allenums,txtfile);
    
    '''
    txtfile=getCurrentPath()+r"lua/"+modulename+"_enums_error.lua";
    deleteFile(txtfile);
    if len(enumerr)>0:
        writeArrayToFile(enumerr,txtfile);
    '''
    
    txtfile=getCurrentPath()+r"lua/"+modulename+"_consts_all.lua";
    deleteFile(txtfile);
    if len(allconst)>0:
        nwriteArrayToFile(allconst,txtfile);
    
    #struct convert
    pstack=Stack(5);
    cmdstack=Stack(5);
    convert_list=[];
    for proto_file in protocols:
        cntf=open(getCurrentPath()+proto_file,"r");
        convert=ConvertFile();
        convert_list.append(convert);
        lcount=0;
        for line_cnt in cntf:
            #TODOLIST
            lcount=lcount+1;
            if line_cnt.strip()=="" or line_cnt.startswith("/*") or line_cnt.startswith(" *") or line_cnt.startswith("//") or line_cnt.endswith("\\\n") or line_cnt.strip().startswith("enum_"):
                continue;
            if line_cnt.strip().startswith("return"):
                continue;
            name=StructMatch(line_cnt);
            if name!="":
                regex=getRegexPattern(name);
                pattern=re.compile(regex);
                match = pattern.match(line_cnt);
            struct=Struct();   
            if pstack.count()>=4:
                print "pstack";
                pstack.list();
            if cmdstack.count()>=4:
                print "cmdstack";
                cmdstack.list();
            if name=="m_struct_start":
                struct.stype="m_struct";
                struct.sname=match.group("struct");
                struct.scmd=match.group("cmdstr");
                struct.sparent="JSQY_MsgHead";
                struct.sfile=proto_file;
                if pstack.isempty():
                    convert.structs.append(struct);
                pstack.push(struct);
            elif name=="struct_start":
                struct.stype="struct";
                struct.sname=match.group("struct");
                struct.sparent=match.group("parent");
                struct.sfile=proto_file;
                if pstack.isempty():
                    convert.structs.append(struct);
                pstack.push(struct);
            elif name=="b_struct_start":
                struct.stype="b_struct";
                struct.sname=match.group("struct");
                struct.sfile=proto_file;
                if pstack.isempty():
                    convert.structs.append(struct);
                pstack.push(struct);
                #errtxt="<"+proto_file+">"+struct.sname+";line="+str(lcount)+";b_struct";
                #protoerror.append(errtxt+"\n");
            elif name=="union_start":
                struct.stype="union";
                struct.sname=match.group("union");
                struct.sfile=proto_file;
                if pstack.isempty():
                    convert.structs.append(struct);
                pstack.push(struct);
            elif name=="enum_start":
                struct.stype="enum";
                struct.sfile=proto_file;
                struct.sname=match.group("ename");
                if pstack.isempty():
                    convert.structs.append(struct);
                pstack.push(struct);
            elif name=="bracket_start":
                if not cmdstack.isempty():
                    struct.stype="bracket";
                    struct.sname="bracket";
                    struct.sfile=proto_file;
                    pstack.push(struct);
                    cmdstack.pop();
            elif name=="struct_end" or name=="m_struct_end" or name=="bracket_end":
                if not pstack.isempty():
                    top_type=pstack.top();
                    if top_type.stype=="enum":
                        enumtxt="<"+proto_file+">"+top_type.sname+";line="+str(lcount)+";";
                        #enumlist.append(enumtxt+"\n");
                    else:
                        if top_type.sname!="bracket":
                            #structs
                            #cntxt=top_type.stype+","+top_type.sname+","+top_type.scmd+","+top_type.sparent;
                            #protocnt.append(cntxt);
                            #pstack.list();
                            if top_type.stype=="b_struct" or top_type.stype=="struct" or top_type.stype=="m_struct":
                                #xmlwriter
                                xml_struct=protocol_dom.createElement("Struct");
                                xml_struct.setAttribute("name",top_type.sname); 
                                xml_struct.setAttribute("ncmd_cjoy_",top_type.scmd);
                                #xml_struct.setAttribute("parent",top_type.sparent);
                                xml_structs.appendChild(xml_struct);
                            #luawriter
                            #protocnt.append(top_type.sname+"={");
                                
                            #fields
                            for field in top_type.sfields:
                                fieldtxt=field.ftype+","+field.fname+","+str(field.fcount)#+","+field.fmemo;
                                #protocnt.append(fieldtxt);
                                if (top_type.stype=="b_struct" or top_type.stype=="struct" or top_type.stype=="m_struct") and field.fname.strip()!="":
                                    #xmlwriter
                                    xml_field=protocol_dom.createElement("Item");
                                    xml_field.setAttribute("name",field.fname);
                                    convert_type=getConvertType(field.ftype);
                                    if convert_type[1]!=-1:
                                        xml_field.setAttribute("type",convert_type[0]);
                                    else:
                                        xml_field.setAttribute("type",field.ftype);
                                    
                                    #lua writer
                                    '''
                                    one_type_len=convert_type[1];
                                    if field.ftype in allenums:
                                        one_type_len=4;
                                    field_len=one_type_len;
                                    if field.isarray:
                                        field_len=one_type_len*SearchConsts(field.fcount,allconst);
                                    lua_field=field.fname+"={'"+convert_type[0]+"',"+str(field_len)+"}";
                                    protocnt.append(lua_field);
                                    '''
                                    
                                    jtype=field.ftype;
                                    if field.isarray:
                                        jtype=field.ftype+"["+field.fcount+"]";
                                    if not jtype in protocoltypes:
                                        protocoltypes.append(jtype);
                                        protocoltypes_cnt.append(jtype+"="+top_type.sname);
                                        if newTypes and isProtocolBaseType(field.ftype):
                                            type_field=protocol_dom.createElement("type");
                                            type_field.setAttribute("oname",jtype);
                                            type_field.setAttribute("zname",jtype);
                                            types.appendChild(type_field);
                                        ptype_field=protocol_dom.createElement("type");
                                        ptype_field.setAttribute("oname",jtype);
                                        ptype_field.setAttribute("zname",jtype);
                                        if field.ftype in allenums:
                                            ptype_field.setAttribute("ztype","enum");
                                        elif isProtocolBaseType(field.ftype):
                                            ptype_field.setAttribute("ztype","base");
                                        else:
                                            ptype_field.setAttribute("ztype","struct");
                                        proptypes.appendChild(ptype_field);
                                    if field.isarray:
                                        xml_field.setAttribute("ymaxlen_cjoy_",str(field.fcount));
                                        xml_field.setAttribute("yrefer_cjoy_","");
                                    ffmemo=field.fmemo;
                                    if ffmemo!="":
                                        #print ffmemo;
                                        ffmemo=field.fmemo.replace("<","");
                                        ffmemo=ffmemo.replace("&","");
                                        ffmemo=ffmemo.replace("\t","");
                                        #enumerr.append(ffmemo+"\n");
                                        xml_field.setAttribute("zcomment_cjoy_",ffmemo.strip());
                                    xml_struct.appendChild(xml_field);
                                    
                            if top_type.stype=="b_struct":
                                b_cntxt="<"+proto_file+">"+top_type.sname+";line="+str(lcount);
                                bprotocnt.append(b_cntxt+"\n");
                                
                            #protocnt.append("}");
                            
                        if top_type.scmd=="":
                            errtxt="<"+proto_file+">"+top_type.sname+";line="+str(lcount)+";scmd is nil";
                            protoerror.append(errtxt+"\n");
                        if top_type.sparent=="":
                            errtxt="<"+proto_file+">"+top_type.sname+";line="+str(lcount)+";sparent is nil";
                            protoerror.append(errtxt+"\n");
                    pstack.pop();
                else:
                    errtxt="<"+proto_file+"> line="+str(lcount)+";ended not match";
                    protoerror.append(errtxt+"\n");
                    #print errtxt;
            elif name=="global_const":
                regex=getRegexPattern("global_const");
                pattern=re.compile(regex);
                fmatch = pattern.match(line_cnt);
                if match:
                    field=Field();
                    field.itype="const";
                    field.ftype=fmatch.group("ctype");
                    field.fname=fmatch.group("cname");
                    field.fvalue=fmatch.group("cvalue");
                    field.fmemo=fmatch.group("cmemo");
                    convert.defines.append(field);
                    #allconst.append(field.fname+"="+field.fvalue+"\n");
            elif name=="global_define":
                regex=getRegexPattern("global_define");
                pattern=re.compile(regex);
                fmatch = pattern.match(line_cnt);
                if match:
                    field=Field();
                    field.itype="define";
                    field.fname=fmatch.group("cname");
                    field.fvalue=fmatch.group("cvalue");
                    field.fmemo=fmatch.group("cmemo");
                    convert.defines.append(field);
                    #allconst.append(field.fname+"="+field.fvalue+"\n");
            else:
                if pstack.isempty():
                    continue;
                cur_type=pstack.top();
                #print("top="+str(cur_type));
                fname=FieldMatch(line_cnt);
                if fname!="":
                    regex=getRegexPattern(fname);
                    pattern=re.compile(regex);
                    fmatch = pattern.match(line_cnt);
                    #print("field="+fname);
                if fname=="struct_cmd":
                    cur_type.scmd=fmatch.group("cmdstr");
                    continue;
                if fname=="commandid_cmd":
                    cur_type.scmd=fmatch.group("cmdstr");
                    continue;
                    #cmdlist.append(fmatch.group("cmdstr"));
                    #print cur_type.scmd;
                if fname=="worldgame_cmd" or fname=="worldgame_cmd2":
                    if fmatch.group("cmd1str")!="":
                        cur_type.wgcmd1=fmatch.group("cmd1str");
                    cur_type.wgcmd2=fmatch.group("cmd2str");
                    if cur_type.scmd=="":
                        cur_type.scmd="JSQY_USR_WORDGAME";
                    #print cur_type.scmd+"="+fname;
                    continue;
                if cur_type.stype=="m_struct":
                    #字段
                    field=Field();
                    if fname=="struct_field":
                        field.itype="member";
                        field.ftype=fmatch.group("ftype");
                        field.fname=fmatch.group("fname");
                        field.fmemo=fmatch.group("fmemo");
                        field.fblen=fmatch.group("flen");
                        if field.fname.strip()!="" or field.fname.strip()!="val":
                            cur_type.sfields.append(field);
                    elif fname=="struct_array_field":
                        field.itype="member";
                        field.ftype=fmatch.group("ftype");
                        field.fname=fmatch.group("fname");
                        field.fmemo=fmatch.group("fmemo");
                        field.isarray=True;
                        field.fcount=fmatch.group("fcount");
                        if field.fname.strip()!="" or field.fname.strip()!="val":
                            cur_type.sfields.append(field);
                    elif fname=="struct_struct_field":
                        field.itype="member";
                        field.ftype=fmatch.group("ftype");
                        field.fname=fmatch.group("fname");
                        field.fmemo=fmatch.group("fmemo");
                        field.isstruct=True;
                        if field.fname.strip()!="" or field.fname.strip()!="val":
                            cur_type.sfields.append(field);
                    #else:
                        #print ".",
                        #print("unknown field="+fname);
                elif cur_type.stype=="struct":
                    #字段
                    #命令字
                    field=Field();
                    if fname=="struct_field":
                        field.itype="member";
                        field.ftype=fmatch.group("ftype");
                        field.fname=fmatch.group("fname");
                        field.fmemo=fmatch.group("fmemo");
                        field.fblen=fmatch.group("flen");
                        if field.fname.strip()!="" or field.fname.strip()!="val":
                            cur_type.sfields.append(field);
                    elif fname=="struct_array_field":
                        field.itype="member";
                        field.ftype=fmatch.group("ftype");
                        field.fname=fmatch.group("fname");
                        field.fmemo=fmatch.group("fmemo");
                        field.isarray=True;
                        field.fcount=fmatch.group("fcount");
                        if field.fname.strip()!="" or field.fname.strip()!="val":
                            cur_type.sfields.append(field);
                    elif fname=="struct_struct_field":
                        field.itype="member";
                        field.ftype=fmatch.group("ftype");
                        field.fname=fmatch.group("fname");
                        field.fmemo=fmatch.group("fmemo");  
                        field.isstruct=True;
                        if field.fname.strip()!="" or field.fname.strip()!="val":
                            cur_type.sfields.append(field);
                    #else:
                        #print ".",
                        #print("unknown field="+fname);
                elif cur_type.stype=="b_struct":
                    field=Field();
                    if fname=="struct_field":
                        field.itype="member";
                        field.ftype=fmatch.group("ftype");
                        field.fname=fmatch.group("fname");
                        field.fmemo=fmatch.group("fmemo");
                        field.fblen=fmatch.group("flen");
                        if field.fname.strip()!="" or field.fname.strip()!="val":
                            cur_type.sfields.append(field);
                    elif fname=="struct_array_field":
                        field.itype="member";
                        field.ftype=fmatch.group("ftype");
                        field.fname=fmatch.group("fname");
                        field.fmemo=fmatch.group("fmemo");
                        field.isarray=True;
                        field.fcount=fmatch.group("fcount");
                        if field.fname.strip()!="" or field.fname.strip()!="val":
                            cur_type.sfields.append(field);
                    elif fname=="struct_struct_field":
                        field.itype="member";
                        field.ftype=fmatch.group("ftype");
                        field.fname=fmatch.group("fname");
                        field.fmemo=fmatch.group("fmemo");
                        field.isstruct=True;
                        if field.fname.strip()!="" or field.fname.strip()!="val":
                            cur_type.sfields.append(field);
                    #else:
                        #print ".",
                        #print("unknown field="+fname);
                elif cur_type.stype=="enum":
                    field=Field();
                    rstr=getRegexPattern("enum_field");
                    pattern=re.compile(rstr);
                    match = pattern.match(line_cnt);
                    if match:
                        isHex=True;
                        cmdindex=0;
                        if match.group("cmdid")!="":
                            if ishexstr(match.group("cmdid")):
                                cmdid=int(match.group("cmdid"),16);
                                cmdindex=cmdid;
                                isHex=True;
                                cmdid_str=str(cmdid);
                            else:
                                try:
                                    cmdid=int(match.group("cmdid"),10);
                                except:
                                    cmdid=SearchConsts(match.group("cmdid").strip(),allconst);
                                    #print match.group("cmdid")+"="+str(cmdid);
                                cmdindex=cmdid;
                                isHex=False;
                        else:
                            cmdindex=cmdindex+1;
                        if isHex:
                            cmdid_str=str(cmdindex);
                        else:
                            cmdid_str=myhex(cmdindex);
                        field.ftype="int";
                        field.fname=match.group("cmdstr")+"="+myhex(cmdindex);
                        #allconst.append(field.fname+"\t--");
                        field.fmemo="--"+match.group("cmdmemo")+"--"+str(cmdindex)+"\n";
                        cur_type.sfields.append(field);
                    #print("unprocess enum="+cur_type.stype);
                #else:
                    #print ".",
                    #print("unknown type="+cur_type.stype);
        cntf.close();
        
    #lua writer
    protocnt.append("");
    allfieldlen=[]
    protocnt.append("--[index]={name,type,index,flag(0:base,1:enum,2:struct,20:base array,21:enum array,22:struct array),len,total_len=len*arraycount,defval}");
    protocnt.append("");
    
    define_str="Define";
    #protocol header writer
    define_tab="JSQY_MsgHead_"+define_str;
    protocnt.append(define_tab+"={");
    protocnt.append("[1]={'m_uTotalLength','INT16',0,2,2},");
    protocnt.append("[2]={'m_uCommandID','INT16',2,0,2,2},");
    protocnt.append("[3]={'m_uJnls','INT32',3,0,4,4},");
    protocnt.append("[4]={'m_uUserID','INT32',4,0,4,4}");
    protocnt.append("}");
    
    protocnt.append("JSQY_MsgHead={");
    protocnt.append("define="+define_tab+",");
    protocnt.append("m_uTotalLength=0,");
    protocnt.append("m_uCommandID=0,");
    protocnt.append("m_uJnls=0,");
    protocnt.append("m_uUserID=0");
    protocnt.append("}");
    
    define_tab="MessageHead_"+define_str;
    protocnt.append(define_tab+"={");
    protocnt.append("[1]={'m_uTotalLength','INT16',0,2,2},");
    protocnt.append("[2]={'m_uCommandID','INT16',2,0,2,2},");
    protocnt.append("[3]={'m_uJnls','INT32',3,0,4,4},");
    protocnt.append("[4]={'m_uUserID','INT32',4,0,4,4}");
    protocnt.append("}");
    
    protocnt.append("MessageHead={");
    protocnt.append("define="+define_tab+",");
    protocnt.append("m_uTotalLength=0,");
    protocnt.append("m_uCommandID=0,");
    protocnt.append("m_uJnls=0,");
    protocnt.append("m_uUserID=0");
    protocnt.append("}");
    
    for convert in convert_list:
        for istruct in convert.structs:
            if istruct.stype=="b_struct" or istruct.stype=="struct" or istruct.stype=="m_struct":
                define_tab=istruct.sname+"_"+define_str;
                protocnt.append(define_tab+"={");
                findex=0;
                for field in istruct.sfields:
                    if field.fname.strip()=="" or field.fname.strip()=="val":
                        continue;
                    findex=findex+1;
                    convert_type=getConvertType(field.ftype);
                    def_value=str(convert_type[2]);
                    one_type_len=convert_type[1];
                    field_type_flag=0;
                    if field.ftype in allenums:
                        one_type_len=4;
                        field_type_flag=1;
                        def_value=0;
                    elif one_type_len==-1:
                        one_type_len=CalcStructLen(field.ftype,convert_list,allenums,allconst);
                        field_type_flag=2;
                    field_len=one_type_len;
                    myftype=convert_type[0];
                    if field.isarray:
                        allfieldlen.append(istruct.sname+","+field.fname+","+field.fcount);
                        isNum,base=isIntS(field.fcount);
                        if isNum:
                            field_len=one_type_len*int(field.fcount,base);
                        else:
                            if field.fcount.find("[")!=-1:
                                field_len=one_type_len*CalcArrayLen(field.fcount,allconst);
                                #print field.fcount;
                            else:
                                field_len=one_type_len*SearchConsts(field.fcount,allconst);
                        field_type_flag=field_type_flag+20;
                        if convert_type[0]=="INT8" or convert_type[0]=="UINT8":
                            def_value="''";
                        else:
                            def_value="{}";
                    #lua_field=field.fname+"={'"+myftype+"',"+str(findex)+","+str(field_type_flag)+","+str(one_type_len)+","+str(field_len)+"}";
                    lua_field_desc="["+str(findex)+"]"+"={'"+field.fname+"','"+myftype+"',"+str(field_type_flag)+","+str(one_type_len)+","+str(field_len)+","+str(def_value)+"}";
                    if findex<len(istruct.sfields):
                        lua_field_desc=lua_field_desc+",";
                    protocnt.append(lua_field_desc);
                protocnt.append("}");
                
                protocnt.append(istruct.sname+"={");
                protocnt.append("define="+define_tab+",");
                if istruct.scmd!="":
                    cmdid_str="cmd='"+istruct.scmd+"'";
                    if len(istruct.sfields)>0:
                        cmdid_str=cmdid_str+",";
                    protocnt.append(cmdid_str);
                findex=0;
                for field in istruct.sfields:
                    if field.fname.strip()=="" or field.fname.strip()=="val":
                        continue;
                    findex=findex+1;
                    convert_type=getConvertType(field.ftype);
                    def_value=str(convert_type[2]);
                    if field.isarray and (convert_type[1]=="INT8" or convert_type[1]=="UINT8"):
                        def_value="''";
                    elif field.isarray:
                        def_value="{}";
                    lua_field=field.fname+"="+def_value;
                    if findex<len(istruct.sfields):
                        lua_field=lua_field+",";
                    protocnt.append(lua_field);
                protocnt.append("}");
            elif istruct.stype=="enum":
                print ".",
            elif istruct.stype=="union":
                print ".",
                
    txtfile=getCurrentPath()+r"lua/"+modulename+".lua";
    deleteFile(txtfile);
    if len(protocnt)>0:
        nwriteArrayToFile(protocnt,txtfile);

    txtfile=getCurrentPath()+r"lua/"+modulename+"_field_lens.lua";
    deleteFile(txtfile);
    if len(allfieldlen)>0:
        nwriteArrayToFile(allfieldlen,txtfile);
    
    txtfile=getCurrentPath()+r"lua/"+modulename+"_doc.lua";
    deleteFile(txtfile);
    if len(protodoc)>0:
        nwriteArrayToFile(protodoc,txtfile);
        
    txtfile=getCurrentPath()+r"lua/"+modulename+"_types.lua";
    deleteFile(txtfile);
    if len(protocoltypes_cnt)>0:
        nwriteArrayToFile(protocoltypes_cnt,txtfile);

    txtfile=getCurrentPath()+r"lua/"+modulename+"_base.lua";
    deleteFile(txtfile);
    if len(bprotocnt)>0:
        writeArrayToFile(bprotocnt,txtfile);
    
    txtfile=getCurrentPath()+r"lua/"+modulename+"_error.lua";
    deleteFile(txtfile);
    if len(protoerror)>0:
        writeArrayToFile(protoerror,txtfile);
    
    #used const save to xml
    unusedconsts=[];
    for line_cnt in allconst:
        regex=getRegexPattern("lua_const");
        pattern=re.compile(regex);
        match = pattern.match(line_cnt);
        if match:
            #print match.group("cname").strip();
            if IsConstUsed(match.group("cname").strip(),xml_structs):                
                #xmlwriter #used consts
                const_val=SearchConsts(match.group("cname").strip(),allconst);
                const_str=match.group("cname").strip()+"="+str(const_val);
                #if match.group("cmemo").strip()!="":
                const_memo="\t--"+myhex(const_val)+"\t--"+match.group("cmemo").strip();
                const_str = const_str+const_memo;
                
                define=protocol_dom.createElement("Define");
                define.setAttribute("name",match.group("cname").strip());
                define.setAttribute("value",str(const_val));
                if const_memo!="":
                    #print const_memo;
                    ffmemo=const_memo.replace("<","");
                    ffmemo=ffmemo.replace("&","");
                    #enumerr.append(ffmemo+"\n");
                    ffmemo=ffmemo.replace("\t","");
                    define.setAttribute("zcomment_cjoy_",ffmemo.strip());
                defines.appendChild(define);
                #lua writer
            else:
                unusedconsts.append(line_cnt);
            
    txtfile=getCurrentPath()+r"lua/"+modulename+"_consts_n.lua";
    deleteFile(txtfile);
    if len(nallconst)>0:
        nwriteArrayToFile(nallconst,txtfile);
    
    prettyxml=protocol_dom.toprettyxml()
    f=open(xmlfile,"w")
    f.write(prettyxml)  
    f.close()
    
    xmlf=open(xmlfile,"r");
    xmlprotocol=[];
    for line_cnt in xmlf:
        xmlprotocol.append(ReplaceStr(line_cnt));
    xmlf.close();    
    writeArrayToFile(xmlprotocol,xmlfile);
    
    
    #writeArrayToFile(protocnt,getCurrentPath()+r"lua/"+modulename+".lua");;
    #appendArrayToFile(protodoc,getCurrentPath()+r"lua/"+modulename+".lua");;   

def CalcArrayLen(acount,list):
    arrlen="["+acount+"]";
    rstr=getRegexPattern("array_len");
    pattern=re.compile(rstr);
    match = pattern.match(arrlen);
    num=0;
    while match:
        num=num+SearchConsts(match.group("array_len"),list);
        arrlen=arrlen.replace(match.group(),"");
        match=pattern.match(arrlen);
    #print "multi array len="+str(num);
    return num;
    
def CalcStructLen(sname,convertlist,allenums,allconsts):
    struct_len=0;
    for convert in convertlist:
        for struct in convert.structs:
            if sname==struct.sname:
                for field in struct.sfields:
                    ctype=getConvertType(field.ftype);
                    one_len=0;
                    if ctype[1]==-1:
                        if field.ftype in allenums:
                            one_len=one_len+4;
                        else:
                            one_len=one_len+CalcStructLen(field.ftype,convertlist,allenums,allconsts);
                    else:
                        one_len=one_len+ctype[1];
                    field_len=one_len;
                    if field.isarray:
                        field_len=one_len*SearchConsts(field.fcount,allconsts);
                    struct_len=struct_len+field_len;
    if struct_len==0:
        if sname=="S_DyeColor":
            struct_len=6;#publicdata 739
        elif sname=="U_CellData":
            struct_len=30;#publicdata 3210
        elif sname=="InitPlayerStruct":
            struct_len=0;#TODO
        elif sname=="S_BattleCardDetail":
            struct_len=0;#TODO
        elif sname=="S_KnowCardDetail":
            struct_len=0;#TODO
        elif sname=="CardActionStruct":
            struct_len=0;#TODO
        elif sname=="U_VipItem":
            struct_len=2;
        elif sname=="U_PlayerState":
            struct_len=1;
        elif sname=="U_UNIONAFFAIR":
            struct_len=4;
        elif sname=="U_MEMBER_NUM":
            struct_len=4;
        elif sname=="S_FallGold":
            struct_len=8;
        elif sname=="S_TaskItem":
            struct_len=14;
        elif sname=="S_MonsterDrop":
            struct_len=8;
        elif sname=="S_StolenItem":
            struct_len=12;
        elif sname=="S_MonsterSkill":
            struct_len=6;
        elif sname=="LPSTR":
            struct_len=0;
        elif sname=="u_ItemInfo":
            struct_len=10;
        elif sname=="U_CompeteData":
            struct_len=4;
    return struct_len;
    
def ReplaceStr(line_cnt):
    replist={};
    replist["zcomment_cjoy_"]="comment";
    replist["ymaxlen_cjoy_"]="maxlen";
    replist["yrefer_cjoy_"]="refer";
    replist["aid_cjoy_"]="id";
    replist["ncmd_cjoy_"]="cmd";
    for (key,value) in replist.items():
        line_cnt=line_cnt.replace(key,value);
    return line_cnt;
    
def QryCommandType(command,xml_structs):    
    for xml_struct in xml_structs.childNodes:
        if xml_struct.getAttribute("ncmd_cjoy_")==command:
            return xml_struct.getAttribute("name");
    return "";
    
def IsConstUsed(const,xml_structs):
    for xml_struct in xml_structs.childNodes:
        for xml_field in xml_struct.childNodes:
            fvalue=xml_field.getAttribute("ymaxlen_cjoy_").strip();
            if fvalue=="":
                continue;
            else:
                isNum,base=isIntS(fvalue);
                if isNum:
                    continue;
            #print fvalue;
            if fvalue.strip()==const.strip():
                return True;
            else:
                mregex=getRegexPattern("arithmetic");
                pattern=re.compile(mregex);
                sub_match = pattern.match(fvalue);
                if sub_match and (sub_match.group("name1").strip()==const.strip() or sub_match.group("name2").strip()==const.strip()):
                    return True;
    return False;

def EnumMatch(content):
    matchlist=[];
    matchlist.append("struct_end");
    matchlist.append("enum_start");
    matchlist.append("enum_field");
    for line in matchlist:
        rstr=getRegexPattern(line);
        pattern=re.compile(rstr);
        match = pattern.match(content);
        if match:
            return line,match;
    return "",None;

def ConstMatch(content):
    matchlist=[];
    matchlist.append("global_const");
    matchlist.append("global_define");
    for line in matchlist:
        rstr=getRegexPattern(line);
        pattern=re.compile(rstr);
        match = pattern.match(content);
        if match:
            return line;
    return "";

def StructMatch(content):
    matchlist=[];
    matchlist.append("m_struct_start");
    matchlist.append("m_struct_end");
    matchlist.append("struct_struct_field");
    matchlist.append("struct_start");
    matchlist.append("b_struct_start");
    matchlist.append("enum_start");
    matchlist.append("union_start");
    matchlist.append("global_const");
    matchlist.append("global_define");
    matchlist.append("struct_end");
    #matchlist.append("bracket_start");
    #matchlist.append("bracket_end");
    for line in matchlist:
        rstr=getRegexPattern(line);
        pattern=re.compile(rstr);
        match = pattern.match(content);
        if match:
            return line;
    return "";
    
def FieldMatch(content):
    matchlist=[];
    matchlist.append("struct_struct_field");
    matchlist.append("struct_array_field");
    matchlist.append("struct_field");
    matchlist.append("struct_cmd");
    matchlist.append("commandid_cmd");
    matchlist.append("worldgame_cmd");
    matchlist.append("worldgame_cmd2");
    for line in matchlist:
        rstr=getRegexPattern(line);
        pattern=re.compile(rstr);
        match = pattern.match(content);
        if match:
            return line;
    return "";
    
def CalcMatch(linecontent):
    matchlist=[];
    matchlist.append("m_struct_start");
    matchlist.append("m_struct_end");
    matchlist.append("struct_start");
    matchlist.append("b_struct_start");
    matchlist.append("struct_end");
    matchlist.append("struct_cmd");
    matchlist.append("commandid_cmd");
    matchlist.append("worldgame_cmd");
    matchlist.append("worldgame_cmd2");
    matchlist.append("struct_field");
    matchlist.append("struct_array_field");
    matchlist.append("global_const");
    matchlist.append("lua_const");
    matchlist.append("arithmetic");
    matchlist.append("global_define");
    matchlist.append("enum_start");
    matchlist.append("enum_field");
    matchlist.append("union_start");
    matchlist.append("bracket_start");
    matchlist.append("bracket_end");    
    matchlist.append("enum_memo");
    result=[];
    for lmatch in matchlist:
        rstr=getRegexPattern(lmatch);
        pattern=re.compile(rstr);
        match = pattern.match(linecontent);
        if match:
            result.append(lmatch);
    return result;
            
def getRegexPattern(regex):
    reg_str="cjoyaaa*";
    if regex=="m_struct_start":
        reg_str=r"\s*DEFINE_JSQY_COMMAND_BEGIN\s*\(\s*(?P<struct>\w*)\s*,\s*(?P<cmdstr>\w*)\s*\)\s*";
    elif regex=="m_struct_end":
        reg_str=r"\s*DEFINE_COMMAND_END\s*";
    elif regex=="struct_start":
        reg_str=r"\s*struct\s*(?P<struct>\w*)\s*:\s*(?P<parent>\w*)\s*";
    elif regex=="b_struct_start":
        reg_str=r"\s*struct\s*(?P<struct>\w*)\s*\{{0,1}\s*";
    elif regex=="struct_end":
        reg_str=r"\s*\}[\s\w]*;\s*";
    elif regex=="struct_cmd":
        reg_str=r".*?:\s*MessageHead\s*\(\s*(?P<cmdstr>\w*)\s*\)\s*";
    elif regex=="commandid_cmd":
        reg_str=r"\s*m_uCommandID\s*=\s*(?P<cmdstr>\w*)\s*;\s*";
    elif regex=="worldgame_cmd":
        reg_str=r".*?\(\s*\w*\s*uRsv1={0,1}(?P<cmd1str>\w*?)\s*,\s*\w*\s*uRsv2\s*=\s*(?P<cmd2str>\w*)\s*";
    elif regex=="worldgame_cmd2":
        reg_str=".*?:*\s*S_JSQYWordGameHead\s*\(\s*(?P<cmd1str>\w*)\s*,\s*(?P<cmd2str>\w*)\s*\)\s*";
    elif regex=="struct_field":
        reg_str=r"\s*(?P<ftype>\w*)\s*(?P<fname>\w*)\s*:{0,1}(?P<flen>\d*)\s*;\s*/*(?P<fmemo>[\s\S]*?)\s*";
    elif regex=="struct_struct_field":
        reg_str=r"\s*struct\s*(?P<ftype>\w*)\s*(?P<fname>\w*)\s*\;\s*/*(?P<fmemo>[\s\S]*?)\s*";
    elif regex=="struct_array_field":
        reg_str=r"\s*(?P<ftype>\w*)\s*(?P<fname>\w*)\s*\[\s*(?P<fcount>.*?)\s*\]\s*;\s*/*(?P<fmemo>[\s\S]*)\s*";
    elif regex=="global_const":
        reg_str=r"\s*const\s*(?P<ctype>\w*)\s*(?P<cname>\w*)\s*=\s*\({0,1}\s*(?P<cvalue>[\w\s+-\\*]*)\s*\){0,1}\s*;\s*/*(?P<cmemo>[\s\S]*)\s*";
    elif regex=="lua_const":
        reg_str=r"\s*(?P<cname>\w*)\s*=\s*\({0,1}\s*(?P<cvalue>[\w\s+-]*\s*)\s*\){0,1}\s*--\s*(?P<cmemo>[\s\S]*)\s*";
    elif regex=="arithmetic":
        reg_str=r"\s*(?P<name1>\w*)\s*(?P<oper>[+-])\s*(?P<name2>\w*)\s*";
    elif regex=="global_define":
        reg_str=r"\s*#(?P<ctype>[define]*)\s*(?P<cname>\w*)\s*\({0,1}\s*(?P<cvalue>[\w\s+-\\*]*)\s*\){0,1}\s*/*\s*(?P<cmemo>[\s\S]*)\s*";
    elif regex=="enum_start":
        reg_str=r"\s*enum\s*(?P<ename>\w*)\s*";
    elif regex=="enum_field":
        reg_str="\s*(?P<cmdstr>\w*)\s*(?P<cmdeq>={0,1})\s*(?P<cmdid>[\w\s+-]*)\s*,{0,1}\s*/*(?P<cmdmemo>[\s\S]*)";
        #reg_str="\s*(?P<cmdstr>\w*)\s*(?P<cmdeq>={0,1})\s*(?P<cmdid>\w*)\s*,\s*/*(?P<cmdmemo>[\s\S]*)";
    elif regex=="enum_memo":
        reg_str=r"\s*//+(?P<cmdmemo>[\s\S]*)";
    elif regex=="union_start":
        reg_str=r"\s*union\s*(?P<union>\w*)\s*\{{0,1}\s*";
    elif regex=="bracket_start":
        reg_str=r"\s*\{\s*";
    elif regex=="bracket_end":
        reg_str=r"\s*\};*\s*";
    elif regex=="array_len":
        reg_str=r"\s*\[(?P<array_len>[\w\s+-\\*/]*)\]\s*";
    return reg_str;
    
def SearchConsts(name,list):
    iname=name.strip();
    isNum,base=isIntS(iname);    
    if isNum:
        return int(iname,base);    
    mregex=getRegexPattern("arithmetic");
    pattern=re.compile(mregex);
    sub_match = pattern.match(iname);
    #print sub_match,fmatch.group("cvalue").strip();
    if sub_match:
        if sub_match.group("oper").strip()=="+":
            return SearchConsts(sub_match.group("name1"),list)+SearchConsts(sub_match.group("name2"),list);
        elif sub_match.group("oper").strip()=="-":
            return SearchConsts(sub_match.group("name1"),list)-SearchConsts(sub_match.group("name2"),list);
        elif sub_match.group("oper").strip()=="*":
            return SearchConsts(sub_match.group("name1"),list)*SearchConsts(sub_match.group("name2"),list);
        elif sub_match.group("oper").strip()=="/":
            return SearchConsts(sub_match.group("name1"),list)/SearchConsts(sub_match.group("name2"),list);
    else:
        for line_cnt in list:
            mregex=getRegexPattern("lua_const");        
            pattern=re.compile(mregex);
            fmatch = pattern.match(line_cnt);        
            #print fmatch.group("cname");
            if fmatch and fmatch.group("cname").strip()==iname.strip():
                isNum,base=isIntS(fmatch.group("cvalue").strip());
                #print iname,isNum,base,fmatch.group("cvalue").strip();
                if isNum:                
                    return int(fmatch.group("cvalue"),base);
                else:
                    mregex=getRegexPattern("arithmetic");
                    pattern=re.compile(mregex);
                    sub_match = pattern.match(fmatch.group("cvalue").strip());
                    #print sub_match,fmatch.group("cvalue").strip();
                    if sub_match:
                        if sub_match.group("oper")=="+":
                            return SearchConsts(sub_match.group("name1"),list)+SearchConsts(sub_match.group("name2"),list);
                        elif sub_match.group("oper")=="-":
                            return SearchConsts(sub_match.group("name1"),list)-SearchConsts(sub_match.group("name2"),list);
                        elif sub_match.group("oper")=="*":
                            return SearchConsts(sub_match.group("name1"),list)*SearchConsts(sub_match.group("name2"),list);
                        elif sub_match.group("oper")=="/":
                            return SearchConsts(sub_match.group("name1"),list)/SearchConsts(sub_match.group("name2"),list);
                    else:
                        #print fmatch.group("cvalue");
                        return SearchConsts(fmatch.group("cvalue"),list);
                        
    return -1;
    
def isBaseType(name):
    types={};
    types["void"]=0;
    types["UCHAR"]=1;
    types["CHAR"]=1;
    types["UINT"]=4;
    types["INT"]=4;
    types["USHORT"]=2;
    types["SHORT"]=2;
    types["ULONG"]=4;
    types["LONG"]=4;
    types["FLOAT"]=4;
    types["uchar"]=1;
    types["ushort"]=2;
    types["uint"]=4;
    types["IP_t"]=4;
    types["PacketID_t"]=2;
    types["BOOL"]=4;
    types["WORD"]=2;
    types["DWORD"]=4;
    for (d,x) in types.items():
        if name==d:
            return True;
    return False;
    
def isProtocolBaseType(tname):
    btypes={};
    btypes["char"]=1;
    btypes["int"]=4;
    btypes["WORD"]=2;
    btypes["DWORD"]=4;
    btypes["BYTE"]=1;
    btypes["INT8"]=1;
    btypes["UINT8"]=1;
    btypes["bool"]=1;
    btypes["INT"]=4;
    btypes["short"]=2;
    btypes["float"]=4;
    btypes["ULONG"]=4;
    btypes["LONG"]=4;
    btypes["UINT"]=4;
    btypes["long"]=4;
    btypes["time_t"]=4;
    btypes["CHAR"]=1;
    btypes["WCHAR"]=2;
    for (d,x) in btypes.items():
        if tname==d:
            return True;
    return False;
    
def getNewType(tname):
    nbtypes={};
    nbtypes["INT8"]=0;
    nbtypes["UINT8"]=1;
    nbtypes["INT16"]=2;
    nbtypes["UINT16"]=3;
    nbtypes["INT32"]=4;
    nbtypes["UINT32"]=5;
    nbtypes["INT64"]=6;
    nbtypes["UINT64"]=7;
    nbtypes["FLOAT"]=8;
    nbtypes["DOUBLE"]=9;
    #nbtypes["ARRAY"]=10;#equal to vector
    #nbtypes["MAP"]=11;
    #nbtypes["ENUM"]=12;
    #nbtypes["VECTOR"]=13;
    #nbtypes["STRUCT"]=14;
    for (d,x) in btypes.items():
        if tname==d:
            return x;
    return -1;

    
def getConvertType(tname):
    btypes={};
    btypes["char"]=["INT8",1,0];
    btypes["int"]=["INT32",4,0];
    btypes["WORD"]=["UINT16",2,0];
    btypes["DWORD"]=["UINT32",4,0];
    btypes["BYTE"]=["UINT8",1,0];
    btypes["INT8"]=["INT8",1,0];
    btypes["UINT8"]=["UINT8",1,0];
    btypes["bool"]=["INT8",1,0];
    btypes["INT"]=["INT32",4],0;
    btypes["short"]=["INT16",2,0];
    btypes["float"]=["FLOAT",4,0];
    btypes["double"]=["DOUBLE",8,0];
    btypes["ULONG"]=["UINT32",4,0];
    btypes["LONG"]=["INT32",4,0];
    btypes["UINT"]=["UINT32",4,0];
    btypes["long"]=["INT32",4,0];
    btypes["time_t"]=["UINT32",4,0];
    btypes["CHAR"]=["INT8",1,0];
    btypes["UCHAR"]=["UINT8",1,0];
    btypes["WCHAR"]=["INT16",2,0];
    for (d,x) in btypes.items():
        if tname.strip().lower()==d.lower():
            return x;
    return [tname,-1,-1];
    
def isInt(str):
    if type(str) is int:
        return True;
    return False;
    
def isIntS(str):
    try:
        int(str);
        return True,10
    except ValueError:
        try:
            int(str,16);
            return True,16
        except ValueError:
            return False,0;
        return False,0;

def ApiToSnippets(file):
    apis=open(file);
    cls_str=r"\s*(?P<ns>\w*)\.(?P<cls>\w*)\s*=\s*\{\}*\s*";
    fun_str=r"\s*function\s*(?P<cls>\S*)\:(?P<fun>\S*)\s*end\s*"
    snippets=[];
    snippets.append("{\n");
    cls=[];
    names=[];
    ns=[];
    for line in apis:
        pattern=re.compile(cls_str);
        match = pattern.match(line);
        if match:
            cname=match.group('ns')+"."+match.group('cls');
            if not match.group("ns") in ns:
                ns.append(match.group("ns"));
            if not cname in cls:
                cls.append(cname);
            if not cname in names: 
                names.append(cname);
                snippets.append('"'+cname+'":{\n');
                snippets.append('"prefix":"'+cname+'",\n');
                snippets.append('"body":["'+cname+'"],\n');
                snippets.append('"description":"cocos class",\n');
                snippets.append('"scope":"source.lua"},\n\n');
        else:
            funp=re.compile(fun_str);
            funm = funp.match(line);
            if funm:
                cname=funm.group('cls')+":"+funm.group('fun');
                if not cname in names:
                    names.append(cname);
                    snippets.append('"'+cname+'":{\n');
                    snippets.append('"prefix":"'+cname+'",\n');
                    snippets.append('"body":["'+cname+'"],\n');
                    snippets.append('"description":"'+funm.group('fun')+'",\n');
                    snippets.append('"scope":"source.lua"},\n\n');
    print(snippets[len(snippets)-1][-3]);
    snippets[len(snippets)-1]=snippets[len(snippets)-1].replace(",","");
    snippets.append("}");
    snippets.append("}");
    writeArrayToFile(snippets,getCurrentPath()+"snippet.json");   
    nappendArrayToFile(ns,getCurrentPath()+"ns.json");
    nappendArrayToFile(names,getCurrentPath()+"names.json");
    nappendArrayToFile(cls,getCurrentPath()+"ncls.json");

def toLuaLuas():
    root=r"frameworks/cocos2d-x/cocos/scripting/lua-bindings/"
    apis=[];
    apis.append(root+r"auto/api/AbstractCheckButton.lua");
    apis.append(root+r"auto/api/Action.lua");
    apis.append(root+r"auto/api/ActionCamera.lua");
    apis.append(root+r"auto/api/ActionEase.lua");
    apis.append(root+r"auto/api/ActionFadeFrame.lua");
    apis.append(root+r"auto/api/ActionFloat.lua");
    apis.append(root+r"auto/api/ActionFrame.lua");
    apis.append(root+r"auto/api/ActionInstant.lua");
    apis.append(root+r"auto/api/ActionInterval.lua");
    apis.append(root+r"auto/api/ActionManager.lua");
    apis.append(root+r"auto/api/ActionManagerEx.lua");
    apis.append(root+r"auto/api/ActionMoveFrame.lua");
    apis.append(root+r"auto/api/ActionObject.lua");
    apis.append(root+r"auto/api/ActionRotationFrame.lua");
    apis.append(root+r"auto/api/ActionScaleFrame.lua");
    apis.append(root+r"auto/api/ActionTimeline.lua");
    apis.append(root+r"auto/api/ActionTimelineCache.lua");
    apis.append(root+r"auto/api/ActionTimelineData.lua");
    apis.append(root+r"auto/api/ActionTimelineNode.lua");
    apis.append(root+r"auto/api/ActionTintFrame.lua");
    apis.append(root+r"auto/api/ActionTween.lua");
    apis.append(root+r"auto/api/AlphaFrame.lua");
    apis.append(root+r"auto/api/AmbientLight.lua");
    apis.append(root+r"auto/api/AnchorPointFrame.lua");
    apis.append(root+r"auto/api/Animate.lua");
    apis.append(root+r"auto/api/Animate3D.lua");
    apis.append(root+r"auto/api/Animation.lua");
    apis.append(root+r"auto/api/Animation3D.lua");
    apis.append(root+r"auto/api/AnimationCache.lua");
    apis.append(root+r"auto/api/AnimationData.lua");
    apis.append(root+r"auto/api/AnimationFrame.lua");
    apis.append(root+r"auto/api/Application.lua");
    apis.append(root+r"auto/api/Armature.lua");
    apis.append(root+r"auto/api/ArmatureAnimation.lua");
    apis.append(root+r"auto/api/ArmatureData.lua");
    apis.append(root+r"auto/api/ArmatureDataManager.lua");
    apis.append(root+r"auto/api/ArmatureDisplayData.lua");
    apis.append(root+r"auto/api/AssetsManager.lua");
    apis.append(root+r"auto/api/AssetsManagerEx.lua");
    apis.append(root+r"auto/api/AsyncTaskPool.lua");
    apis.append(root+r"auto/api/AtlasNode.lua");
    apis.append(root+r"auto/api/AttachNode.lua");
    apis.append(root+r"auto/api/AudioEngine.lua");
    apis.append(root+r"auto/api/AudioProfile.lua");
    apis.append(root+r"auto/api/AutoPolygon.lua");
    apis.append(root+r"auto/api/BaseData.lua");
    apis.append(root+r"auto/api/BaseLight.lua");
    apis.append(root+r"auto/api/BatchNode.lua");
    apis.append(root+r"auto/api/BezierBy.lua");
    apis.append(root+r"auto/api/BezierTo.lua");
    apis.append(root+r"auto/api/BillBoard.lua");
    apis.append(root+r"auto/api/BlendFuncFrame.lua");
    apis.append(root+r"auto/api/Blink.lua");
    apis.append(root+r"auto/api/Bone.lua");
    apis.append(root+r"auto/api/BoneData.lua");
    apis.append(root+r"auto/api/BoneNode.lua");
    apis.append(root+r"auto/api/Bundle3D.lua");
    apis.append(root+r"auto/api/Button.lua");
    apis.append(root+r"auto/api/CallFunc.lua");
    apis.append(root+r"auto/api/Camera.lua");
    apis.append(root+r"auto/api/CameraBackgroundBrush.lua");
    apis.append(root+r"auto/api/CameraBackgroundColorBrush.lua");
    apis.append(root+r"auto/api/CameraBackgroundDepthBrush.lua");
    apis.append(root+r"auto/api/CameraBackgroundSkyBoxBrush.lua");
    apis.append(root+r"auto/api/CardinalSplineBy.lua");
    apis.append(root+r"auto/api/CardinalSplineTo.lua");
    apis.append(root+r"auto/api/CatmullRomBy.lua");
    apis.append(root+r"auto/api/CatmullRomTo.lua");
    apis.append(root+r"auto/api/CCBAnimationManager.lua");
    apis.append(root+r"auto/api/CCBReader.lua");
    apis.append(root+r"auto/api/CheckBox.lua");
    apis.append(root+r"auto/api/ClippingNode.lua");
    apis.append(root+r"auto/api/ClippingRectangleNode.lua");
    apis.append(root+r"auto/api/ColorFrame.lua");
    apis.append(root+r"auto/api/ComAttribute.lua");
    apis.append(root+r"auto/api/ComAudio.lua");
    apis.append(root+r"auto/api/ComController.lua");
    apis.append(root+r"auto/api/ComExtensionData.lua");
    apis.append(root+r"auto/api/Component.lua");
    apis.append(root+r"auto/api/ComponentLua.lua");
    apis.append(root+r"auto/api/ComRender.lua");
    apis.append(root+r"auto/api/Console.lua");
    apis.append(root+r"auto/api/ContourData.lua");
    apis.append(root+r"auto/api/Control.lua");
    apis.append(root+r"auto/api/ControlButton.lua");
    apis.append(root+r"auto/api/ControlColourPicker.lua");
    apis.append(root+r"auto/api/ControlHuePicker.lua");
    apis.append(root+r"auto/api/Controller.lua");
    apis.append(root+r"auto/api/ControlPotentiometer.lua");
    apis.append(root+r"auto/api/ControlSaturationBrightnessPicker.lua");
    apis.append(root+r"auto/api/ControlSlider.lua");
    apis.append(root+r"auto/api/ControlStepper.lua");
    apis.append(root+r"auto/api/ControlSwitch.lua");
    apis.append(root+r"auto/api/CSLoader.lua");
    apis.append(root+r"auto/api/DelayTime.lua");
    apis.append(root+r"auto/api/Device.lua");
    apis.append(root+r"auto/api/DirectionLight.lua");
    apis.append(root+r"auto/api/Director.lua");
    apis.append(root+r"auto/api/DisplayData.lua");
    apis.append(root+r"auto/api/DisplayManager.lua");
    apis.append(root+r"auto/api/DrawNode.lua");
    apis.append(root+r"auto/api/EaseBackIn.lua");
    apis.append(root+r"auto/api/EaseBackInOut.lua");
    apis.append(root+r"auto/api/EaseBackOut.lua");
    apis.append(root+r"auto/api/EaseBezierAction.lua");
    apis.append(root+r"auto/api/EaseBounce.lua");
    apis.append(root+r"auto/api/EaseBounceIn.lua");
    apis.append(root+r"auto/api/EaseBounceInOut.lua");
    apis.append(root+r"auto/api/EaseBounceOut.lua");
    apis.append(root+r"auto/api/EaseCircleActionIn.lua");
    apis.append(root+r"auto/api/EaseCircleActionInOut.lua");
    apis.append(root+r"auto/api/EaseCircleActionOut.lua");
    apis.append(root+r"auto/api/EaseCubicActionIn.lua");
    apis.append(root+r"auto/api/EaseCubicActionInOut.lua");
    apis.append(root+r"auto/api/EaseCubicActionOut.lua");
    apis.append(root+r"auto/api/EaseElastic.lua");
    apis.append(root+r"auto/api/EaseElasticIn.lua");
    apis.append(root+r"auto/api/EaseElasticInOut.lua");
    apis.append(root+r"auto/api/EaseElasticOut.lua");
    apis.append(root+r"auto/api/EaseExponentialIn.lua");
    apis.append(root+r"auto/api/EaseExponentialInOut.lua");
    apis.append(root+r"auto/api/EaseExponentialOut.lua");
    apis.append(root+r"auto/api/EaseIn.lua");
    apis.append(root+r"auto/api/EaseInOut.lua");
    apis.append(root+r"auto/api/EaseOut.lua");
    apis.append(root+r"auto/api/EaseQuadraticActionIn.lua");
    apis.append(root+r"auto/api/EaseQuadraticActionInOut.lua");
    apis.append(root+r"auto/api/EaseQuadraticActionOut.lua");
    apis.append(root+r"auto/api/EaseQuarticActionIn.lua");
    apis.append(root+r"auto/api/EaseQuarticActionInOut.lua");
    apis.append(root+r"auto/api/EaseQuarticActionOut.lua");
    apis.append(root+r"auto/api/EaseQuinticActionIn.lua");
    apis.append(root+r"auto/api/EaseQuinticActionInOut.lua");
    apis.append(root+r"auto/api/EaseQuinticActionOut.lua");
    apis.append(root+r"auto/api/EaseRateAction.lua");
    apis.append(root+r"auto/api/EaseSineIn.lua");
    apis.append(root+r"auto/api/EaseSineInOut.lua");
    apis.append(root+r"auto/api/EaseSineOut.lua");
    apis.append(root+r"auto/api/EditBox.lua");
    apis.append(root+r"auto/api/Event.lua");
    apis.append(root+r"auto/api/EventAcceleration.lua");
    apis.append(root+r"auto/api/EventAssetsManagerEx.lua");
    apis.append(root+r"auto/api/EventController.lua");
    apis.append(root+r"auto/api/EventCustom.lua");
    apis.append(root+r"auto/api/EventDispatcher.lua");
    apis.append(root+r"auto/api/EventFocus.lua");
    apis.append(root+r"auto/api/EventFrame.lua");
    apis.append(root+r"auto/api/EventKeyboard.lua");
    apis.append(root+r"auto/api/EventListener.lua");
    apis.append(root+r"auto/api/EventListenerAcceleration.lua");
    apis.append(root+r"auto/api/EventListenerAssetsManagerEx.lua");
    apis.append(root+r"auto/api/EventListenerController.lua");
    apis.append(root+r"auto/api/EventListenerCustom.lua");
    apis.append(root+r"auto/api/EventListenerFocus.lua");
    apis.append(root+r"auto/api/EventListenerKeyboard.lua");
    apis.append(root+r"auto/api/EventListenerMouse.lua");
    apis.append(root+r"auto/api/EventListenerPhysicsContact.lua");
    apis.append(root+r"auto/api/EventListenerPhysicsContactWithBodies.lua");
    apis.append(root+r"auto/api/EventListenerPhysicsContactWithGroup.lua");
    apis.append(root+r"auto/api/EventListenerPhysicsContactWithShapes.lua");
    apis.append(root+r"auto/api/EventListenerTouchAllAtOnce.lua");
    apis.append(root+r"auto/api/EventListenerTouchOneByOne.lua");
    apis.append(root+r"auto/api/EventMouse.lua");
    apis.append(root+r"auto/api/EventTouch.lua");
    apis.append(root+r"auto/api/FadeIn.lua");
    apis.append(root+r"auto/api/FadeOut.lua");
    apis.append(root+r"auto/api/FadeOutBLTiles.lua");
    apis.append(root+r"auto/api/FadeOutDownTiles.lua");
    apis.append(root+r"auto/api/FadeOutTRTiles.lua");
    apis.append(root+r"auto/api/FadeOutUpTiles.lua");
    apis.append(root+r"auto/api/FadeTo.lua");
    apis.append(root+r"auto/api/FileUtils.lua");
    apis.append(root+r"auto/api/FiniteTimeAction.lua");
    apis.append(root+r"auto/api/FlipX.lua");
    apis.append(root+r"auto/api/FlipX3D.lua");
    apis.append(root+r"auto/api/FlipY.lua");
    apis.append(root+r"auto/api/FlipY3D.lua");
    apis.append(root+r"auto/api/Follow.lua");
    apis.append(root+r"auto/api/Frame.lua");
    apis.append(root+r"auto/api/FrameData.lua");
    apis.append(root+r"auto/api/GLProgram.lua");
    apis.append(root+r"auto/api/GLProgramCache.lua");
    apis.append(root+r"auto/api/GLProgramState.lua");
    apis.append(root+r"auto/api/GLView.lua");
    apis.append(root+r"auto/api/GLViewImpl.lua");
    apis.append(root+r"auto/api/Grid3D.lua");
    apis.append(root+r"auto/api/Grid3DAction.lua");
    apis.append(root+r"auto/api/GridAction.lua");
    apis.append(root+r"auto/api/GridBase.lua");
    apis.append(root+r"auto/api/GUIReader.lua");
    apis.append(root+r"auto/api/HBox.lua");
    apis.append(root+r"auto/api/Helper.lua");
    apis.append(root+r"auto/api/Hide.lua");
    apis.append(root+r"auto/api/Image.lua");
    apis.append(root+r"auto/api/ImageView.lua");
    apis.append(root+r"auto/api/InnerActionFrame.lua");
    apis.append(root+r"auto/api/JumpBy.lua");
    apis.append(root+r"auto/api/JumpTiles3D.lua");
    apis.append(root+r"auto/api/JumpTo.lua");
    apis.append(root+r"auto/api/Label.lua");
    apis.append(root+r"auto/api/LabelAtlas.lua");
    apis.append(root+r"auto/api/Layer.lua");
    apis.append(root+r"auto/api/LayerColor.lua");
    apis.append(root+r"auto/api/LayerGradient.lua");
    apis.append(root+r"auto/api/LayerMultiplex.lua");
    apis.append(root+r"auto/api/LayerRadialGradient.lua");
    apis.append(root+r"auto/api/Layout.lua");
    apis.append(root+r"auto/api/LayoutComponent.lua");
    apis.append(root+r"auto/api/LayoutParameter.lua");
    apis.append(root+r"auto/api/Lens3D.lua");
    apis.append(root+r"auto/api/LinearLayoutParameter.lua");
    apis.append(root+r"auto/api/Liquid.lua");
    apis.append(root+r"auto/api/ListView.lua");
    apis.append(root+r"auto/api/LoadingBar.lua");
    apis.append(root+r"auto/api/lua_cocos2dx_3d_auto_api.lua");
    apis.append(root+r"auto/api/lua_cocos2dx_audioengine_auto_api.lua");
    apis.append(root+r"auto/api/lua_cocos2dx_auto_api.lua");
    apis.append(root+r"auto/api/lua_cocos2dx_cocosbuilder_auto_api.lua");
    apis.append(root+r"auto/api/lua_cocos2dx_cocosdenshion_auto_api.lua");
    apis.append(root+r"auto/api/lua_cocos2dx_controller_auto_api.lua");
    apis.append(root+r"auto/api/lua_cocos2dx_csloader_auto_api.lua");
    apis.append(root+r"auto/api/lua_cocos2dx_experimental_auto_api.lua");
    apis.append(root+r"auto/api/lua_cocos2dx_experimental_video_auto_api.lua");
    apis.append(root+r"auto/api/lua_cocos2dx_experimental_webview_auto_api.lua");
    apis.append(root+r"auto/api/lua_cocos2dx_extension_auto_api.lua");
    apis.append(root+r"auto/api/lua_cocos2dx_navmesh_auto_api.lua");
    apis.append(root+r"auto/api/lua_cocos2dx_physics3d_auto_api.lua");
    apis.append(root+r"auto/api/lua_cocos2dx_physics_auto_api.lua");
    apis.append(root+r"auto/api/lua_cocos2dx_spine_auto_api.lua");
    apis.append(root+r"auto/api/lua_cocos2dx_studio_auto_api.lua");
    apis.append(root+r"auto/api/lua_cocos2dx_ui_auto_api.lua");
    apis.append(root+r"auto/api/lua_sky_rad_auto_api.lua");
    apis.append(root+r"auto/api/Manifest.lua");
    apis.append(root+r"auto/api/Material.lua");
    apis.append(root+r"auto/api/Menu.lua");
    apis.append(root+r"auto/api/MenuItem.lua");
    apis.append(root+r"auto/api/MenuItemAtlasFont.lua");
    apis.append(root+r"auto/api/MenuItemFont.lua");
    apis.append(root+r"auto/api/MenuItemImage.lua");
    apis.append(root+r"auto/api/MenuItemLabel.lua");
    apis.append(root+r"auto/api/MenuItemSprite.lua");
    apis.append(root+r"auto/api/MenuItemToggle.lua");
    apis.append(root+r"auto/api/Mesh.lua");
    apis.append(root+r"auto/api/MotionStreak.lua");
    apis.append(root+r"auto/api/MotionStreak3D.lua");
    apis.append(root+r"auto/api/MoveBy.lua");
    apis.append(root+r"auto/api/MovementBoneData.lua");
    apis.append(root+r"auto/api/MovementData.lua");
    apis.append(root+r"auto/api/MoveTo.lua");
    apis.append(root+r"auto/api/NavMesh.lua");
    apis.append(root+r"auto/api/NavMeshAgent.lua");
    apis.append(root+r"auto/api/NavMeshObstacle.lua");
    apis.append(root+r"auto/api/Node.lua");
    apis.append(root+r"auto/api/NodeGrid.lua");
    apis.append(root+r"auto/api/OrbitCamera.lua");
    apis.append(root+r"auto/api/PageTurn3D.lua");
    apis.append(root+r"auto/api/PageView.lua");
    apis.append(root+r"auto/api/ParallaxNode.lua");
    apis.append(root+r"auto/api/ParticleBatchNode.lua");
    apis.append(root+r"auto/api/ParticleData.lua");
    apis.append(root+r"auto/api/ParticleDisplayData.lua");
    apis.append(root+r"auto/api/ParticleExplosion.lua");
    apis.append(root+r"auto/api/ParticleFire.lua");
    apis.append(root+r"auto/api/ParticleFireworks.lua");
    apis.append(root+r"auto/api/ParticleFlower.lua");
    apis.append(root+r"auto/api/ParticleGalaxy.lua");
    apis.append(root+r"auto/api/ParticleMeteor.lua");
    apis.append(root+r"auto/api/ParticleRain.lua");
    apis.append(root+r"auto/api/ParticleSmoke.lua");
    apis.append(root+r"auto/api/ParticleSnow.lua");
    apis.append(root+r"auto/api/ParticleSpiral.lua");
    apis.append(root+r"auto/api/ParticleSun.lua");
    apis.append(root+r"auto/api/ParticleSystem.lua");
    apis.append(root+r"auto/api/ParticleSystem3D.lua");
    apis.append(root+r"auto/api/ParticleSystemQuad.lua");
    apis.append(root+r"auto/api/Pass.lua");
    apis.append(root+r"auto/api/Physics3D6DofConstraint.lua");
    apis.append(root+r"auto/api/Physics3DComponent.lua");
    apis.append(root+r"auto/api/Physics3DConeTwistConstraint.lua");
    apis.append(root+r"auto/api/Physics3DConstraint.lua");
    apis.append(root+r"auto/api/Physics3DHingeConstraint.lua");
    apis.append(root+r"auto/api/Physics3DObject.lua");
    apis.append(root+r"auto/api/Physics3DPointToPointConstraint.lua");
    apis.append(root+r"auto/api/Physics3DRigidBody.lua");
    apis.append(root+r"auto/api/Physics3DShape.lua");
    apis.append(root+r"auto/api/Physics3DSliderConstraint.lua");
    apis.append(root+r"auto/api/Physics3DWorld.lua");
    apis.append(root+r"auto/api/PhysicsBody.lua");
    apis.append(root+r"auto/api/PhysicsContact.lua");
    apis.append(root+r"auto/api/PhysicsContactPostSolve.lua");
    apis.append(root+r"auto/api/PhysicsContactPreSolve.lua");
    apis.append(root+r"auto/api/PhysicsJoint.lua");
    apis.append(root+r"auto/api/PhysicsJointDistance.lua");
    apis.append(root+r"auto/api/PhysicsJointFixed.lua");
    apis.append(root+r"auto/api/PhysicsJointGear.lua");
    apis.append(root+r"auto/api/PhysicsJointGroove.lua");
    apis.append(root+r"auto/api/PhysicsJointLimit.lua");
    apis.append(root+r"auto/api/PhysicsJointMotor.lua");
    apis.append(root+r"auto/api/PhysicsJointPin.lua");
    apis.append(root+r"auto/api/PhysicsJointRatchet.lua");
    apis.append(root+r"auto/api/PhysicsJointRotaryLimit.lua");
    apis.append(root+r"auto/api/PhysicsJointRotarySpring.lua");
    apis.append(root+r"auto/api/PhysicsJointSpring.lua");
    apis.append(root+r"auto/api/PhysicsShape.lua");
    apis.append(root+r"auto/api/PhysicsShapeBox.lua");
    apis.append(root+r"auto/api/PhysicsShapeCircle.lua");
    apis.append(root+r"auto/api/PhysicsShapeEdgeBox.lua");
    apis.append(root+r"auto/api/PhysicsShapeEdgeChain.lua");
    apis.append(root+r"auto/api/PhysicsShapeEdgePolygon.lua");
    apis.append(root+r"auto/api/PhysicsShapeEdgeSegment.lua");
    apis.append(root+r"auto/api/PhysicsShapePolygon.lua");
    apis.append(root+r"auto/api/PhysicsSprite3D.lua");
    apis.append(root+r"auto/api/PhysicsWorld.lua");
    apis.append(root+r"auto/api/Place.lua");
    apis.append(root+r"auto/api/PlayableFrame.lua");
    apis.append(root+r"auto/api/PointLight.lua");
    apis.append(root+r"auto/api/PolygonInfo.lua");
    apis.append(root+r"auto/api/PositionFrame.lua");
    apis.append(root+r"auto/api/ProgressFromTo.lua");
    apis.append(root+r"auto/api/ProgressTimer.lua");
    apis.append(root+r"auto/api/ProgressTo.lua");
    apis.append(root+r"auto/api/Properties.lua");
    apis.append(root+r"auto/api/ProtectedNode.lua");
    apis.append(root+r"auto/api/PUParticleSystem3D.lua");
    apis.append(root+r"auto/api/RadioButton.lua");
    apis.append(root+r"auto/api/RadioButtonGroup.lua");
    apis.append(root+r"auto/api/Ref.lua");
    apis.append(root+r"auto/api/RelativeBox.lua");
    apis.append(root+r"auto/api/RelativeLayoutParameter.lua");
    apis.append(root+r"auto/api/RemoveSelf.lua");
    apis.append(root+r"auto/api/RenderState.lua");
    apis.append(root+r"auto/api/RenderTexture.lua");
    apis.append(root+r"auto/api/Repeat.lua");
    apis.append(root+r"auto/api/RepeatForever.lua");
    apis.append(root+r"auto/api/ReuseGrid.lua");
    apis.append(root+r"auto/api/RichElement.lua");
    apis.append(root+r"auto/api/RichElementCustomNode.lua");
    apis.append(root+r"auto/api/RichElementImage.lua");
    apis.append(root+r"auto/api/RichElementNewLine.lua");
    apis.append(root+r"auto/api/RichElementText.lua");
    apis.append(root+r"auto/api/RichText.lua");
    apis.append(root+r"auto/api/Ripple3D.lua");
    apis.append(root+r"auto/api/RotateBy.lua");
    apis.append(root+r"auto/api/RotateTo.lua");
    apis.append(root+r"auto/api/RotationFrame.lua");
    apis.append(root+r"auto/api/RotationSkewFrame.lua");
    apis.append(root+r"auto/api/Scale9Sprite.lua");
    apis.append(root+r"auto/api/ScaleBy.lua");
    apis.append(root+r"auto/api/ScaleFrame.lua");
    apis.append(root+r"auto/api/ScaleTo.lua");
    apis.append(root+r"auto/api/Scene.lua");
    apis.append(root+r"auto/api/SceneReader.lua");
    apis.append(root+r"auto/api/Scheduler.lua");
    apis.append(root+r"auto/api/ScrollView.lua");
    apis.append(root+r"auto/api/Sequence.lua");
    apis.append(root+r"auto/api/Shaky3D.lua");
    apis.append(root+r"auto/api/ShakyTiles3D.lua");
    apis.append(root+r"auto/api/ShatteredTiles3D.lua");
    apis.append(root+r"auto/api/Show.lua");
    apis.append(root+r"auto/api/ShuffleTiles.lua");
    apis.append(root+r"auto/api/SimpleAudioEngine.lua");
    apis.append(root+r"auto/api/Skeleton3D.lua");
    apis.append(root+r"auto/api/SkeletonAnimation.lua");
    apis.append(root+r"auto/api/SkeletonNode.lua");
    apis.append(root+r"auto/api/SkeletonRenderer.lua");
    apis.append(root+r"auto/api/SkewBy.lua");
    apis.append(root+r"auto/api/SkewFrame.lua");
    apis.append(root+r"auto/api/SkewTo.lua");
    apis.append(root+r"auto/api/Skin.lua");
    apis.append(root+r"auto/api/Skybox.lua");
    apis.append(root+r"auto/api/Slider.lua");
    apis.append(root+r"auto/api/Spawn.lua");
    apis.append(root+r"auto/api/Speed.lua");
    apis.append(root+r"auto/api/SplitCols.lua");
    apis.append(root+r"auto/api/SplitRows.lua");
    apis.append(root+r"auto/api/SpotLight.lua");
    apis.append(root+r"auto/api/Sprite.lua");
    apis.append(root+r"auto/api/Sprite3D.lua");
    apis.append(root+r"auto/api/Sprite3DCache.lua");
    apis.append(root+r"auto/api/Sprite3DMaterial.lua");
    apis.append(root+r"auto/api/SpriteBatchNode.lua");
    apis.append(root+r"auto/api/SpriteDisplayData.lua");
    apis.append(root+r"auto/api/SpriteFrame.lua");
    apis.append(root+r"auto/api/SpriteFrameCache.lua");
    apis.append(root+r"auto/api/SpriteWithHue.lua");
    apis.append(root+r"auto/api/StopGrid.lua");
    apis.append(root+r"auto/api/TabControl.lua");
    apis.append(root+r"auto/api/TabHeader.lua");
    apis.append(root+r"auto/api/TableView.lua");
    apis.append(root+r"auto/api/TableViewCell.lua");
    apis.append(root+r"auto/api/TargetedAction.lua");
    apis.append(root+r"auto/api/Technique.lua");
    apis.append(root+r"auto/api/Terrain.lua");
    apis.append(root+r"auto/api/Text.lua");
    apis.append(root+r"auto/api/TextAtlas.lua");
    apis.append(root+r"auto/api/TextBMFont.lua");
    apis.append(root+r"auto/api/TextField.lua");
    apis.append(root+r"auto/api/Texture2D.lua");
    apis.append(root+r"auto/api/TextureCache.lua");
    apis.append(root+r"auto/api/TextureCube.lua");
    apis.append(root+r"auto/api/TextureData.lua");
    apis.append(root+r"auto/api/TextureFrame.lua");
    apis.append(root+r"auto/api/TiledGrid3D.lua");
    apis.append(root+r"auto/api/TiledGrid3DAction.lua");
    apis.append(root+r"auto/api/TileMapAtlas.lua");
    apis.append(root+r"auto/api/Timeline.lua");
    apis.append(root+r"auto/api/Timer.lua");
    apis.append(root+r"auto/api/TintBy.lua");
    apis.append(root+r"auto/api/TintTo.lua");
    apis.append(root+r"auto/api/TMXLayer.lua");
    apis.append(root+r"auto/api/TMXLayerInfo.lua");
    apis.append(root+r"auto/api/TMXMapInfo.lua");
    apis.append(root+r"auto/api/TMXObjectGroup.lua");
    apis.append(root+r"auto/api/TMXTiledMap.lua");
    apis.append(root+r"auto/api/TMXTilesetInfo.lua");
    apis.append(root+r"auto/api/ToggleVisibility.lua");
    apis.append(root+r"auto/api/Touch.lua");
    apis.append(root+r"auto/api/TransitionCrossFade.lua");
    apis.append(root+r"auto/api/TransitionEaseScene.lua");
    apis.append(root+r"auto/api/TransitionFade.lua");
    apis.append(root+r"auto/api/TransitionFadeBL.lua");
    apis.append(root+r"auto/api/TransitionFadeDown.lua");
    apis.append(root+r"auto/api/TransitionFadeTR.lua");
    apis.append(root+r"auto/api/TransitionFadeUp.lua");
    apis.append(root+r"auto/api/TransitionFlipAngular.lua");
    apis.append(root+r"auto/api/TransitionFlipX.lua");
    apis.append(root+r"auto/api/TransitionFlipY.lua");
    apis.append(root+r"auto/api/TransitionJumpZoom.lua");
    apis.append(root+r"auto/api/TransitionMoveInB.lua");
    apis.append(root+r"auto/api/TransitionMoveInL.lua");
    apis.append(root+r"auto/api/TransitionMoveInR.lua");
    apis.append(root+r"auto/api/TransitionMoveInT.lua");
    apis.append(root+r"auto/api/TransitionPageTurn.lua");
    apis.append(root+r"auto/api/TransitionProgress.lua");
    apis.append(root+r"auto/api/TransitionProgressHorizontal.lua");
    apis.append(root+r"auto/api/TransitionProgressInOut.lua");
    apis.append(root+r"auto/api/TransitionProgressOutIn.lua");
    apis.append(root+r"auto/api/TransitionProgressRadialCCW.lua");
    apis.append(root+r"auto/api/TransitionProgressRadialCW.lua");
    apis.append(root+r"auto/api/TransitionProgressVertical.lua");
    apis.append(root+r"auto/api/TransitionRotoZoom.lua");
    apis.append(root+r"auto/api/TransitionScene.lua");
    apis.append(root+r"auto/api/TransitionSceneOriented.lua");
    apis.append(root+r"auto/api/TransitionShrinkGrow.lua");
    apis.append(root+r"auto/api/TransitionSlideInB.lua");
    apis.append(root+r"auto/api/TransitionSlideInL.lua");
    apis.append(root+r"auto/api/TransitionSlideInR.lua");
    apis.append(root+r"auto/api/TransitionSlideInT.lua");
    apis.append(root+r"auto/api/TransitionSplitCols.lua");
    apis.append(root+r"auto/api/TransitionSplitRows.lua");
    apis.append(root+r"auto/api/TransitionTurnOffTiles.lua");
    apis.append(root+r"auto/api/TransitionZoomFlipAngular.lua");
    apis.append(root+r"auto/api/TransitionZoomFlipX.lua");
    apis.append(root+r"auto/api/TransitionZoomFlipY.lua");
    apis.append(root+r"auto/api/TurnOffTiles.lua");
    apis.append(root+r"auto/api/Tween.lua");
    apis.append(root+r"auto/api/Twirl.lua");
    apis.append(root+r"auto/api/UserDefault.lua");
    apis.append(root+r"auto/api/VBox.lua");
    apis.append(root+r"auto/api/VideoPlayer.lua");
    apis.append(root+r"auto/api/VisibleFrame.lua");
    apis.append(root+r"auto/api/Waves.lua");
    apis.append(root+r"auto/api/Waves3D.lua");
    apis.append(root+r"auto/api/WavesTiles3D.lua");
    apis.append(root+r"auto/api/WebView.lua");
    apis.append(root+r"auto/api/Widget.lua");
    apis.append(root+r"auto/api/ZOrderFrame.lua");

    luas=[];
    luas.append(root+r"script/init.lua");
    luas.append(root+r"script/3d/3dConstants.lua");
    luas.append(root+r"script/cocos2d/bitExtend.lua");
    luas.append(root+r"script/cocos2d/Cocos2d.lua");
    luas.append(root+r"script/cocos2d/Cocos2dConstants.lua");
    luas.append(root+r"script/cocos2d/deprecated.lua");
    luas.append(root+r"script/cocos2d/DeprecatedCocos2dClass.lua");
    luas.append(root+r"script/cocos2d/DeprecatedCocos2dEnum.lua");
    luas.append(root+r"script/cocos2d/DeprecatedCocos2dFunc.lua");
    luas.append(root+r"script/cocos2d/DeprecatedOpenglEnum.lua");
    luas.append(root+r"script/cocos2d/DrawPrimitives.lua");
    luas.append(root+r"script/cocos2d/functions.lua");
    luas.append(root+r"script/cocos2d/json.lua");
    luas.append(root+r"script/cocos2d/luaj.lua");
    luas.append(root+r"script/cocos2d/luaoc.lua");
    luas.append(root+r"script/cocos2d/Opengl.lua");
    luas.append(root+r"script/cocos2d/OpenglConstants.lua");
    luas.append(root+r"script/cocosbuilder/CCBReaderLoad.lua");
    luas.append(root+r"script/cocosbuilder/DeprecatedCocosBuilderClass.lua");
    luas.append(root+r"script/cocosdenshion/AudioEngine.lua");
    luas.append(root+r"script/cocosdenshion/DeprecatedCocosDenshionClass.lua");
    luas.append(root+r"script/cocosdenshion/DeprecatedCocosDenshionFunc.lua");
    luas.append(root+r"script/cocostudio/CocoStudio.lua");
    luas.append(root+r"script/cocostudio/DeprecatedCocoStudioClass.lua");
    luas.append(root+r"script/cocostudio/DeprecatedCocoStudioFunc.lua");
    luas.append(root+r"script/cocostudio/StudioConstants.lua");
    luas.append(root+r"script/controller/ControllerConstants.lua");
    luas.append(root+r"script/extension/DeprecatedExtensionClass.lua");
    luas.append(root+r"script/extension/DeprecatedExtensionEnum.lua");
    luas.append(root+r"script/extension/DeprecatedExtensionFunc.lua");
    luas.append(root+r"script/extension/ExtensionConstants.lua");
    luas.append(root+r"script/framework/audio.lua");
    luas.append(root+r"script/framework/device.lua");
    luas.append(root+r"script/framework/display.lua");
    luas.append(root+r"script/framework/init.lua");
    luas.append(root+r"script/framework/package_support.lua");
    luas.append(root+r"script/framework/transition.lua");
    luas.append(root+r"script/framework/components/event.lua");
    luas.append(root+r"script/framework/extends/LayerEx.lua");
    luas.append(root+r"script/framework/extends/MenuEx.lua");
    luas.append(root+r"script/framework/extends/NodeEx.lua");
    luas.append(root+r"script/framework/extends/SpriteEx.lua");
    luas.append(root+r"script/framework/extends/UICheckBox.lua");
    luas.append(root+r"script/framework/extends/UIEditBox.lua");
    luas.append(root+r"script/framework/extends/UIListView.lua");
    luas.append(root+r"script/framework/extends/UIPageView.lua");
    luas.append(root+r"script/framework/extends/UIScrollView.lua");
    luas.append(root+r"script/framework/extends/UISlider.lua");
    luas.append(root+r"script/framework/extends/UITextField.lua");
    luas.append(root+r"script/framework/extends/UIWidget.lua");
    luas.append(root+r"script/network/DeprecatedNetworkClass.lua");
    luas.append(root+r"script/network/DeprecatedNetworkEnum.lua");
    luas.append(root+r"script/network/DeprecatedNetworkFunc.lua");
    luas.append(root+r"script/network/NetworkConstants.lua");
    luas.append(root+r"script/physics3d/physics3d-constants.lua");
    luas.append(root+r"script/spine/SpineConstants.lua");
    luas.append(root+r"script/ui/DeprecatedUIEnum.lua");
    luas.append(root+r"script/ui/DeprecatedUIFunc.lua");
    luas.append(root+r"script/ui/experimentalUIConstants.lua");
    luas.append(root+r"script/ui/GuiConstants.lua");

    for line in apis:
        api=open(getCurrentPath()+"../"+line);

    for line in luas:
        lua=open(getCurrentPath()+"../"+line);

def toLuaCpps():
    cpps=[];
    clsdir=r"frameworks/runtime-src/Classes/";
    cpps.append(clsdir+r"reader/lua-bindings/creator_reader_bindings.cpp");
    cpps.append(clsdir+r"reader/lua-bindings/dragonbones/lua_dragonbones_auto.cpp");
    cpps.append(clsdir+r"reader/lua-bindings/dragonbones/lua_dragonbones_manual.cpp");
    cpps.append(clsdir+r"reader/lua-bindings/reader/lua_creator_reader_auto.cpp");
    cpps.append(clsdir+r"reader/lua-bindings/reader/lua_creator_reader_manual.cpp");
    clsdir2=r"frameworks/cocos2d-x/cocos/scripting/lua-bindings/";
    cpps.append(clsdir2+r"auto/lua_cocos2dx_3d_auto.cpp");
    cpps.append(clsdir2+r"auto/lua_cocos2dx_audioengine_auto.cpp");
    cpps.append(clsdir2+r"auto/lua_cocos2dx_auto.cpp");
    cpps.append(clsdir2+r"auto/lua_cocos2dx_cocosbuilder_auto.cpp");
    cpps.append(clsdir2+r"auto/lua_cocos2dx_cocosdenshion_auto.cpp");
    cpps.append(clsdir2+r"auto/lua_cocos2dx_controller_auto.cpp");
    cpps.append(clsdir2+r"auto/lua_cocos2dx_csloader_auto.cpp");
    cpps.append(clsdir2+r"auto/lua_cocos2dx_experimental_auto.cpp");
    cpps.append(clsdir2+r"auto/lua_cocos2dx_experimental_video_auto.cpp");
    cpps.append(clsdir2+r"auto/lua_cocos2dx_experimental_webview_auto.cpp");
    cpps.append(clsdir2+r"auto/lua_cocos2dx_extension_auto.cpp");
    cpps.append(clsdir2+r"auto/lua_cocos2dx_navmesh_auto.cpp");
    cpps.append(clsdir2+r"auto/lua_cocos2dx_physics3d_auto.cpp");
    cpps.append(clsdir2+r"auto/lua_cocos2dx_physics_auto.cpp");
    cpps.append(clsdir2+r"auto/lua_cocos2dx_spine_auto.cpp");
    cpps.append(clsdir2+r"auto/lua_cocos2dx_studio_auto.cpp");
    cpps.append(clsdir2+r"auto/lua_cocos2dx_ui_auto.cpp");
    cpps.append(clsdir2+r"auto/lua_sky_rad_auto.cpp");
    cpps.append(clsdir2+r"manual/3d/lua_cocos2dx_3d_manual.cpp");
    cpps.append(clsdir2+r"manual/audioengine/lua_cocos2dx_audioengine_manual.cpp");
    cpps.append(clsdir2+r"manual/cocos2d/lua_cocos2dx_deprecated.cpp");
    cpps.append(clsdir2+r"manual/cocos2d/lua_cocos2dx_experimental_manual.cpp");
    cpps.append(clsdir2+r"manual/cocos2d/lua_cocos2dx_manual.cpp");
    cpps.append(clsdir2+r"manual/cocos2d/lua_cocos2dx_physics_manual.cpp");
    cpps.append(clsdir2+r"manual/cocos2d/luaOpengl.cpp");
    cpps.append(clsdir2+r"manual/cocos2d/luaScriptHandlerMgr.cpp");
    cpps.append(clsdir2+r"manual/cocosbuilder/CCBProxy.cpp");
    cpps.append(clsdir2+r"manual/cocosbuilder/lua_cocos2dx_cocosbuilder_manual.cpp");
    cpps.append(clsdir2+r"manual/cocosdenshion/lua_cocos2dx_cocosdenshion_manual.cpp");
    cpps.append(clsdir2+r"manual/cocostudio/CustomGUIReader.cpp");
    cpps.append(clsdir2+r"manual/cocostudio/lua_cocos2dx_coco_studio_manual.cpp");
    cpps.append(clsdir2+r"manual/cocostudio/lua_cocos2dx_csloader_manual.cpp");
    cpps.append(clsdir2+r"manual/cocostudio/lua-cocos-studio-conversions.cpp");
    cpps.append(clsdir2+r"manual/extension/lua_cocos2dx_extension_manual.cpp");
    cpps.append(clsdir2+r"manual/navmesh/lua_cocos2dx_navmesh_conversions.cpp");
    cpps.append(clsdir2+r"manual/navmesh/lua_cocos2dx_navmesh_manual.cpp");
    cpps.append(clsdir2+r"manual/network/lua_cocos2dx_network_manual.cpp");
    cpps.append(clsdir2+r"manual/network/lua_extensions.c");
    cpps.append(clsdir2+r"manual/network/lua_web_socket.cpp");
    cpps.append(clsdir2+r"manual/network/lua_xml_http_request.cpp");
    cpps.append(clsdir2+r"manual/physics3d/lua_cocos2dx_physics3d_manual.cpp");
    cpps.append(clsdir2+r"manual/spine/lua_cocos2dx_spine_manual.cpp");
    cpps.append(clsdir2+r"manual/spine/LuaSkeletonAnimation.cpp");
    cpps.append(clsdir2+r"manual/ui/lua_cocos2dx_experimental_video_manual.cpp");
    cpps.append(clsdir2+r"manual/ui/lua_cocos2dx_experimental_webview_manual.cpp");
    cpps.append(clsdir2+r"manual/ui/lua_cocos2dx_ui_manual.cpp");
    for line in cpps:
        cpp=open(getCurrentPath()+"../"+line);
    
if __name__ == '__main__':
    reload(sys)
    sys.setdefaultencoding( "utf-8" )
    ss=u"请关注全国留守儿童，请关注全国城乡差距，请关注全国教育现状......"
    print ss
    cmd = "TITLE "+ss
    os.system(cmd.encode('gb2312'))
    print "author cclin 2015.08"
    print "support:e-mail=12092020@qq.com"
    print "copyright 2015~2018 for anyone to use"
    
    #pattern=re.compile(r"\s*(?P<cname>\w*)\s*=\s*\({0,1}\s*(?P<cvalue>[\w\s+-]*\s*)\s*\){0,1}\s*--\s*[\s*\S*]*");#luaconst
    
    #JSQY_CMD_RSV1_WORDGAME_LAST=JSQY_CMD_RSV1_TOWERDEFENCE + 1
    #JSQY_WORDGAME_COUNT=JSQY_CMD_RSV1_WORDGAME_LAST - JSQY_CMD_RSV1_WORDGAME_START
    
    #pattern=re.compile(getRegexPattern(""));
    '''
    pattern=re.compile(r"\s*\[(?P<arraylen>[\w\s+-\\*/]*)\]\s*");
    conten="[ a / 2 ][b][c][d]";
    match=pattern.match(conten);
    if match:
        #print "type="+match.group("ftype"),"name="+match.group("fname"),"count="+match.group("fcount"),"memo="+match.group("fmemo");
        print "arraylen="+str(match.group());
    else:
        print "no match";
    '''
    #isNum,base=isIntS("0x0001");
    #print CalcMatch(conten);
    
    #content="	char	m_szName[MAX_NAME_LEN+1];\n";
    #print StructMatch(content);
    
    #for c++
    #ConvertCmdID(r"protocol/protocol.h",r"nprotocol/protocol.h")
    #ConvertCmdID(r"protocol/Notify.h","nprotocol/Notify.h")

    #for lua
    #CommandToLua(r"protocol/protocol.h",r"lua/skydream_cmd.lua");

    #ConvertProtocol();
    
    #ApiToSnippets(getCurrentPath()+r"cocos2dx3Api.lua");;
    
    '''
    reg_str=r"\s*(?P<ns>\w*)\.(?P<cls>\w*)\s*=\s*\{\}*\s*";
    pattern=re.compile(reg_str);
    match = pattern.match(r" ccexp.VideoPlayer = {} ");
    print(match.groups())
    '''

    '''
    reg_str=r"\s*function\s*(?P<cls>\S*)\:(?P<fun>\S*)\s*end\s*";
    pattern=re.compile(reg_str);
    match = pattern.match(r"function cc.CSLoader:createNodeWithVisibleSize(const_std_string_filename) end");
    print(match.group("cls")+":"+match.group("fun"))
    '''
    
    '''
    class_str=r'\s*tolua_cclass(tolua_S,"Ref","cc.Ref","",nullptr);\s*';
    class_begin=r'tolua_beginmodule(tolua_S,"Ref");';
    class_end=r'tolua_endmodule(tolua_S);';
    member_str=r'tolua_function(tolua_S,"release",lua_cocos2dx_Ref_release);';
    #TODO:
    member_params=r'';

    manul_start=r'static void extendEventListenerFocusEvent(lua_State* L)';
    class_name=r'lua_pushstring(tolua_S, "cc.AutoPolygon");'
    member_name=r'tolua_function(tolua_S, "generatePolygon", lua_cocos2dx_AutoPolygon_generatePolygon);';
    #TODO:
    member_params=r'';
    manul_end=r'}';
    
    #new_str=r'tolua_function(tolua_S,"new",lua_cocos2dx_ui_LayoutParameter_constructor);';

    '''

    api_start="--------------------------------"

    '''
    fun_structs=[           #local function,global function
        fun_name=""         #field name
        fun_type=""         #enum={"static","normal"}
        params=[]           #params names
        ret_values=[]       #return fields, should be field structs 
    ]
    field_structs=[         #table,userdata,params struct,field_struct,class,var,lightuserdata,cdata(math&luajit is a global field)
        fname=""            #field name
        ext_names=[]        #parent class names
        ftype=""            #field type,can be class name
        pnt_name=""         #parent module, field_struct available
        fun_names=[]        #function names 
        fld_names=[]        #field names
        mta_names=[]        #meta table names, should be a lua table
    ]
    file_struct=[           #lua script code file
        file_pname=""       #the file path & file name
        ept_funs=[]         #export function names
        ept_fields=[]       #export field names
        glo_funcs=[]        #global function names
        glo_fields=[]       #global vars names
        inr_funcs=[]        #inner local func names
        inr_fields=[]       #inner local field names 
    ]
    module_struct=[         #a module is like a global field         
        mod_name=""         #module name
        file_pnames=[]      #this module files
    ]
    #189
    #296
    #41508
    '''

    #toLuaCpps()
    toLuaLuas()
    
                                       