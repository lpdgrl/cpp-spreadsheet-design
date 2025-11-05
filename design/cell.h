#pragma once

#include "common.h"
#include "formula.h"

#include <optional>

class Cell : public CellInterface {
public:
    Cell(SheetInterface& sheet);
    ~Cell();

    // Используем pos для сохранения позиции ячейки чтобы из нее начать поиск зависимостей DFS
    void Set(Position pos, std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;

    std::vector<Position> GetReferencedCells() const override;

private:
    class ImplCell {
    public:
        virtual ~ImplCell() = default;
        virtual Value GetValue() const = 0;
        virtual std::string GetText() const = 0;
        virtual void Clear() = 0;
    };

    class EmptyImplCell : public ImplCell {
        EmptyImplCell() = default;
        ~EmptyImplCell() = default;

        Value GetValue() const override;
        std::string GetText() const override;
        void Clear() override;
    };

    class TextImplCell : public ImplCell {
        TextImplCell(std::string& text);
        ~TextImplCell() = default;

        Value GetValue() const override;
        std::string GetText() const override;
        void Clear() override;
    private:
        std::string text_;
    };
    
    class FormulaImplCell : public ImplCell {
    public:
        FormulaImplCell(std::string& formula);
        ~FormulaImplCell() = default;

        Value GetValue() const override;
        std::string GetText() const override;
        void Clear() override;
        void UpdateCache(); 
    private:
        std::unique_ptr<FormulaInterface> formula_;
        // Храним вычисленный результат (кэш). Помечен как mutable для изменения в константных методах
        mutable std::optional<FormulaInterface::Value> calculate_value_;
    };

    // поиск циклических зависимостей при помощи алгоритма DFS (обхода графа в глубину)
    // передаем вектор с вершинами для обхода
    bool PerfomDFS(const std::vector<Position>& cells) const;

    // реализация ячейки
    std::unique_ptr<ImplCell> impl_;
    
    // контейнер клеток которые зависят от данной клетки для инвалидации кэша
    std::unordered_set<Position, PositionHash> linked_cells_;

    // контейнер для хранения указателей на ячейки которые используются в this-ячейке (отслеживание циклов)
    std::vector<Position> dependecies_;

    // Позиция ячейки в таблице
    Position pos_in_sheet_;
    // Ссылка на таблицу, передается через конструктор. Используется для вычислений/поиска зависимостей
    SheetInterface& sheet_;
};