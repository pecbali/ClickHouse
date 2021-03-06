#pragma once

#include <Storages/MergeTree/MergeTreeIndices.h>
#include <Storages/MergeTree/MergeTreeData.h>
#include <Storages/MergeTree/KeyCondition.h>

#include <memory>


namespace DB
{

class MergeTreeMinMaxIndex;


struct MergeTreeMinMaxGranule : public MergeTreeIndexGranule
{
    explicit MergeTreeMinMaxGranule(const MergeTreeMinMaxIndex & index);

    void serializeBinary(WriteBuffer & ostr) const override;
    void deserializeBinary(ReadBuffer & istr) override;

    String toString() const override;
    bool empty() const override { return parallelogram.empty(); }

    void update(const Block & block, size_t * pos, size_t limit) override;

    ~MergeTreeMinMaxGranule() override = default;

    const MergeTreeMinMaxIndex & index;
    std::vector<Range> parallelogram;
};


class MinMaxCondition : public IndexCondition
{
public:
    MinMaxCondition(
        const SelectQueryInfo & query,
        const Context & context,
        const MergeTreeMinMaxIndex & index);

    bool alwaysUnknownOrTrue() const override;

    bool mayBeTrueOnGranule(MergeTreeIndexGranulePtr idx_granule) const override;

    ~MinMaxCondition() override = default;
private:
    const MergeTreeMinMaxIndex & index;
    KeyCondition condition;
};


class MergeTreeMinMaxIndex : public MergeTreeIndex
{
public:
    MergeTreeMinMaxIndex(
        String name_,
        ExpressionActionsPtr expr_,
        const Names & columns_,
        const DataTypes & data_types_,
        const Block & header_,
        size_t granularity_)
        : MergeTreeIndex(name_, expr_, columns_, data_types_, header_, granularity_) {}

    ~MergeTreeMinMaxIndex() override = default;

    MergeTreeIndexGranulePtr createIndexGranule() const override;

    IndexConditionPtr createIndexCondition(
        const SelectQueryInfo & query, const Context & context) const override;

};

std::unique_ptr<MergeTreeIndex> MergeTreeMinMaxIndexCreator(
    const NamesAndTypesList & columns, std::shared_ptr<ASTIndexDeclaration> node, const Context & context);

}
