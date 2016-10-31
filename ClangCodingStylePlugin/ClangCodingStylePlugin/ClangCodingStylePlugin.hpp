/*
 *  ClangCodingStylePlugin.hpp
 *  ClangCodingStylePlugin
 *
 *  Created by KyleWong on 26/10/2016.
 *  Copyright © 2016 KyleWong. All rights reserved.
 *
 */

#ifndef ClangCodingStylePlugin_
#define ClangCodingStylePlugin_

#include<iostream>
#include<sstream>
#include<typeinfo>

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "CodingStyleUtil.hpp"
/**
 * @discussion Enviroment:clang-3.9.1(release 39)
 * In terminal, use "/opt/llvm/llvm_build/bin/clang ../test.m -Xclang -load -Xclang /opt/llvm/clangplugin/libClangCodingStylePlugin.dylib -Xclang -plugin -Xclang ClangCodingStylePlugin".
 * In Xcode, use "-Xclang -load -Xclang /opt/llvm/clangplugin/libClangCodingStylePlugin.dylib -Xclang -add-plugin -Xclang ClangCodingStylePlugin",notice the -add-plugin instead of -plugin.
 *****XcodeHacking
 * sudo mv HackedClang.xcplugin `xcode-select -print-path`/../PlugIns/Xcode3Core.ideplugin/Contents/SharedSupport/Developer/Library/Xcode/Plug-ins
 * sudo mv HackedBuildSystem.xcspec `xcode-select -print-path`/Platforms/iPhoneSimulator.platform/Developer/Library/Xcode/Specifications
 * See HackedClang.xcplugin/HackedBuildSystem.xcspec @https://raw.githubusercontent.com/kangwang1988/kangwang1988.github.io/master/others/XcodeHacking.
 */

/* The classes below are exported */
#pragma GCC visibility push(default)
using namespace clang;
using namespace std;
using namespace llvm;

namespace CodingStyle
{
    class CodingStyleASTVisitor : public RecursiveASTVisitor<CodingStyleASTVisitor>
    {
    private:
        ASTContext *context;
        string objcClsImpl;
        bool objcIsInstanceMethod;
        string objcSelector;
        string objcMethodSrcCode;
    public:
        void setContext(ASTContext &context);
        string pureSelFromSelector(string selector);
        bool VisitDecl(Decl *decl);
        bool VisitStmt(Stmt *s);
        void checkInterfaceDecl(ObjCInterfaceDecl* interfDecl);
        void checkPropertyDecl(ObjCPropertyDecl* interfDecl);
        void checkMethodDecl(ObjCMethodDecl* methodDecl);
        void checkIvarDecl(ObjCIvarDecl* ivarDecl);
        void checkEnumDecl(EnumDecl* methodDecl);
        void checkMessageExpr(ObjCMessageExpr *objcExpr);
        void checkIfStmt(IfStmt* is);
    };
    class CodingStyleASTConsumer : public ASTConsumer
    {
    private:
        CodingStyleASTVisitor visitor;
        void HandleTranslationUnit(ASTContext &context);
    };
    
    class CodingStyleASTAction : public PluginASTAction
    {
    public:
        unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &Compiler,llvm::StringRef InFile);
        bool ParseArgs(const CompilerInstance &CI, const std::vector<std::string>& args);
    };
}
#pragma GCC visibility pop
#endif
