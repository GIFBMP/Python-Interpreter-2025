#pragma once
#include "ParserRuleContext.h"
#include "Python3Parser.h"
#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H


#include "Python3ParserBaseVisitor.h"
#include <map>
#include <string>
#include <iomanip>
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
        std::string id; bool is_all;
        variable(std::string id, bool is_all) : id(id) , is_all(is_all){}
    } ;
    std::unordered_map<std::string, int> funcs;
    struct functions {
        std::string id;
        Python3Parser::FuncdefContext *ctx;
        std::vector<variable> paras;
        std::vector<std::any> inivals;
        functions() {paras.clear();}
    } func_information[2010];
    int func_count = 0;
    struct returnvals {
        std::any x;
        returnvals(std::any x) : x(x) {}
    } ;
    struct Scope {
        std::vector<int> restore;
        struct varspace {
            std::unordered_map<std::string, std::any> val;
            int pr;
        } a[20010];
        int nw = 1, cnt = 1;
        Scope() {
            cnt = 1; nw = 1;
            restore.clear();
        }
        void getback() {
            restore.push_back(nw);
            a[nw].val.clear();
            int pr = a[nw].pr;
            a[nw].pr = 0;
            if(nw) nw = pr;
        }
        void newscope() {
            int tmp;
            if (restore.empty()) tmp = ++cnt;
            else {
                tmp = restore.back();
                restore.pop_back();
            }
            //std::cerr << "newscope_id:" << tmp << '\n';
            a[tmp].val.clear(), a[tmp].pr = nw;
            nw = tmp;
        }
        void revise(variable var, std::any v) {
            //if (a[nw].val.count(var.id)) a[nw].val[var.id] = v;
            //else a[1].val[var.id] = v;
            // if (a[1].val.count(var.id))
            if (var.is_all) {
                bool fl = 0;
                for (int i = nw; i; i = a[i].pr) {
                    if (a[i].val.count(var.id)) {
                        a[i].val[var.id] = v; fl = 1;
                        //if (nw <= 20) std::cerr << "nw:" << nw << ",modified:" << var.id << '\n';
                        break;
                    }
                }
                // if (a[1].val.count(var.id))
                //     a[1].val[var.id] = v, fl = 1;
                if (!fl) {
                    a[nw].val[var.id] = v;
                }
            }
            else a[nw].val[var.id] = v;
        }
        bool findvar(variable var) {
            return a[nw].val.count(var.id);
        }
        std::any getvar(variable var) {
            // if (var.is_all) {
            //     if (a[1].val.count(var.id)) return a[1].val[var.id];
            //     return NoneState;
            // }
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
        auto vlist = std::any_cast<std::vector<std::any> >(&x);
        if (vlist) {
            int len = (*vlist).size();
            for (int i = 0; i < len; i++) {
                (*vlist)[i] = trans_into_val((*vlist)[i]);
            }
            return (*vlist);
        }
        auto v = std::any_cast<variable>(&x);
        if (v) {
            //std::cerr << "var:" << (*v).id << '\n';
            return scope.getvar(*v);
        }
        else if (getsta(x)) return NoneState;
        return x;
    }
    void var_trans(std::any x, std::any y, std::any &tx, std::any &ty) {
        x = trans_into_val(x), y = trans_into_val(y);
        auto vstr1 = std::any_cast<std::string>(&x);
        auto vstr2 = std::any_cast<std::string>(&y);
        if (vstr1 || vstr2) {
            tx = x; ty = y;
            return;
        }
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
        if (vb1) t1 = (long long)(*vb1), tx = t1;
        else if (vll1) t1 = (*vll1), tx = t1;
        else tx = NoneState;
        if (vb2) t2 = (long long)(*vb2), ty = t2;
        else if (vll2) t2 = (*vll2), ty = t2;
        else ty = NoneState;
        return;
    }
    bool comp(std::any x, std::any y, std::string op) {
        //std::cerr << "comp,op:" << op << '\n';
        std::any t1, t2;
        var_trans(x, y, t1, t2);
        //type_x=type_y is guaranteed
        auto vsrt1 = std::any_cast<short>(&t1);
        auto vsrt2 = std::any_cast<short>(&t2);
        if (vsrt1 || vsrt2) {
            if (vsrt1 && vsrt2) {
                if (op == "==") return true;
                else return false;
            }
            return false;
            //if (op == "==") return false;
            //else return true;
        } 
        auto vdb1 = std::any_cast<double>(&t1);
        if (vdb1) {
            auto vdb2 = std::any_cast<double>(&t2);
            if (vdb2) return comp_simpl(*vdb1, *vdb2, op);
            if (op != "!=") return false;
            else return true;
        }
        auto vll1 = std::any_cast<int2048>(&t1);
        if (vll1) {
            //std::cerr << "int2048 comparison" << '\n';
            auto vll2 = std::any_cast<int2048>(&t2);
            if (vll2) return comp_simpl(*vll1, *vll2, op);
            if (op != "!=") return false;
            else return true;
        }
        auto vstr1 = std::any_cast<std::string>(&t1);
        if (vstr1) {
            auto vstr2 = std::any_cast<std::string>(&t2);
            if(vstr2) return comp_simpl(*vstr1, *vstr2, op);
            if (op != "!=") return false;
            else return true;
        }
        if (op != "!=") return false;
        else return true;
    }
    std::any addorsub(std::any x, std::any y, std::string op) {
        //std::cerr << "addorsub:" << op << '\n'; 
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
            //std::cerr << "string add";
            if (vstr2 && op == "+") return (*vstr1) + (*vstr2);
        }
        return NoneState;
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
                double ret = floor((*vdb1) / (*vdb2));
                return ret;
            }
        }
        auto vll1 = std::any_cast<int2048>(&t1);
        if (vll1) {
            auto vll2 = std::any_cast<int2048>(&t2);
            if (vll2) {
                if (op == "*") return (*vll1) * (*vll2);
                else if (op == "/") {
                    long long v1 = transtoll(*vll1), v2 = transtoll(*vll2);
                    return 1.0 * v1 / v2;
                }
                else if (op == "//") return (*vll1) / (*vll2);
                else if (op == "%") return (*vll1) % (*vll2);
            }
            else {
                auto vstr2 = std::any_cast<std::string>(&t2);
                if (op == "*") {
                    std::string ret = "";
                    long long cnt = transtoll(*vll1);
                    for (int i = 0; i < cnt; i++) ret += (*vstr2);
                    return ret;
                }
            }
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
            auto vb2 = std::any_cast<bool>(&t2);
            if (vb2 && op == "*") {
                std::string ret = "";
                long long cnt = (*vb2);
                for (int i = 0; i < cnt; i++) ret += (*vstr1);
                return ret;
            }
        }
        auto vb1 = std::any_cast<bool>(&t1);
        if (vb1) {
            auto vstr2 = std::any_cast<std::string>(&t2);
            if (op == "*") {
                std::string ret = "";
                long long cnt = (*vb1);
                for (int i = 0; i < cnt; i++) ret += (*vstr2);
                return ret;
            }
        }
        return NoneState;
    }
    short getsta (std::any x) {
        auto v = std::any_cast<short>(&x);
        if (v) return *v;
        auto ret = std::any_cast<returnvals>(&x);
        if (ret) {
            //std::cerr << "break because of returnval\n";
            return ReturnState;
        }
        return NoneState;
    }
    bool checkval(std::any x) {
        x = trans_into_val(x);
        auto vb = std::any_cast<bool>(&x);
        if (vb) {
            //std::cerr << "check_bool:" << (*vb) << '\n';
            return (*vb);
        }
        auto vll = std::any_cast<int2048>(&x);
        if (vll) return ((*vll) != 0);
        auto vdb = std::any_cast<double>(&x);
        if (vdb) return ((*vdb) != 0);
        auto vstr = std::any_cast<std::string>(&x);
        if (vstr) return ((*vstr) != "");
        return false;
    }
    void assign(std::any x, std::any y) {//x<-y
        auto xlist = std::any_cast<std::vector<std::any> >(&x);
        if (xlist) {
            int len = (*xlist).size();
            auto ylist = std::any_cast<std::vector<std::any> >(&y);
            for (int i = 0; i < len; i++)
                assign((*xlist)[i], (*ylist)[i]);
            return;
        }
        auto id = std::any_cast<variable>(&x);
        //std::cerr << "assign_name:" << (*id).id << '\n' ;
        y = trans_into_val(y);
        scope.revise(*id, y);
    }
    virtual std::any visitArglist(Python3Parser::ArglistContext *ctx) override {
        //std::cerr << "arglist:" << ctx->getText() << '\n';
        int len = (ctx->argument()).size();
        //std::cerr << "arglist_len:" << len << '\n';
        if (len == 0) return NoneState;
        if (len == 1) {
            std::any tt;
            if(ctx->argument(0)->ASSIGN()) tt = trans_into_val(visit(ctx->argument(0)->test(1)));
            else return visit(ctx->argument(0));
            std::any rt = visit(ctx->argument(0)->test(0));
            auto nw_var = std::any_cast<variable>(&rt);
            scope.a[scope.nw].val[(*nw_var).id] = tt;
            return rt;
        }
        std::vector<std::any> ret, tmp; ret.resize(len);
        tmp.resize(len);
        for (int i = 0; i < len; i++) {
            if (ctx->argument(i)->ASSIGN()) tmp[i] = trans_into_val(visit(ctx->argument(i)->test(1)));
            else ret[i] = visit(ctx->argument(i));
        }
        for (int i = 0; i < len; i++) {
            if (ctx->argument(i)->ASSIGN()) {
                ret[i] = visit(ctx->argument(i)->test(0));
                auto nw_var = std::any_cast<variable>(&ret[i]);
                //scope.a[scope.nw].val[(*nw_var).id] = NoneState;
                //assign(ret[i], tmp[i]);
                //std::cerr << "nw:" << scope.nw << '\n';
                //std::cerr << "id:" << (*nw_var).id << '\n';
                //auto v_tmp = std::any_cast<int2048>(&tmp[i]);
                //std::cerr << "val:" << (*v_tmp) << '\n';
                scope.a[scope.nw].val[(*nw_var).id] = tmp[i];
            }
        }
        return ret;
    }
    virtual std::any visitArgument(Python3Parser::ArgumentContext *ctx) override {
        if (ctx->ASSIGN()) {
            std::any t1 = visit(ctx->test(1));
            std::any t0 = visit(ctx->test(0));
            assign(t0, t1);
            return t0;
            //return trans_into_val(t0);
        }
        return trans_into_val(visit(ctx->test(0)));
    }
    virtual std::any visitTestlist(Python3Parser::TestlistContext *ctx) override {
        int len = (ctx->test()).size();
        if (len == 0) return NoneState;
        if (len == 1) return visit(ctx->test(0));
        std::vector<std::any> ret; ret.resize(len);
        for (int i = 0; i < len; i++) ret[i] = visit(ctx->test(i));
        return ret;
    }
    virtual std::any visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) override {
        if (ctx->break_stmt()) return visit(ctx->break_stmt());
        if (ctx->return_stmt()) return visit(ctx->return_stmt());
        if (ctx->continue_stmt()) return visit(ctx->continue_stmt());
        return NoneState;
    }
    virtual std::any visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) override {
        //std::cerr << "small_stmt:" << ctx->getText() << '\n';
        if (ctx->flow_stmt()) return visit(ctx->flow_stmt());
        if (ctx->expr_stmt()) return visit(ctx->expr_stmt());
        return NoneState;
    }
    virtual std::any visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) override {
        if (ctx->small_stmt()) return visit(ctx->small_stmt());
        return NoneState;
    }
    virtual std::any visitStmt(Python3Parser::StmtContext *ctx) override {
        if (ctx->simple_stmt()) return visit(ctx->simple_stmt());
        if (ctx->compound_stmt()) return visit(ctx->compound_stmt());
        return NoneState;
    }
    virtual std::any visitSuite(Python3Parser::SuiteContext *ctx) override {
        //std::cerr << "suite" << '\n';
        if (ctx->simple_stmt()) {
            std::any nw = visit(ctx->simple_stmt());
            if (getsta(nw)) return nw;
            auto vreturn = std::any_cast<returnvals>(&nw);
            if (vreturn) return (*vreturn);
        }
        int len = (ctx->stmt()).size();
        //std::cerr << "statements:" << len << '\n';
        for (int i = 0; i < len; i++) {
            std::any nw = visit(ctx->stmt(i));
            //std::cerr << "stmt[" << i << "]:" << ctx->stmt(i)->getText() << '\n';
            if (getsta(nw)) {
                //std::cerr << "br:" << getsta(nw) << '\n';
                return nw;
            }
            auto vreturn = std::any_cast<returnvals>(&nw);
            if (vreturn) {
                //std::cerr << "break because of returnval\n";
                return (*vreturn);
            }
        }
        return NoneState;
    }
    virtual std::any visitIf_stmt(Python3Parser::If_stmtContext *ctx) override {
        int len = (ctx->test()).size();
        //std::cerr << "If:" << ctx->getText() << '\n';
        for (int i = 0; i < len; i++) {
            std::any nw = visit(ctx->test(i));
            if (checkval(nw)) {
                //std::cerr << "pp:" << i << '\n';
                return visit(ctx->suite(i));
            }
        }
        //std::cerr << "test_len:" << len << " suite_len:" << (ctx->suite()).size() << '\n';
        if (ctx->ELSE()) {
            return visit(ctx->suite(len));
        }
        //std::cerr << "*\n";
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
        //std::cerr << "returnstmt" << '\n' ;
        if (ctx->testlist()) {
            //std::cerr << "nonemptyreturn\n";
            std::any ret = visit(ctx->testlist());
            ret = trans_into_val(ret);
            /*auto vv = std::any_cast<int2048>(&ret);
            if (vv) std::cerr << "test:" << (*vv) << '\n';*/
            return returnvals(ret);
        }
        //std::cerr << "empty" << '\n' ;
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
        //std::cerr << "expr_stmt:" << ctx->getText() << '\n';
        int len = (ctx->testlist()).size();
        if (len == 1) return visit(ctx->testlist(0));
        //std::cerr << "len=" << len << '\n';
        std::any ls, nw;
        ls = visit(ctx->testlist(len - 1));
        for (int i = len - 2; i >= 0; i--) {
            nw = visit(ctx->testlist(i));
            assign(nw, ls);
            ls = nw;
        }
        return ls;
    }
    std::string getstring(std::any x) {
        x = trans_into_val(x);
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
        auto vstr = std::any_cast<std::string>(&x);
        if (vstr) {
            int len = (*vstr).size();
            // if (len) return *vstr;
            // else return "None";
            return *vstr;
        }
        auto vnone = std::any_cast<short>(&x);
        if (vnone) return "None";
        auto vll = std::any_cast<int2048>(&x);
        if (vll) return transtostring(*vll);
        auto vdb = std::any_cast<double>(&x);
        if (vdb) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(6) << (*vdb);
            return oss.str();
        }
        auto vb = std::any_cast<bool>(&x);
        if (vb) {
            if ((*vb) == false) return "False";
            else return "True";
        }
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
                    if (tmp[k] == '\\') {
                        if (k == len - 1) ret += '\\', k--;
                        else if (tmp[k + 1] == 'n') ret += '\n';
                        else if (tmp[k + 1] == 't') ret += '\t';
                        else if (tmp[k + 1] == '\"') ret += tmp[k + 1];
                        else if (tmp[k + 1] == '\'') ret += tmp[k + 1];
                        else if (tmp[k + 1] == '\\') ret += tmp[k + 1];
                        else k--;
                        k++;
                    }
                    else if (tmp[k] == '{' || tmp[k] == '}') ret += tmp[k], k++;
                    else ret += tmp[k];
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
                if (tmp[k] == '\\') {
                    if (k == len - 1) ret += '\\', k--;
                    else if (tmp[k + 1] == 'n') ret += '\n';
                    else if (tmp[k + 1] == 't') ret += '\t';
                    else if (tmp[k + 1] == '\"') ret += tmp[k + 1];
                    else if (tmp[k + 1] == '\'') ret += tmp[k + 1];
                    else if (tmp[k + 1] == '\\') ret += tmp[k + 1];
                    else k--;
                    k++;
                }
                else if (tmp[k] == '{' || tmp[k] == '}') ret += tmp[k], k++;
                else ret += tmp[k];
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
        if (ctx->arglist()) return visit(ctx->arglist());
        else return NoneState;
    }
    virtual std::any visitTypedargslist(Python3Parser::TypedargslistContext *ctx) override {
        int len = (ctx->tfpdef()).size();
        std::vector<std::any> paras, inis;
        int test_len = (ctx->test()).size(), test_pos = 0;
        //std::cerr << "tfpdef_len:" << len << '\n';
        //std::cerr << "tf_test_len:" << test_len << '\n';
        for(int i = 0; i < len; i++) {
            std::any nw = visit(ctx->tfpdef(i));
            auto var = std::any_cast<variable>(&nw);
            paras.push_back(nw);
            if (i + test_len >= len) {
                //std::cerr << "ini:" << (*var).id << '\n';
                //std::cerr << scope.nw << ' ' << scope.findvar(*var) << '\n';
                inis.push_back (trans_into_val(visit(ctx->test(test_pos))));
                // if (scope.nw > 1 && !scope.findvar(*var)) {
                //     //std::cerr << "assi_func:" << (*var).id << '\n';
                //     assign(*var, visit(ctx->test(test_pos)));
                // }
                test_pos++;
            }
            else inis.push_back (NoneState);
        }
        //std::cerr << "tf_para_len" << paras.size() << '\n';
        return std::make_pair(paras, inis);
    }
    virtual std::any visitTfpdef(Python3Parser::TfpdefContext *ctx) override {
        return variable((ctx->NAME()->getText()), false);
    }
    virtual std::any visitParameters(Python3Parser::ParametersContext *ctx) override {
        if (ctx->typedargslist()) return visit(ctx->typedargslist());
        return NoneState;
    }
    virtual std::any visitFuncdef(Python3Parser::FuncdefContext *ctx) override {
        std::string func_name = ctx->NAME()->getText();
        funcs[func_name] = ++func_count;
        func_information[func_count].id = func_name;
        func_information[func_count].ctx = ctx;
        std::any v = visit(ctx->parameters());
        auto var = std::any_cast<variable>(&v);
        if (var) func_information[func_count].paras.push_back(*var);
        else {
            auto lst = std::any_cast<std::pair<std::vector<std::any>, std::vector<std::any> > >(&v);
            if (lst) {
                int len = (*lst).first.size();
                for (int i = 0; i < len; i++) {
                    std::any nw = (*lst).first[i];
                    std::any ival = (*lst).second[i];
                    auto para = std::any_cast<variable>(&nw);
                    func_information[func_count].paras.push_back (*para);
                    func_information[func_count].inivals.push_back (ival);
                }
            }
        }
        //std::cerr << "funcdef done:" << "id:" << func_count << ",paras:" << func_information[func_count].paras.size() << '\n';
        return NoneState;
    }
    virtual std::any visitAtom_expr(Python3Parser::Atom_exprContext *ctx) override {
        //std::cerr << "atomexpr:" << ctx->getText() << '\n';
        if (ctx->trailer()) {
            //function todo
            std::string func_name = ctx->atom()->NAME()->getText();
            if (func_name == "print") {
                if (ctx->trailer()->arglist()) {
                    std::any x = visit(ctx->trailer());
                    std::cout << getstring(x) << '\n';
                }
                else std::cout << '\n';
                return NoneState;
            }
            else if (func_name == "int") {
                std::any x = visit(ctx->trailer());
                auto vdb = std::any_cast<double>(&x);
                if (vdb) return (int2048)((long long)(*vdb));
                auto vll = std::any_cast<int2048>(&x);
                if (vll) return (*vll);
                auto vb = std::any_cast<bool>(&x);
                if (vb) return (int2048)(*vb);
                auto vstr = std::any_cast<std::string>(&x);
                if (vstr) return (int2048)(*vstr);
                return (int2048)0;
            }
            else if (func_name == "float") {
                std::any x = visit(ctx->trailer());
                auto vdb = std::any_cast<double>(&x);
                if (vdb) return (*vdb);
                auto vll = std::any_cast<int2048>(&x);
                if (vll) return (double)transtoll(*vll);
                auto vb = std::any_cast<bool>(&x);
                if (vb) return (double)(*vb);
                auto vstr = std::any_cast<std::string>(&x);
                if (vstr) {
                    double ret = 0, nw_dig = 0.1; bool poi = 0, neg = 0;
                    std::string str = (*vstr);
                    int len = str.size();
                    for (int i = 0; i < len; i++) {
                        if (str[i] == '-') {
                            neg = 1;
                            continue;
                        }
                        if (str[i] != '.') {
                            if (!poi) ret = ret * 10.0 + str[i] - '0';
                            else ret += nw_dig * (str[i] - '0'), nw_dig /= 10.0;
                        }
                        else poi = 1;
                        //std::cerr << "nw_float:" << ret << '\n';
                    }
                    //std::cerr << ret << '\n';
                    if (neg) ret = -ret;
                    return ret;
                }
                return (double)0.0;
            }
            else if (func_name == "str") {
                std::any x = visit(ctx->trailer());
                return getstring(x);
            }
            else if (func_name == "bool") {
                std::any x = visit(ctx->trailer());
                auto vdb = std::any_cast<double>(&x);
                if (vdb) return ((*vdb) != 0) ? true : false;
                auto vll = std::any_cast<int2048>(&x);
                if (vll) return ((*vll) != 0) ? true : false;
                auto vb = std::any_cast<bool>(&x);
                if (vb) return (*vb);
                auto vstr = std::any_cast<std::string>(&x);
                if (vstr) return ((*vstr) != "") ? true : false;
                return false;
            }
            else {
                int pos = funcs[func_name];
                //std::cerr << pos << '\n';
                scope.newscope();
                //std::cerr << scope.nw << '\n';
                Python3Parser::FuncdefContext *func_ctx = func_information[pos].ctx;
                //if (ctx) std::cerr << "not empty" << '\n';
                int len = func_information[pos].paras.size();
                if (len) {
                    std::any x = visit(ctx->trailer());
                    auto lst = std::any_cast<std::vector<std::any> >(&x);
                    //todo:有初值的变量，……
                    if (lst) {
                        //std::cerr << "trailer_list" << '\n';
                        int arg_len = (*lst).size();
                        // for (int i = 0; i < arg_len; i++) 
                        //     (*lst)[i] = trans_into_val((*lst)[i]); 
                        for (int i = 0; i < len && i < arg_len; i++) {
                            variable nw = func_information[pos].paras[i];
                            //std::cerr << "paraname:" << nw.id << '\n';
                            if (!scope.findvar(nw)) {
                                //std::cerr << "->:" << nw.id << '\n';
                                
                                auto isvar = std::any_cast<variable>(&((*lst)[i]));
                                if (isvar == nullptr) {
                                    scope.a[scope.nw].val[nw.id] = NoneState;
                                    assign(nw , (*lst)[i]);
                                }
                                    
                                //else std::cerr << "have_var:" << (*lst) 
                            }
                        }
                    }
                    else {
                        auto sta = std::any_cast<short>(&x);
                        if (sta == nullptr) {
                            //x = trans_into_val(x);
                            //std::cerr << "paras_size:" << sz << '\n';
                            variable nw = func_information[pos].paras[0];
                            if (!scope.findvar(nw)) {
                                auto isvar = std::any_cast<variable>(&x);
                                if (isvar == nullptr) {
                                    scope.a[scope.nw].val[nw.id] = NoneState;
                                    assign(nw , x);
                                }
                            }
                        } 
                    }
                }
                //visit(func_ctx->parameters());
                for (int i = 0; i < len; i++) {
                    variable nw = func_information[pos].paras[i];
                    if (!scope.findvar(nw)) {
                        scope.a[scope.nw].val[nw.id] = NoneState;
                        assign(nw, func_information[pos].inivals[i]);
                    }
                }
                std::any ret = NoneState;
                if (func_ctx->suite()) ret = visit(func_ctx->suite());
                scope.getback();
                if (func_ctx->suite()) {
                    ret = trans_into_val(ret);
                    return ret;
                }
                else return NoneState;
            }
            return NoneState;
        }
        return visit(ctx->atom());
    }
    std::any getrev(std::any x) {
        x = trans_into_val(x);
        auto vll = std::any_cast<int2048>(&x);
        if (vll) return -(*vll);
        auto vdb = std::any_cast<double>(&x);
        if (vdb) return -(*vdb);
        auto vb = std::any_cast<bool>(&x);
        if (vb) return (-(int2048)(*vb));
        return NoneState;
    }
    virtual std::any visitFactor(Python3Parser::FactorContext *ctx) override {
        if (ctx->MINUS()) {
            if (ctx->atom_expr()) return getrev(visit(ctx->atom_expr()));
            else return getrev(visit(ctx->factor()));
        }
        if (ctx->atom_expr()) return visit(ctx->atom_expr());
        else return visit(ctx->factor());
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
        else if(auto vstr = std::any_cast<std::string>(&ret);
                vstr) {
                    if ((*vstr) == "") return true;
                    else return false;
                }
        else if(auto vdb = std::any_cast<double>(&ret);
                vdb) {
                    if ((*vdb) == 0.0) return true;
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
        //std::cerr << "Atom:" << ctx->getText() << '\n' ;
        //todo: format_string 
        if (ctx->TRUE()) return true;
        if (ctx->FALSE()) return false;
        if (ctx->NAME()) {
            std::string name = ctx->NAME()->getText();
            bool is_all ;
            if (scope.nw == 1) is_all = 1;
            else if (scope.a[scope.nw].val.count(name)) is_all = false;
            else is_all = bool(scope.a[1].val.count(name));
            return variable(name, is_all);
        }
        if (ctx->NUMBER()) {
            std::string str = ctx->NUMBER()->getText();
            int len = str.size(); bool fl = false;
            for (int i = 0; i < len; i++)
                if (str[i] == '.') fl = true;
            if (fl) {
                double ret = std::stod(str);
                // double ret = 0, nw_dig = 0.1; bool poi = 0, neg = 0;
                // for (int i = 0; i < len; i++) {
                //     if (str[i] == '-') {
                //         neg = 1;
                //         continue;
                //     }
                //     if (str[i] != '.') {
                //         if (!poi) ret = ret * 10.0 + str[i] - '0';
                //         else ret += nw_dig * (str[i] - '0'), nw_dig /= 10.0;
                //     }
                //     else poi = 1;
                //     //std::cerr << "nw_float:" << ret << '\n';
                // }
                // //std::cerr << ret << '\n';
                // if (neg) {
                //     ret = -ret;
                //     if (ret == 0.0) ret = -0.0;
                // }
                return ret;
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
                //std::cerr << tmp << '\n';
                int sz = tmp.size();
                if (sz <= 2) continue;
                tmp = tmp.substr(1 , sz - 2);
                //std::cerr << tmp << '\n';
                str += tmp;
            }
            len = str.size();
            std::string ret = "";
            for (int i = 0; i < len; i++) {
                if (str[i] == '\\') {
                    if (i == len - 1) ret += '\\', i--;
                    else if (str[i + 1] == 'n') ret += '\n';
                    else if (str[i + 1] == 't') ret += '\t';
                    else if (str[i + 1] == '\"') ret += str[i + 1];
                    else if (str[i + 1] == '\'') ret += str[i + 1];
                    else if (str[i + 1] == '\\') ret += str[i + 1];
                    else i--;
                    i++;
                }
                else ret += str[i];
            }
            //str = '\"' + str + '\"';
            //std::cerr << "string:" << str << '\n';
            return ret;
        }
        if (ctx->test()) {
            return visit(ctx->test());
        }
        return NoneState;
    }
};


#endif//PYTHON_INTERPRETER_EVALVISITOR_H