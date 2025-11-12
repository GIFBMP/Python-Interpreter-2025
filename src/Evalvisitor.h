#pragma once
#include "ParserRuleContext.h"
#include "Python3Parser.h"
#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H


#include "Python3ParserBaseVisitor.h"
#include <map>
#include <string>
#include "int2048.h"

using sjtu::int2048;

class EvalVisitor : public Python3ParserBaseVisitor {
	// TODO: override all methods of Python3ParserBaseVisitor
    //todo: format_string or_test and_test not_test comp add_sub mul_div 
    /*
    none: short(0)
    return: short(1)
    continue: short(2)
    break: short(3)
    */
    #define NoneState short(0)
    #define ReturnState short(1)
    #define ContinueState short(2)
    #define BreakState short(3)
    struct variable {
        std::string id;
        variable(std::string id) : id(id) {}
    } ;
    std::unordered_map<std::string, int> funcs;
    struct functions {
        std::string id;
        Python3Parser::FuncdefContext *ctx;
        std::vector<variable> paras;
        functions() {paras.clear();}
    } func_information[2010];
    int func_count = 0;
    struct returnvals {
        std::any x;
        returnvals(std::any x) : x(x) {}
    } ;
    struct Scope {
        struct varspace {
            std::unordered_map<std::string, std::any> val;
            int pr;
        } a[1010];
        int nw = 1, cnt = 1;
        Scope() {
            cnt = 1; nw = 1;
        }
        void getback() {
            if(nw) nw = a[nw].pr;
        }
        void newscope() {
            cnt++; a[cnt].val.clear(), a[cnt].pr = nw;
            nw = cnt;
        }
        void revise(variable var, std::any v) {
            bool fl = 0;
            for (int i = nw; i; i = a[i].pr) {
                if (a[i].val.count(var.id)) {
                    a[i].val[var.id] = v; fl = 1;
                    break;
                }
            }
            if (!fl) {
                a[nw].val[var.id] = v;
            }
        }
        bool findvar(variable var) {
            return a[nw].val.count(var.id);
        }
        std::any getvar(variable var) {
            for (int i = nw; i; i = a[i].pr) {
                if (a[i].val.count(var.id))
                    return a[i].val[var.id];
            }
            return NoneState;
        }
    } scope ;
    
    template <class T> 
    bool comp_simpl(T a, T b, std::string op) {
        if (op == "<") return a < b;
        if (op == ">") return a > b;
        if (op == "<=") return a <= b;
        if (op == ">=") return a >= b;
        if (op == "==") return a == b;
        if (op == "!=") return a != b;
        return false;
    }
    std::any trans_into_val (std::any x) {
        auto tmp = std::any_cast<returnvals>(&x);
        if (tmp) return trans_into_val((*tmp).x);
        auto v = std::any_cast<variable>(&x);
        if (v) return scope.getvar(*v);
        else return x;
    }
    void var_trans(std::any x, std::any y, std::any &tx, std::any &ty) {
        x = trans_into_val(x), y = trans_into_val(y);
        auto vstr1 = std::any_cast<std::string>(&x);
        auto vstr2 = std::any_cast<std::string>(&y);
        if (vstr1 || vstr2) return;
        auto vdb1 = std::any_cast<double>(&x);
        auto vdb2 = std::any_cast<double>(&y);
        auto vb1 = std::any_cast<bool>(&x);
        auto vb2 = std::any_cast<bool>(&y);
        auto vll1 = std::any_cast<int2048>(&x);
        auto vll2 = std::any_cast<int2048>(&y);
        if (vdb1 || vdb2) {
            double t1 , t2;
            if (vdb1 && vdb2) t1 = (*vdb1), t2 = (*vdb2);
            else if (vdb1) {
                t1 = (*vdb1);
                if (vb2) t2 = double(*vb2);
                else t2 = (double)transtoll(*vll2);
            }
            else if (vdb2) {
                t2 = (*vdb2);
                if (vb1) t1 = double(*vb1);
                else t1 = (double)transtoll(*vll1);
            }
            tx = t1, ty = t2;
            return;
        }
        int2048 t1 , t2;
        if (vb1) t1 = (long long)(*vb1);
        else t1 = (*vll1);
        if (vb2) t2 = (long long)(*vb2);
        else t2 = (*vll2);
        tx = t1, ty = t2;
        return;
    }
    bool comp(std::any x, std::any y, std::string op) {
        std::any t1, t2;
        var_trans(x, y, t1, t2);
        //type_x=type_y is guaranteed
        auto vdb1 = std::any_cast<double>(&t1);
        if (vdb1) {
            auto vdb2 = std::any_cast<double>(&t2);
            return comp_simpl(*vdb1, *vdb2, op);
        }
        auto vll1 = std::any_cast<int2048>(&t1);
        if (vll1) {
            auto vll2 = std::any_cast<int2048>(&t2);
            return comp_simpl(*vll1, *vll2, op);
        }
        auto vstr1 = std::any_cast<std::string>(&x);
        if (vstr1) {
            auto vstr2 = std::any_cast<std::string>(&y);
            return comp_simpl(*vstr1, *vstr2, op);
        }
        return false;
    }
    std::any addorsub(std::any x, std::any y, std::string op) {
        std::any t1, t2;
        var_trans(x, y, t1, t2);
        //valid
        auto vdb1 = std::any_cast<double>(&t1);
        if (vdb1) {
            auto vdb2 = std::any_cast<double>(&t2);
            if (op == "+") return (*vdb1) + (*vdb2);
            else return (*vdb1) - (*vdb2);
        }
        auto vll1 = std::any_cast<int2048>(&t1);
        if (vll1) {
            auto vll2 = std::any_cast<int2048>(&t2);
            if (op == "+") return (*vll1) + (*vll2);
            else return (*vll1) - (*vll2);
        }
        auto vstr1 = std::any_cast<std::string>(&t1);
        if (vstr1) {
            auto vstr2 = std::any_cast<std::string>(&t2);
            if (vstr2 && op == "+") return (*vstr1) + (*vstr2);
        }
        return 0;
    }
    std::any muldivmod(std::any x, std::any y, std::string op) {
        std::any t1, t2;
        var_trans(x, y, t1, t2);
        //valid
        auto vdb1 = std::any_cast<double>(&t1);
        if (vdb1) {
            auto vdb2 = std::any_cast<double>(&t2);
            if (op == "*") return (*vdb1) * (*vdb2);
            else if (op == "/") return (*vdb1) / (*vdb2);
            else if (op == "//") {
                long long ret = (long long)floor((*vdb1) / (*vdb2));
                return (int2048)ret;
            }
        }
        auto vll1 = std::any_cast<int2048>(&t1);
        if (vll1) {
            auto vll2 = std::any_cast<int2048>(&t2);
            if (op == "*") return (*vll1) * (*vll2);
            else if (op == "/") {
                long long v1 = transtoll(*vll1), v2 = transtoll(*vll2);
                return 1.0 * v1 / v2;
            }
            else if (op == "//") return (*vll1) / (*vll2);
            else if (op == "%") return (*vll1) % (*vll2);
        }
        auto vstr1 = std::any_cast<std::string>(&t1);
        if (vstr1) {
            auto vll2 = std::any_cast<int2048>(&t2);
            if (vll2 && op == "*") {
                std::string ret = "";
                long long cnt = transtoll(*vll2);
                for (int i = 0; i < cnt; i++) ret += (*vstr1);
                return ret;
            }
        }
        return 0;
    }
    short getsta (std::any x) {
        auto v = std::any_cast<short>(&x);
        if (v) return *v;
        auto ret = std::any_cast<returnvals>(&x);
        if (ret) return ReturnState;
        return NoneState;
    }
    bool checkval(std::any x) {
        x = trans_into_val(x);
        auto vb = std::any_cast<bool>(&x);
        if (vb) return (*vb);
        auto vll = std::any_cast<int2048>(&x);
        if (vll) return ((*vll) != 0);
        auto vdb = std::any_cast<double>(&x);
        if (vdb) return ((*vdb) != 0);
        auto vstr = std::any_cast<std::string>(&x);
        if (vstr) return ((*vstr) != "");
        return false;
    }
    void assign(std::any x, std::any y) {//x<=y
        auto xlist = std::any_cast<std::vector<std::any> >(&x);
        if (xlist) {
            int len = (*xlist).size();
            auto ylist = std::any_cast<std::vector<std::any> >(&y);
            for (int i = 0; i < len; i++)
                assign((*xlist)[i], (*ylist)[i]);
            return;
        }
        auto id = std::any_cast<variable>(&x);
        y = trans_into_val(y);
        scope.revise(*id, y);
    }
    virtual std::any visitArglist(Python3Parser::ArglistContext *ctx) override {
        int len = (ctx->argument()).size();
        if (len == 0) return NoneState;
        if (len == 1) return visit(ctx->argument(0));
        std::vector<std::any> ret; ret.resize(len);
        for (int i = 0; i < len; i++) ret[i] = visit(ctx->argument(i));
        return ret;
    }
    virtual std::any visitArgument(Python3Parser::ArgumentContext *ctx) override {
        if (ctx->ASSIGN()) {
            std::any t1 = visit(ctx->test(1));
            std::any t0 = visit(ctx->test(0));
            assign(t0, t1);
            return t0;
        }
        return visit(ctx->test(0));
    }
    virtual std::any visitTestlist(Python3Parser::TestlistContext *ctx) override {
        int len = (ctx->test()).size();
        if (len == 0) return NoneState;
        if (len == 1) return visit(ctx->test(0));
        std::vector<std::any> ret; ret.resize(len);
        for (int i = 0; i < len; i++) ret[i] = visit(ctx->test(i));
        return ret;
    }
    virtual std::any visitSuite(Python3Parser::SuiteContext *ctx) override {
        int len = (ctx->stmt()).size();
        for (int i = 0; i < len; i++) {
            std::any nw = visit(ctx->stmt(i));
            if (getsta(nw)) return nw;
        }
        return NoneState;
    }
    virtual std::any visitIf_stmt(Python3Parser::If_stmtContext *ctx) override {
        int len = (ctx->test()).size();
        for (int i = 0; i < len; i++) {
            std::any nw = visit(ctx->test(i));
            if (checkval(nw)) {
                return visit(ctx->suite(i));
            }
        }
        if (ctx->ELSE()) {
            return visit(ctx->suite(len));
        }
        return NoneState;
    }
    virtual std::any visitWhile_stmt(Python3Parser::While_stmtContext *ctx) override {
        while (1) {
            std::any nw = visit(ctx->test());
            if (!checkval(nw)) return NoneState;
            std::any sta = visit(ctx->suite());
            short tmp = getsta(sta);
            if (tmp == BreakState) break;
            if (tmp == ReturnState) return sta;
        }
        return NoneState;
    }
    virtual std::any visitBreak_stmt(Python3Parser::Break_stmtContext *ctx) override {
        return BreakState;
    }
    virtual std::any visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) override {
        return ContinueState;
    }
    virtual std::any visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) override {
        if (ctx->testlist()) {
            return returnvals(visit(ctx->testlist()));
        }
        return ReturnState;
    }
    virtual std::any visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) override {
        //todo
        if (ctx->augassign()) {
            std::string op = ctx->augassign()->getText();
            std::any t0 = visit(ctx->testlist(0));
            std::any t1 = visit(ctx->testlist(1));
            std::any tmp ;
            if (op == "+=") tmp = addorsub(t0, t1, "+");
            else if (op == "-=") tmp = addorsub(t0, t1, "-");
            else if (op == "*=") tmp = muldivmod(t0, t1, "*");
            else if (op == "/=") tmp = muldivmod(t0, t1, "/");
            else if (op == "//=") tmp = muldivmod(t0, t1, "//");
            else if (op == "%=") tmp = muldivmod(t0, t1, "%"); 
            assign(t0, tmp);
            return t0;
        }
        int len = (ctx->testlist()).size();
        if (len == 1) return visit(ctx->testlist(0));
        std::any ls, nw;
        ls = visit(ctx->testlist(len - 1));
        for (int i = len - 2; i >= 0; i--) {
            nw = visit(ctx->testlist(i));
            assign(ls, nw);
            ls = nw;
        }
        return ls;
    }
    std::string getstring(std::any x) {
        auto vctr = std::any_cast<std::vector<std::any> >(&x);
        if (vctr) {
            std::string ret = "";
            int len = (*vctr).size();
            for (int i = 0; i < len; i++) {
                ret += getstring((*vctr)[i]);
                if (i < len - 1) ret += ' ';
            }
            return ret;
        }
        x = trans_into_val(x);
        auto vstr = std::any_cast<std::string>(&x);
        if (vstr) return *vstr;
        auto vnone = std::any_cast<short>(&x);
        if (vnone) return "None";
        auto vll = std::any_cast<int2048>(&x);
        if (vll) return transtostring(*vll);
        auto vdb = std::any_cast<double>(&x);
        if (vdb) {
            std::string ret = "";
            double v = (*vdb);
            if (v < 0) v = -v, ret += "-";
            long long x = (long long)(v);
            for (; x; x /= 10) ret = (char)(x % 10 + '0') + ret;
            ret = ret + '.';
            double tmp = v - x;
            for (int i = 1 ; i < 6; i++, tmp *= 10) {
                long long nw = (long long)tmp;
                ret = ret + (char)(nw % 10 + '0');
            }
            return ret;
        }
        auto vb = std::any_cast<bool>(&x);
        if ((*vb) == false) return "False";
        else return "True";
        return "None";
    }
    virtual std::any visitFormat_string(Python3Parser::Format_stringContext *ctx) override {
        std::string ret = "";
        int len_literal = (ctx->FORMAT_STRING_LITERAL()).size();
        int len_test = (ctx->testlist()).size();
        int i = 0, j = 0 ;
        while (i < len_literal && j < len_test) {
            int p1 = ctx->FORMAT_STRING_LITERAL(i)->getSymbol()->getTokenIndex();
            int p2 = ctx->OPEN_BRACE(j)->getSymbol()->getTokenIndex();
            if (p1 < p2) {
                std::string tmp = ctx->FORMAT_STRING_LITERAL(i)->getText();
                int len = tmp.size();
                for (int k = 0; k < len; k++) {
                    ret += tmp[k];
                    if (tmp[k] == '{' || tmp[k] == '}') k++;
                }
                i++;
            }
            else {
                std::any tmp = visit(ctx->testlist(j));
                ret += getstring(tmp);
                j++;
            }
        }
        while (i < len_literal) {
            std::string tmp = ctx->FORMAT_STRING_LITERAL(i)->getText();
            int len = tmp.size();
            for (int k = 0; k < len; k++) {
                ret += tmp[k];
                if (tmp[k] == '{' || tmp[k] == '}') k++;
            }
            i++;
        }
        while (j < len_test) {
            std::any tmp = visit(ctx->testlist(j));
            ret += getstring(tmp);
            j++;
        }
        //todo
        return ret;
    }
    virtual std::any visitTrailer(Python3Parser::TrailerContext *ctx) override {
        return visit(ctx->arglist());
    }
    virtual std::any visitFuncdef(Python3Parser::FuncdefContext *ctx) override {
        std::string func_name = ctx->NAME()->getText();
        funcs[func_name] = ++func_count;
        func_information[func_count].id = func_name;
        std::any v = visit(ctx->parameters());
        auto var = std::any_cast<variable>(&v);
        if (var) func_information[func_count].paras.push_back(*var);
        auto lst = std::any_cast<std::vector<std::any> >(&v);
        if (lst) {
            int len = (*lst).size();
            for (int i = 0; i < len; i++) {
                std::any nw = (*lst)[i];
                auto para = std::any_cast<variable>(&nw);
                func_information[func_count].paras.push_back (*para);
            }
        }
        return visitChildren(ctx);
    }
    virtual std::any visitAtom_expr(Python3Parser::Atom_exprContext *ctx) override {
        if (ctx->trailer()) {
            //function todo
            std::string func_name = ctx->atom()->NAME()->getText();
            std::any x = visit(ctx->trailer());
            if (func_name == "print") {
                std::cout << getstring(x) << '\n';
                return NoneState;
            }
            else if (func_name == "int") {
                auto vdb = std::any_cast<double>(&x);
                if (vdb) return (int2048)((long long)(*vdb));
                auto vll = std::any_cast<int2048>(&x);
                if (vll) return (*vll);
                auto vb = std::any_cast<bool>(&x);
                if (vb) return (int2048)(*vb);
                auto vstr = std::any_cast<std::string>(&x);
                if (vstr) return (int2048)(*vstr);
                return NoneState;
            }
            else if (func_name == "float") {
                auto vdb = std::any_cast<double>(&x);
                if (vdb) return (*vdb);
                auto vll = std::any_cast<int2048>(&x);
                if (vll) return (double)transtoll(*vll);
                auto vb = std::any_cast<bool>(&x);
                if (vb) return (double)(*vb);
                auto vstr = std::any_cast<std::string>(&x);
                if (vstr) {
                    int len = (*vstr).size();
                    double ret = 0, nw = 1;
                    bool fl = false;
                    for (int i = 0; i < len; i++) {
                        if ((*vstr)[i] == '.') fl = true;
                        ret = ret * 10.0 + (*vstr)[i] - '0';
                        if (fl) ret /= 10.0;
                    }
                    return ret;
                }
                return NoneState;
            }
            else if (func_name == "str") {
                return getstring(x);
            }
            else if (func_name == "bool") {
                auto vdb = std::any_cast<double>(&x);
                if (vdb) return ((*vdb) != 0) ? true : false;
                auto vll = std::any_cast<int2048>(&x);
                if (vll) return ((*vll) != 0) ? true : false;
                auto vb = std::any_cast<bool>(&x);
                if (vb) return (*vb);
                auto vstr = std::any_cast<std::string>(&x);
                if (vstr) return ((*vstr) != "") ? true : false;
            }
            else {
                int pos = funcs[func_name];
                scope.newscope();
                Python3Parser::FuncdefContext *ctx = func_information[func_count].ctx;
                auto lst = std::any_cast<std::vector<std::any> >(&x);
                if (lst) {
                    int len = (*lst).size();
                    for (int i = 0; i < len; i++) {
                        variable nw = func_information[func_count].paras[i];
                        if (!scope.findvar(nw)) assign(nw , (*lst)[i]);
                    }
                }
                std::any ret = visit(ctx->suite());
                scope.getback();
                return ret;
            }
        }
        return visit(ctx->atom());
    }
    virtual std::any visitFactor(Python3Parser::FactorContext *ctx) override {
        if (ctx->atom_expr()) return visit(ctx->atom_expr());
        if (ctx->MINUS()) return addorsub(0, visit(ctx->factor()), "-");
        return visit(ctx->factor());
    }
    virtual std::any visitTerm(Python3Parser::TermContext *ctx) override {
        if (!(ctx->muldivmod_op()).size()) return visit(ctx->factor(0));
        int op_len = (ctx->factor()).size();
        std::any ls = visit (ctx->factor(0)), nw;
        for (int i = 1; i < op_len; i++) {
            std::string op = ctx->muldivmod_op(i - 1)->getText();
            nw = visit(ctx->factor(i));
            ls = muldivmod(ls, nw, op);
        }
        return ls;
    }
    virtual std::any visitArith_expr(Python3Parser::Arith_exprContext *ctx) override {
        if (!(ctx->addorsub_op()).size()) return visit(ctx->term(0));
        int op_len = (ctx->term()).size();
        std::any ls = visit(ctx->term(0)), nw;
        for (int i = 1; i < op_len; i++) {
            std::string op = ctx->addorsub_op(i - 1)->getText();
            nw = visit(ctx->term(i));
            ls = addorsub(ls, nw, op);
        }
        return ls;
    }
    virtual std::any visitComparison(Python3Parser::ComparisonContext *ctx) override {
        if (!(ctx->comp_op()).size()) return visit(ctx->arith_expr(0));
        int arith_len = (ctx->arith_expr()).size();
        std::any ls = visit(ctx->arith_expr(0)), nw;
        //ls = trans_into_val(ls);
        for (int i = 1; i < arith_len; i++) {
            std::string op = ctx->comp_op(i - 1)->getText();
            nw = visit(ctx->arith_expr(i));
            //nw = trans_into_val(nw);
            if (!comp (ls ,nw , op)) return false;
            ls = nw;
        }
        return true;
    }
    virtual std::any visitNot_test(Python3Parser::Not_testContext *ctx) override {
        if (!(ctx->NOT())) {
            if (ctx->comparison()) return visit(ctx->comparison());
            return visit(ctx->not_test());
        }
        std::any ret ;
        if (ctx->not_test()) ret = visit(ctx->not_test());
        else ret = visit(ctx->comparison());
        ret = trans_into_val(ret);
        if (auto vbool = std::any_cast<bool>(&ret);
            vbool) {
                if ((*vbool) == false) return true;
                else return false;
            }
        else if(auto vll = std::any_cast<int2048>(&ret);
                vll) {
                    if ((*vll) == 0) return true;
                    else return false;
                }
        return false;
    }
    virtual std::any visitAnd_test(Python3Parser::And_testContext *ctx) override {
        if (!((ctx->AND()).size())) return visit(ctx->not_test(0));
        int not_len = (ctx->not_test()).size();
        for (int i = 0; i < not_len; i++) {
            std::any nw = visit(ctx->not_test(i));
            if (!checkval(nw)) return false;
        }
        return true;
    }
    virtual std::any visitOr_test(Python3Parser::Or_testContext *ctx) override {
        if (!((ctx->OR()).size())) return visit(ctx->and_test(0));
        int and_len = (ctx->and_test()).size() ;
        for (int i = 0; i < and_len; i++) {
            std::any nw = visit(ctx->and_test(i));
            if (checkval(nw)) return true;
        }
        return false;
    }
    virtual std::any visitTest(Python3Parser::TestContext *ctx) override {
        return visit(ctx->or_test());
    }
    virtual std::any visitAtom(Python3Parser::AtomContext *ctx) override {
        //todo: format_string 
        if (ctx->TRUE()) return true;
        if (ctx->FALSE()) return false;
        if (ctx->NAME()) {
            return variable(ctx->NAME()->getText());
        }
        if (ctx->NUMBER()) {
            std::string str = ctx->NUMBER()->getText();
            int len = str.size(); bool fl = false;
            for (int i = 0; i < len; i++)
                if (str[i] == '.') fl = true;
            if (fl) {
                auto ret = visit(ctx->NUMBER());
                return std::any_cast<double>(ret);
            }
            else {
                sjtu::int2048 ret = str;
                return ret;
            }
        }
        if (ctx->format_string()) {
            return visit(ctx->format_string());
        }
        if ((ctx->STRING()).size()) {
            std::string str = "";
            int len = (ctx->STRING()).size();
            for (int i = 0; i < len; i++) {
                std::string tmp = ctx->STRING(i)->getText();
                int sz = tmp.size();
                if (sz <= 2) continue;
                tmp = tmp.substr(1 , len - 2);
                str += tmp;
            }
            //str = '\"' + str + '\"';
            return str;
        }
        if (ctx->test()) {
            return visit(ctx->test());
        }
        return NoneState;
    }
};


#endif//PYTHON_INTERPRETER_EVALVISITOR_H
