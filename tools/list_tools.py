#coding=utf-8
#__author__ = 'cclin'
#write by cclin 2015.5.10

import sys
import os,os.path
import re
import codecs
import xml.dom.minidom as minidom         
from xml.etree import ElementTree as ET

# ==由于minidom默认的writexml()函数在读取一个xml文件后，修改后重新写入如果加了newl='\n',会将原有的xml中写入多余的行
#　 ==因此使用下面这个函数来代替
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

def walk_files(dir,list,aext,topdown=True):
    for root, dirs, files in os.walk(dir, topdown):
        for name in files:
            ext=os.path.splitext(name)[1]
            file=os.path.join(root,name)     
            file=file.replace("\\","/")
            #print file
            if (ext == aext) and  file.find(".svn")==-1 and file not in list:                
                list.append(file)
                #print file;

def forceDirectory(file):
    path=os.path.dirname(file);    
    if not os.path.exists(path):
        os.makedirs(path);

def nappendArrayToFile(rlist,rfile):        
    f=open(rfile,"a")    
    for line in rlist:
        f.write(line+"\n")
    f.flush()
    f.close()
    
def nwriteArrayToFile(rlist,rfile):
    forceDirectory(rfile);
    f=open(rfile,"w")    
    for line in rlist:
        f.write(line+"\n")
    f.flush()
    f.close()

def deleteEmptyLine(rfile):
     f=open(rfile,"r")
     newlist=[]
     for line in f:        
        if line.strip()!="":            
            newlist.append(line)            
     wf=open(rfile,"w")     
     wf.truncate()
     wf.writelines(newlist)

def getCurrentPath():
    return os.path.split(os.path.realpath(__file__))[0]+r"/"


def writeArrayToFile(rlist,rfile):        
    f=open(rfile,"a")    
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

def isPlatformRealWin32(projfile):
    iswin32s=[];
    iswin32s.append("2d/libcocos2d.");
    iswin32s.append("proj.win32/");
    iswin32s.append("proj.win10/");
    isWin32Proj=False;
    for line in iswin32s:
        if projfile.find(line)>=0:
            isWin32Proj=True;
    nowin32s=[];
    nowin32s.append("__PACKAGE_NAME__");
    nowin32s.append("-template-");
    for line in nowin32s:
        if projfile.find(line)>=0:
            isWin32Proj=False;
    return isWin32Proj;
    
def getRootDir(projfile):
    dirtrees=projfile.split("/");
    rootdir="";
    for dir in dirtrees:        
        if dir.startswith("cocos2d"):
            break;
        if rootdir!="":
            rootdir=rootdir+"/";
        rootdir=rootdir+dir;
    if projtype=="project":
        rootdir=rootdir;
    #print rootdir;
    forceDirectory(rootdir);
    #print rootdir;
    return rootdir;
    
def getEngineDir(projectfiles):    
    for line in projectfiles:
        if line.find("/dragonbones/renderer/")>-1:
            continue;
        if line.find("cocos2d")>-1:
            rootdir=getRootDir(line);
            return rootdir;
    print "error get Enginedir";
    return "";
    
def SolutionRelList(fileName):
    namelist=[];
    namelist.append("libbox2d.vcxproj");
    namelist.append("libcocos2d.vcxproj");
    namelist.append("libluacocos2d.vcxproj");
    namelist.append("libJSBinding.vcxproj");
    namelist.append("libsimulator.vcxproj");
    namelist.append("libSpine.vcxproj");
    namelist.append("skydream.vcxproj");
    namelist.append("quick_libs.vcxproj");
    for line in namelist:        
        if fileName.strip()==line.strip():            
            return True;
    return False;
    
def changeOutDir(projfile,rootdir):
    #print projfile;
    arr=projfile.split("/");    
    #print arr[len(arr)-1];
    rootdir=rootdir.replace("/","\\");
    
    #out:$(SolutionDir)..\..\..\..\..\_out\$(SolutionName)\$(Configuration).win32\
    #int:$(SolutionDir)..\..\..\..\..\_out\$(SolutionName)\_int\$(ProjectName)_$(Configuration).win32\
    #log:$(SolutionDir)..\..\..\..\..\_out\$(SolutionName)\log\$(MSBuildProjectName).log
    #workdir:$(OutDir)..\$(Configuration)_$(SolutionName)
    #command:$(OutDir)..\$(Configuration)_$(SolutionName)\$(ProjectName).exe
    
    if projtype=="project" and SolutionRelList(arr[len(arr)-1]):
        '''
        outdir="$(SolutionDir)..\\..\\..\\_out\\$(SolutionName)\\$(Configuration).win32\\";
        intdir="$(SolutionDir)..\\..\\..\\_out\\$(SolutionName)\\_int\\$(ProjectName)_$(Configuration).win32\\";
        logdir="$(SolutionDir)..\\..\\..\\_out\\$(SolutionName)\\log\\$(MSBuildProjectName).log";
        workdir="$(OutDir)..\$(Configuration)_$(SolutionName)";
        command="$(OutDir)..\$(Configuration)_$(SolutionName)\$(ProjectName).exe"
        '''
        outdir=rootdir+"\\_out\\$(SolutionName)\\$(Configuration).win32\\";
        intdir=rootdir+"\\_out\\$(SolutionName)\\_int\\$(ProjectName)_$(Configuration).win32\\";
        logdir=rootdir+"\\_out\\$(SolutionName)\\log\\$(MSBuildProjectName).log";
        workdir="$(OutDir)..\\$(Configuration)_$(SolutionName)";
        command="$(OutDir)..\\$(Configuration)_$(SolutionName)\\$(ProjectName).exe"
    else:
        outdir=rootdir+"\\_out\\$(SolutionName)\\$(Configuration).win32\\";
        intdir=rootdir+"\\_out\\$(SolutionName)\\_int\\$(ProjectName)_$(Configuration).win32\\";
        logdir=rootdir+"\\_out\\$(SolutionName)\\log\\$(MSBuildProjectName).log";
    
    print projfile;
    proj_dom=minidom.parse(projfile)
    items=proj_dom.getElementsByTagName("OutDir")
    for item in items:
        item.childNodes[0].nodeValue=outdir;
    
    items=proj_dom.getElementsByTagName("IntDir")
    for item in items:
        item.childNodes[0].nodeValue=intdir;
        
    items=proj_dom.getElementsByTagName("Path")
    if items.length>0:
        for item in items:
            item.childNodes[0].nodeValue=logdir;
    else:
        items=proj_dom.getElementsByTagName("ItemDefinitionGroup")
        for item in items:
            #nameE=dom.createElement('name')
            #nameT=dom.createTextNode('linux')
            lognode=proj_dom.createElement("BuildLog");
            path=proj_dom.createElement("Path");
            lognode.appendChild(path);
            dir=proj_dom.createTextNode(logdir);
            path.appendChild(dir);
            item.appendChild(lognode);
    '''
    items=proj_dom.getElementsByTagName("LocalDebuggerWorkingDirectory")
    for item in items:
        item.childNodes[0].nodeValue=workdir;
        
    items=proj_dom.getElementsByTagName("LocalDebuggerCommand")
    for item in items:
        item.childNodes[0].nodeValue=command;
    '''
    prettyxml=proj_dom.toprettyxml()
    f=open(projfile,"w")
    f.write(prettyxml)  
    f.close()
    
def listVSProjs(path):
    #path=r"F:\_ccWork\cocos\skydream\client";
    projfiles=[];
    realwinewproj=[];
    walk_files(path,projfiles,".vcxproj");
    for line in projfiles:
        if isPlatformRealWin32(line):
            realwinewproj.append(line);
    nwriteArrayToFile(realwinewproj,getCurrentPath()+"./vsprojs.txt");  
    
    global projtype;
    projtype=getDirType(realwinewproj);
    rootdir=getEngineDir(realwinewproj);
    #convert out & int dirs
    for line in realwinewproj:
        #print line;
        changeOutDir(line,rootdir);
        
def getDirType(projfiles):    
    for line in projfiles:
        dirtrees=line.split("/");
        for dir in dirtrees:
            if dir=="cocos2d-x-3.x":
                continue;
            if dir.startswith("cocos2d-x-"):
                return "engine";
            elif dir=="cocos2d" or dir=="cocos2d-html5":
                return "project";
    return "project";
    
def listSolution(path):
    #path=r"F:\_ccWork\cocos\skydream\client";
    slnfiles=[];    
    walk_files(path,slnfiles,".sln");    
    nwriteArrayToFile(slnfiles,getCurrentPath()+"./vsslns.txt");
    
def listUsers(path):
    #path=r"F:\_ccWork\cocos\skydream\client";
    slnfiles=[];    
    walk_files(path,slnfiles,".user");    
    nwriteArrayToFile(slnfiles,getCurrentPath()+"./vsusers.txt");
    
def listSources(path):
    cppfiles=[];
    walk_files(path,cppfiles,".h");
    walk_files(path,cppfiles,".hpp");
    walk_files(path,cppfiles,".cpp");
    walk_files(path,cppfiles,".c");
    walk_files(path,cppfiles,".cxx");
    walk_dirs(path,cppfiles);
    nwriteArrayToFile(cppfiles,getCurrentPath()+"./sources.txt");

def listHeaders(path):
    cppfiles=[];
    nwriteArrayToFile(cppfiles,getCurrentPath()+"./headers.txt");

if __name__ == '__main__':
    reload(sys) 
    sys.setdefaultencoding( "utf-8" )  
    ss=u"请关注全国留守儿童，请关注全国城乡差距，请关注全国教育现状......"
    print ss
    cmd = "TITLE "+ss
    os.system(cmd.encode('gb2312'))    
    print "author cclin 2015.04"
    print "support:e-mail=12092020@qq.com"
    print "copyright 2015~2018 for anyone to use"
        
    #getRootDir("E:/_work/cocos2d-x-3.7/cocos/audio");
    #path = getCurrentPath()+r"../runtime-src/Classes";
    path = r"E:/Cocos/projects/skyheroes/frameworks/runtime-src/Classes"
    listSources(path);
    #listHeaders(path);
    #listVSProjs(path);
    #listSolution(path);
    #listUsers(path);
                                       