// 你需要提交这份代码
#pragma once

#include "visitor.h"

struct calculator : visitor {
    /// TODO: 完成所有需求
    std::any visit_num (num_node *ctx) override {
        return ctx->number;
    }
    std::any visit_add (add_node *ctx) override {
        std::any lc = ((ctx->lnode)->accept(this));
        std::any rc = ((ctx->rnode)->accept(this));
        double *ptr = std::any_cast <double> (&lc);
        if (ptr == nullptr) {
            long long x = std::any_cast <long long> (lc);
            long long y = std::any_cast <long long> (rc);
            return x + y;
        }
        else {
            double x = std::any_cast <double> (lc);
            double y = std::any_cast <double> (rc);
            return x + y;
        }
    }
    std::any visit_sub (sub_node *ctx) override {
        std::any lc = ((ctx->lnode)->accept(this));
        std::any rc = ((ctx->rnode)->accept(this));
        double *ptr = std::any_cast <double> (&lc);
        if (ptr == nullptr) {
            long long x = std::any_cast <long long> (lc);
            long long y = std::any_cast <long long> (rc);
            return x - y;
        }
        else {
            double x = std::any_cast <double> (lc);
            double y = std::any_cast <double> (rc);
            return x - y;
        }
    }
    std::any visit_mul (mul_node *ctx) override {
        std::any lc = ((ctx->lnode)->accept(this));
        std::any rc = ((ctx->rnode)->accept(this));
        double *ptr = std::any_cast <double> (&lc);
        if (ptr == nullptr) {
            long long x = std::any_cast <long long> (lc);
            long long y = std::any_cast <long long> (rc);
            return x * y;
        }
        else {
            double x = std::any_cast <double> (lc);
            double y = std::any_cast <double> (rc);
            return x * y;
        }
    }
    std::any visit_div (div_node *ctx) override {
        std::any lc = ((ctx->lnode)->accept(this));
        std::any rc = ((ctx->rnode)->accept(this));
        double *ptr = std::any_cast <double> (&lc);
        if (ptr == nullptr) {
            long long x = std::any_cast <long long> (lc);
            long long y = std::any_cast <long long> (rc);
            return x / y;
        }
        else {
            double x = std::any_cast <double> (lc);
            double y = std::any_cast <double> (rc);
            return x / y;
        }
    }
    ~calculator() override = default;
};
