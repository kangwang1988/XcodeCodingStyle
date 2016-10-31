/*
 *  ClangCodingStylePlugin.cpp
 *  ClangCodingStylePlugin
 *
 *  Created by KyleWong on 26/10/2016.
 *  Copyright © 2016 KyleWong. All rights reserved.
 *
 */

#include <iostream>
#include "ClangCodingStylePlugin.hpp"

string gSrcRootPath;
static string kClassInterfPrefix = "XX";
static int kMethodParamMaxLen = 15;
static int kMethodParamMaxParamsSingleLine = 3;
static int kMethodBodyMaxLines = 500;

namespace CodingStyle
{
    void CodingStyleASTVisitor::setContext(ASTContext &context)
    {
        this->context = &context;
    }
    string CodingStyleASTVisitor::pureSelFromSelector(string selector){
        string pureSel = string(selector);
        if(selector.find("@selector(")!=string::npos){
            pureSel = selector.substr(string("@selector(").length(),selector.length()-string("@selector(").length()-1);
        }
        return pureSel;
    }
    bool CodingStyleASTVisitor::VisitDecl(Decl *decl) {
        string filename = this->context->getSourceManager().getFilename(decl->getSourceRange().getBegin()).str();
        if(isa<ObjCInterfaceDecl>(decl)){
            ObjCInterfaceDecl *interfDecl = (ObjCInterfaceDecl*)decl;
            if(filename.find(gSrcRootPath)!=string::npos)
                this->checkInterfaceDecl(interfDecl);
        }
        if(isa<ObjCImplDecl>(decl)){
            ObjCImplDecl *interDecl = (ObjCImplDecl*)decl;
            objcClsImpl = interDecl->getNameAsString();
        }
        if(isa<ObjCPropertyDecl>(decl)){
            ObjCPropertyDecl *propertyDecl = (ObjCPropertyDecl *)decl;
            if(filename.find(gSrcRootPath)!=string::npos)
                this->checkPropertyDecl(propertyDecl);
        }
        if(isa<ObjCMethodDecl>(decl)){
            ObjCMethodDecl *methodDecl = (ObjCMethodDecl *)decl;
            if(filename.find(gSrcRootPath)!=string::npos)
                this->checkMethodDecl(methodDecl);
        }
        if(isa<EnumDecl>(decl)){
            if(filename.find(gSrcRootPath)!=string::npos)
                this->checkEnumDecl((EnumDecl *)decl);
        }
        if(isa<ObjCIvarDecl>(decl)){
            if(filename.find(gSrcRootPath)!=string::npos)
                this->checkIvarDecl((ObjCIvarDecl *)decl);
        }
        return true;
    }
    bool CodingStyleASTVisitor::VisitStmt(Stmt *s) {
        string curFilename = context->getSourceManager().getFilename(s->getSourceRange().getBegin()).str();
        //If语句
        if(isa<IfStmt>(s)){
            this->checkIfStmt((IfStmt *)s);
        }
        if(isa<ObjCMessageExpr>(s))
        {
            this->checkMessageExpr((ObjCMessageExpr *)(s));
        }
        return true;
    }
    void CodingStyleASTVisitor::checkInterfaceDecl(ObjCInterfaceDecl* declaration){
        StringRef name = declaration->getName();
        DiagnosticsEngine &diagEngine = context->getDiagnostics();
        SourceLocation location = declaration->getLocation();
        if (name.find(kClassInterfPrefix)!=0) {
            string tempName = string(kClassInterfPrefix)+name.str();
            StringRef replacement(tempName);
            SourceLocation nameStart = declaration->getLocation();
            SourceLocation nameEnd = nameStart.getLocWithOffset(name.size()-1);
            FixItHint fixItHint = FixItHint::CreateReplacement(SourceRange(nameStart, nameEnd), replacement);
            unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "Class name should start with prefix XX");
            diagEngine.Report(location, diagID).AddFixItHint(fixItHint);
            return;
        }
        if (!name.str().compare(kClassInterfPrefix)) {
            string tempName = kClassInterfPrefix+string("ClassName");
            StringRef replacement(tempName);
            SourceLocation nameStart = declaration->getLocation();
            SourceLocation nameEnd = nameStart.getLocWithOffset(name.size()-1);
            FixItHint fixItHint = FixItHint::CreateReplacement(SourceRange(nameStart, nameEnd), replacement);
            unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "Class name should not be XX");
            diagEngine.Report(location, diagID).AddFixItHint(fixItHint);
            return;
        }
        char tmpCh = name.str().at(kClassInterfPrefix.length());
        if(islower(tmpCh)){
            string tempName = string(name.str());
            tempName.replace(kClassInterfPrefix.length(), 1, string(1,toupper(tmpCh)));
            StringRef replacement(tempName);
            SourceLocation nameStart = declaration->getLocation();
            SourceLocation nameEnd = nameStart.getLocWithOffset(name.size()-1);
            FixItHint fixItHint = FixItHint::CreateReplacement(SourceRange(nameStart, nameEnd), replacement);
            unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "Class name should start with upper case.");
            diagEngine.Report(location, diagID).AddFixItHint(fixItHint);
            return;
        }
        if(name.find("_")!=string::npos) {
            string tempName(name.str());
            remove_char_from_string(tempName,'_');
            StringRef replacement(tempName);
            SourceLocation nameStart = declaration->getLocation();
            SourceLocation nameEnd = nameStart.getLocWithOffset(name.size()-1);
            FixItHint fixItHint = FixItHint::CreateReplacement(SourceRange(nameStart, nameEnd), replacement);
            unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "Class name should not contains '_'");
            diagEngine.Report(location, diagID).AddFixItHint(fixItHint);
            return;
        }
    }
    void CodingStyleASTVisitor::checkPropertyDecl(clang::ObjCPropertyDecl *propDecl){
        ObjCPropertyDecl::PropertyAttributeKind attributeKind = propDecl->getPropertyAttributes();
        DiagnosticsEngine &diagEngine = context->getDiagnostics();
        SourceLocation location = propDecl->getLocation();
        if(!(attributeKind & ObjCPropertyDecl::OBJC_PR_nonatomic)){
            unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "Are you sure to use atomic which might reduce the performance.");
            diagEngine.Report(location, diagID);
        }
        if(propDecl->getTypeSourceInfo()){
            string typeStr = propDecl->getType().getAsString();
            if(typeStr.find("NSString *")!=string::npos){
                if(attributeKind & ObjCPropertyDecl::OBJC_PR_strong){
                    unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "NSString *should use the attributes copy instead of strong.");
                    diagEngine.Report(location, diagID);
                    return;
                }
            }
            if(!typeStr.compare("int")){
                unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "Use the built-in NSInteger instead of int.");
                diagEngine.Report(location, diagID);
                return;
            }
            //Delegate
            if(typeStr.find("<")!=string::npos && typeStr.find(">")!=string::npos){
                string typeSrcCode;
                typeSrcCode.assign(context->getSourceManager().getCharacterData(propDecl->getSourceRange().getBegin()),propDecl->getSourceRange().getEnd().getRawEncoding()-propDecl->getSourceRange().getBegin().getRawEncoding());
                if(!(attributeKind & ObjCPropertyDecl::OBJC_PR_weak)){
                    unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "Delegate should be declared as weak.");
                    diagEngine.Report(location, diagID);
                }
                string replaceName = formatPropertyDelegateType(typeSrcCode);
                if(replaceName.compare(typeSrcCode)){
                    StringRef replacement(replaceName);
                    SourceLocation nameStart = propDecl->getSourceRange().getBegin();
                    SourceLocation nameEnd = nameStart.getLocWithOffset(typeSrcCode.length()-1);
                    FixItHint fixItHint = FixItHint::CreateReplacement(SourceRange(nameStart, nameEnd), replacement);
                    unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "Class delegate should be like id<XXX> delegate");
                    diagEngine.Report(location, diagID).AddFixItHint(fixItHint);
                }
            }
        }
    }
    void CodingStyleASTVisitor::checkMethodDecl(ObjCMethodDecl *methodDecl){
        LangOptions LangOpts;
        LangOpts.ObjC2 = true;
        PrintingPolicy Policy(LangOpts);
        DiagnosticsEngine &diagEngine = context->getDiagnostics();
        SourceLocation location = methodDecl->getSourceRange().getBegin();
        //检查方法名格式
        if(!methodDecl->hasBody() && methodDecl->param_size()>kMethodParamMaxParamsSingleLine){
            string methodDeclStr;
            methodDeclStr.assign(context->getSourceManager().getCharacterData(methodDecl->getSourceRange().getBegin()),methodDecl->getSourceRange().getEnd().getRawEncoding()-methodDecl->getSourceRange().getBegin().getRawEncoding());
            string replaceName = formatObjcMethodName(methodDeclStr);
            if(replaceName.compare(methodDeclStr)){
                StringRef replacement(replaceName);
                SourceLocation nameStart = methodDecl->getSourceRange().getBegin();
                SourceLocation nameEnd = nameStart.getLocWithOffset(methodDeclStr.length()-1);
                FixItHint fixItHint = FixItHint::CreateReplacement(SourceRange(nameStart, nameEnd), replacement);
                unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "Method with params over 3 should be seperated into lines and aligned by comma.");
                diagEngine.Report(location, diagID).AddFixItHint(fixItHint);
            }
        }
        //检查函数参数
        for(ObjCMethodDecl::param_iterator pi = methodDecl->param_begin();pi!=methodDecl->param_end();pi++){
            ParmVarDecl *varDecl = *pi;
            string paramName = varDecl->getNameAsString();
            if(paramName.length()>kMethodParamMaxLen){
                string tempName(varDecl->getNameAsString());
                StringRef replacement(paramName.substr(0,kMethodParamMaxLen));
                SourceLocation nameStart = varDecl->getLocation();
                SourceLocation nameEnd = nameStart.getLocWithOffset(paramName.size()-1);
                FixItHint fixItHint = FixItHint::CreateReplacement(SourceRange(nameStart, nameEnd), replacement);
                unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "Single param length should not over 15.");
                diagEngine.Report(location, diagID).AddFixItHint(fixItHint);
                return;
            }
            string typeStr = varDecl->getType().getAsString();
            if(!typeStr.compare("int")){
                StringRef replacement("NSInteger");
                SourceLocation nameStart = varDecl->getTypeSpecStartLoc();
                SourceLocation nameEnd = nameStart.getLocWithOffset(typeStr.size()-1);
                FixItHint fixItHint = FixItHint::CreateReplacement(SourceRange(nameStart, nameEnd), replacement);
                unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "Use the built-in NSInteger instead of int.");
                diagEngine.Report(location, diagID).AddFixItHint(fixItHint);
                return;
            }
        }
        //检查函数体
        if(methodDecl->hasBody()){
            objcIsInstanceMethod = methodDecl->isInstanceMethod();
            objcSelector = methodDecl->getSelector().getAsString();
            Stmt *methodBody = methodDecl->getBody();
            string srcCode;
            srcCode.assign(context->getSourceManager().getCharacterData(methodBody->getSourceRange().getBegin()),methodBody->getSourceRange().getEnd().getRawEncoding()-methodBody->getSourceRange().getBegin().getRawEncoding()+1);
            vector<string> lines = split(srcCode, '\n');
            if(lines.size()>kMethodBodyMaxLines){
                unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "Single method should not have body over 500 lines");
                diagEngine.Report(location, diagID);
                return;
            }
        }
    }
    void CodingStyleASTVisitor::checkIvarDecl(ObjCIvarDecl *ivarDecl){
        DiagnosticsEngine &diagEngine = context->getDiagnostics();
        SourceLocation location = ivarDecl->getLocation();
        string ivarName = ivarDecl->getNameAsString();
        if(ivarName.find("_")!=0){
            StringRef replacement(string("_")+ivarName);
            SourceLocation nameStart = ivarDecl->getLocation();
            SourceLocation nameEnd =  nameStart.getLocWithOffset(ivarName.size()-1);
            FixItHint fixItHint = FixItHint::CreateReplacement(SourceRange(nameStart, nameEnd), replacement);
            unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "Use _ as prefix for Ivar in Objective-C.");
            diagEngine.Report(location, diagID).AddFixItHint(fixItHint);
        }
    }
    void CodingStyleASTVisitor::checkEnumDecl(EnumDecl *enumDecl){
        LangOptions LangOpts;
        LangOpts.ObjC2 = true;
        PrintingPolicy Policy(LangOpts);
        DiagnosticsEngine &diagEngine = context->getDiagnostics();
        SourceLocation location = enumDecl->getLocStart();
        
        string sExpr;
        raw_string_ostream rsoExpr(sExpr);
        enumDecl->print(rsoExpr,Policy);
        string oriExpr = rsoExpr.str();
        remove_blank(oriExpr);
        if(oriExpr.find("typedefenum")==0 || oriExpr.find("enum")==0){
            unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "Use NS_ENUM/NS_OPTIONS instead of enum.");
            diagEngine.Report(location, diagID);
        }
    }
    void CodingStyleASTVisitor::checkMessageExpr(ObjCMessageExpr *objcExpr){
        LangOptions LangOpts;
        LangOpts.ObjC2 = true;
        PrintingPolicy Policy(LangOpts);
        DiagnosticsEngine &diagEngine = context->getDiagnostics();
        SourceLocation location = objcExpr->getLocStart();
        
        string sExpr;
        raw_string_ostream rsoExpr(sExpr);
        objcExpr->printPretty(rsoExpr, 0, Policy);
        string oriExpr = rsoExpr.str();

        //检查消息发送格式
        if(objcExpr->getNumArgs()>kMethodParamMaxParamsSingleLine){
            string messageStr;
            messageStr.assign(context->getSourceManager().getCharacterData(objcExpr->getSourceRange().getBegin()),objcExpr->getSourceRange().getEnd().getRawEncoding()-objcExpr->getSourceRange().getBegin().getRawEncoding());
            string replaceName = formatObjcMethodName(messageStr);
            if(replaceName.compare(messageStr)){
                StringRef replacement(replaceName);
                SourceLocation nameStart = objcExpr->getSourceRange().getBegin();
                SourceLocation nameEnd = nameStart.getLocWithOffset(messageStr.length()-1);
                FixItHint fixItHint = FixItHint::CreateReplacement(SourceRange(nameStart, nameEnd), replacement);
                unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "Method with params over 3 should be seperated into lines and aligned by comma.");
                diagEngine.Report(location, diagID).AddFixItHint(fixItHint);
            }
        }
        remove_blank(oriExpr);
        if((objcSelector.find("init")==0 || !objcSelector.compare("dealloc")) && oriExpr.find("[self")==0){
            unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "Don't send message to self in init/dealloc.");
            diagEngine.Report(location, diagID);
        }
    }
    void CodingStyleASTVisitor::checkIfStmt(IfStmt* is){
        DiagnosticsEngine &diagEngine = context->getDiagnostics();
        SourceLocation location = is->getIfLoc();
        LangOptions LangOpts;
        LangOpts.ObjC2 = true;
        PrintingPolicy Policy(LangOpts);
        Expr *cond = is->getCond();
        Stmt * thenST = is->getThen();
        string sExpr;
        raw_string_ostream rsoExpr(sExpr);
        thenST->printPretty(rsoExpr, 0, Policy);
        string body = rsoExpr.str();
        remove_blank(body);
        //检查Body
        if(!body.compare("") || !body.compare("{}")){
            unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "Don't use empty body in If Statement.");
            diagEngine.Report(location, diagID);
        }
        bool isNot = false;
        int flag = -1;
        //检查一元表达式
        if(isa<UnaryOperator>(cond)){
            UnaryOperator *uo = (UnaryOperator *)cond;
            if(uo->getOpcode()==UO_LNot || uo->getOpcode()==UO_Not){
                isNot = true;
                cond = uo->getSubExpr();
            }
        }
        if(isa<IntegerLiteral>(cond)){
            IntegerLiteral *il = (IntegerLiteral*)cond;
            flag =il->getValue().getBoolValue();
        }
        else if(isa<CharacterLiteral>(cond)){
            CharacterLiteral *cl = (CharacterLiteral*)cond;
            flag =cl->getValue();
        }
        else if(isa<FloatingLiteral>(cond)){
            FloatingLiteral *fl = (FloatingLiteral*)cond;
            flag =fl->getValue().isNonZero();
        }
        if(flag != -1){
            flag = flag?1:0;
            if(isNot)
                flag = 1-flag;
            if(flag){
                unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "Body will certainly be executed when condition true.");
                diagEngine.Report(location, diagID);
            }
            else{
                unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "Body will never be executed when condition false.");
                diagEngine.Report(location, diagID);
            }
        }
        //二元表达式
        if(isa<BinaryOperator>(cond)){
            BinaryOperator *bo = (BinaryOperator*)cond;
            if(bo->isAssignmentOp() ){
                string sCond;
                raw_string_ostream rsoCond(sCond);
                cond->printPretty(rsoCond, 0, Policy);
                unsigned diagID = diagEngine.getCustomDiagID(DiagnosticsEngine::Warning, "Don't use assignment-expr in If Statement.");
                diagEngine.Report(location, diagID);
            }
        }
    }
    void CodingStyleASTConsumer::HandleTranslationUnit(ASTContext &context){
        visitor.setContext(context);
        visitor.TraverseDecl(context.getTranslationUnitDecl());
    }
    unique_ptr<ASTConsumer> CodingStyleASTAction::CreateASTConsumer(CompilerInstance &Compiler,llvm::StringRef InFile)
    {
        return unique_ptr<CodingStyleASTConsumer>(new CodingStyleASTConsumer);
    }
    bool CodingStyleASTAction::ParseArgs(const CompilerInstance &CI, const std::vector<std::string>& args) {
        size_t cnt = args.size();
        if(cnt == 1){
            gSrcRootPath = args.at(0);
        }
        return true;
    }
}

static clang::FrontendPluginRegistry::Add<CodingStyle::CodingStyleASTAction>
X("ClangCodingStylePlugin", "ClangCodingStylePlugin");
