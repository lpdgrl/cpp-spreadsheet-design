#pragma once

#include "FormulaLexer.h"
#include "common.h"

#include <forward_list>
#include <functional>
#include <stdexcept>

namespace ASTImpl {
class Expr;
}

class ParsingError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class FormulaAST {
public:
    explicit FormulaAST(std::unique_ptr<ASTImpl::Expr> root_expr);
    FormulaAST(FormulaAST&&) = default;
    FormulaAST& operator=(FormulaAST&&) = default;
    ~FormulaAST();

    // Добавляем ссылку на таблицу чтобы получить значение ячейки. 
    double Execute(const SheetInterface& sheet) const;
    void Print(std::ostream& out) const;
    void PrintFormula(std::ostream& out) const;
    // Печатает значения ячеек участвующих в формуле
    void PrintCells(std::ostream& out) const;

    std::forward_list<Position>& GetReferencedCells();
    const std::forward_list<Position>& GetReferencedCells() const;

private:
    std::unique_ptr<ASTImpl::Expr> root_expr_;
    // Храним позиции ячеек которые добавлены в формулу
    std::forward_list<Position> cells_; 
};

FormulaAST ParseFormulaAST(std::istream& in);
FormulaAST ParseFormulaAST(const std::string& in_str);