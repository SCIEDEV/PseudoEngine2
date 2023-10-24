#pragma once
#include "interpreter/procedure.h"

namespace Interpreter {
    static Token errToken{TokenType::FUNCTION, 0, 0};

    struct BuiltinFnLength : public Function {
        BuiltinFnLength();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnRight : public Function {
        BuiltinFnRight();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnMid : public Function {
        BuiltinFnMid();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnLeft : public Function {
        BuiltinFnLeft();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnToUpper : public Function {
        BuiltinFnToUpper();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnToLower : public Function {
        BuiltinFnToLower();

        void run(Interpreter::Context &ctx) override;
    };


    struct BuiltinFnNumToStr : public Function {
        BuiltinFnNumToStr();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnStrToNum : public Function {
        BuiltinFnStrToNum();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnIsNum : public Function {
        BuiltinFnIsNum();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnEOF : public Function {
        BuiltinFnEOF();

        void run(Interpreter::Context &ctx) override;
    };


    struct BuiltinFnLCase : public Function {
        BuiltinFnLCase();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnUCase : public Function {
        BuiltinFnUCase();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnASC : public Function {
        BuiltinFnASC();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnCHR : public Function {
        BuiltinFnCHR();

        void run(Interpreter::Context &ctx) override;
    };


    struct BuiltinFnDAY : public Function {
        BuiltinFnDAY();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnMONTH : public Function {
        BuiltinFnMONTH();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnYEAR : public Function {
        BuiltinFnYEAR();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnDAYINDEX : public Function {
        BuiltinFnDAYINDEX();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnSETDATE : public Function {
        BuiltinFnSETDATE();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnTODAY : public Function {
        BuiltinFnTODAY();

        void run(Interpreter::Context &ctx) override;
    };


    struct BuiltinFnRand : public Function {
        BuiltinFnRand();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnInt : public Function {
        BuiltinFnInt();

        void run(Interpreter::Context &ctx) override;
    };


    struct BuiltinFnPow : public Function {
        BuiltinFnPow();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnExp : public Function {
        BuiltinFnExp();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnSin : public Function {
        BuiltinFnSin();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnCos : public Function {
        BuiltinFnCos();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnTan : public Function {
        BuiltinFnTan();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnASin : public Function {
        BuiltinFnASin();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnACos : public Function {
        BuiltinFnACos();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnATan : public Function {
        BuiltinFnATan();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnATan2 : public Function {
        BuiltinFnATan2();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnSqrt : public Function {
        BuiltinFnSqrt();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnLog : public Function {
        BuiltinFnLog();

        void run(Interpreter::Context &ctx) override;
    };

    struct BuiltinFnLn : public Function {
        BuiltinFnLn();

        void run(Interpreter::Context &ctx) override;
    };
}
