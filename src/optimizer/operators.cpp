//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// operators.cpp
//
// Identification: src/optimizer/operators.cpp
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "optimizer/operators.h"
#include "optimizer/operator_visitor.h"

namespace peloton {
namespace optimizer {
//===--------------------------------------------------------------------===//
// Leaf
//===--------------------------------------------------------------------===//
Operator LeafOperator::make(GroupID group) {
  LeafOperator *op = new LeafOperator;
  op->origin_group = group;
  return Operator(op);
}

//===--------------------------------------------------------------------===//
// Get
//===--------------------------------------------------------------------===//
Operator LogicalGet::make(storage::DataTable *table) {
  LogicalGet *get = new LogicalGet;
  get->table = table;
  return Operator(get);
}

bool LogicalGet::operator==(const BaseOperatorNode &node) {
  if (node.type() != OpType::Get) return false;
  const LogicalGet &r = *static_cast<const LogicalGet *>(&node);
  if (table->GetOid() != r.table->GetOid()) return false;

  return true;
}

hash_t LogicalGet::Hash() const {
  hash_t hash = BaseOperatorNode::Hash();
  oid_t table_oid = table->GetOid();
  hash = util::CombineHashes(hash, util::Hash<oid_t>(&table_oid));
  return hash;
}

//===--------------------------------------------------------------------===//
// Project
//===--------------------------------------------------------------------===//
Operator LogicalProject::make() {
  LogicalProject *project = new LogicalProject;
  return Operator(project);
}

//===--------------------------------------------------------------------===//
// Select
//===--------------------------------------------------------------------===//
Operator LogicalFilter::make() {
  LogicalFilter *select = new LogicalFilter;
  return Operator(select);
}

//===--------------------------------------------------------------------===//
// InnerJoin
//===--------------------------------------------------------------------===//
Operator LogicalInnerJoin::make() {
  LogicalInnerJoin *join = new LogicalInnerJoin;
  return Operator(join);
}

//===--------------------------------------------------------------------===//
// LeftJoin
//===--------------------------------------------------------------------===//
Operator LogicalLeftJoin::make() {
  LogicalLeftJoin *join = new LogicalLeftJoin;
  return Operator(join);
}

//===--------------------------------------------------------------------===//
// RightJoin
//===--------------------------------------------------------------------===//
Operator LogicalRightJoin::make() {
  LogicalRightJoin *join = new LogicalRightJoin;
  return Operator(join);
}

//===--------------------------------------------------------------------===//
// OuterJoin
//===--------------------------------------------------------------------===//
Operator LogicalOuterJoin::make() {
  LogicalOuterJoin *join = new LogicalOuterJoin;
  return Operator(join);
}
//===--------------------------------------------------------------------===//
// Aggregate
//===--------------------------------------------------------------------===//
Operator LogicalAggregate::make() {
  LogicalAggregate *agg = new LogicalAggregate;
  return Operator(agg);
}

//===--------------------------------------------------------------------===//
// Limit
//===--------------------------------------------------------------------===//
Operator LogicalLimit::make() {
  LogicalLimit *limit_op = new LogicalLimit;
  return Operator(limit_op);
}

//===--------------------------------------------------------------------===//
// Scan
//===--------------------------------------------------------------------===//
Operator PhysicalScan::make(storage::DataTable *table,
                            std::vector<Column *> cols) {
  PhysicalScan *scan = new PhysicalScan;
  scan->table = table;
  scan->columns = cols;
  return Operator(scan);
}

bool PhysicalScan::operator==(const BaseOperatorNode &node) {
  if (node.type() != OpType::Scan) return false;
  const PhysicalScan &r = *static_cast<const PhysicalScan *>(&node);
  if (table->GetOid() != r.table->GetOid()) return false;
  if (columns.size() != r.columns.size()) return false;

  for (size_t i = 0; i < columns.size(); ++i) {
    if (columns[i] != r.columns[i]) return false;
  }
  return true;
}

hash_t PhysicalScan::Hash() const {
  hash_t hash = BaseOperatorNode::Hash();
  oid_t table_oid = table->GetOid();
  hash = util::CombineHashes(hash, util::Hash<oid_t>(&table_oid));
  for (Column *col : columns) {
    hash = util::CombineHashes(hash, col->Hash());
  }
  return hash;
}

//===--------------------------------------------------------------------===//
// ComputeExprs
//===--------------------------------------------------------------------===//
Operator PhysicalComputeExprs::make() {
  PhysicalComputeExprs *compute = new PhysicalComputeExprs;
  return Operator(compute);
}

//===--------------------------------------------------------------------===//
// Filter
//===--------------------------------------------------------------------===//
Operator PhysicalFilter::make() {
  PhysicalFilter *filter = new PhysicalFilter;
  return Operator(filter);
}

//===--------------------------------------------------------------------===//
// InnerNLJoin
//===--------------------------------------------------------------------===//
Operator PhysicalInnerNLJoin::make() {
  PhysicalInnerNLJoin *join = new PhysicalInnerNLJoin;
  return Operator(join);
}

//===--------------------------------------------------------------------===//
// LeftNLJoin
//===--------------------------------------------------------------------===//
Operator PhysicalLeftNLJoin::make() {
  PhysicalLeftNLJoin *join = new PhysicalLeftNLJoin;
  return Operator(join);
}

//===--------------------------------------------------------------------===//
// RightNLJoin
//===--------------------------------------------------------------------===//
Operator PhysicalRightNLJoin::make() {
  PhysicalRightNLJoin *join = new PhysicalRightNLJoin;
  return Operator(join);
}

//===--------------------------------------------------------------------===//
// OuterNLJoin
//===--------------------------------------------------------------------===//
Operator PhysicalOuterNLJoin::make() {
  PhysicalOuterNLJoin *join = new PhysicalOuterNLJoin;
  return Operator(join);
}

//===--------------------------------------------------------------------===//
// InnerHashJoin
//===--------------------------------------------------------------------===//
Operator PhysicalInnerHashJoin::make() {
  PhysicalInnerHashJoin *join = new PhysicalInnerHashJoin;
  return Operator(join);
}

//===--------------------------------------------------------------------===//
// LeftHashJoin
//===--------------------------------------------------------------------===//
Operator PhysicalLeftHashJoin::make() {
  PhysicalLeftHashJoin *join = new PhysicalLeftHashJoin;
  return Operator(join);
}

//===--------------------------------------------------------------------===//
// RightHashJoin
//===--------------------------------------------------------------------===//
Operator PhysicalRightHashJoin::make() {
  PhysicalRightHashJoin *join = new PhysicalRightHashJoin;
  return Operator(join);
}

//===--------------------------------------------------------------------===//
// OuterHashJoin
//===--------------------------------------------------------------------===//
Operator PhysicalOuterHashJoin::make() {
  PhysicalOuterHashJoin *join = new PhysicalOuterHashJoin;
  return Operator(join);
}

//===--------------------------------------------------------------------===//
template <typename T>
void OperatorNode<T>::accept(OperatorVisitor *v) const {
  v->visit((const T *)this);
}

template <>
void OperatorNode<LeafOperator>::accept(
    UNUSED_ATTRIBUTE OperatorVisitor *v) const {}
template <>
void OperatorNode<LogicalGet>::accept(
    UNUSED_ATTRIBUTE OperatorVisitor *v) const {}
template <>
void OperatorNode<LogicalProject>::accept(
    UNUSED_ATTRIBUTE OperatorVisitor *v) const {}
template <>
void OperatorNode<LogicalFilter>::accept(
    UNUSED_ATTRIBUTE OperatorVisitor *v) const {}
template <>
void OperatorNode<LogicalInnerJoin>::accept(
    UNUSED_ATTRIBUTE OperatorVisitor *v) const {}
template <>
void OperatorNode<LogicalLeftJoin>::accept(
    UNUSED_ATTRIBUTE OperatorVisitor *v) const {}
template <>
void OperatorNode<LogicalRightJoin>::accept(
    UNUSED_ATTRIBUTE OperatorVisitor *v) const {}
template <>
void OperatorNode<LogicalOuterJoin>::accept(
    UNUSED_ATTRIBUTE OperatorVisitor *v) const {}
template <>
void OperatorNode<LogicalAggregate>::accept(
    UNUSED_ATTRIBUTE OperatorVisitor *v) const {}
template <>
void OperatorNode<LogicalLimit>::accept(
    UNUSED_ATTRIBUTE OperatorVisitor *v) const {}

template <>
std::string OperatorNode<LeafOperator>::name_ = "LeafOperator";
template <>
std::string OperatorNode<LogicalGet>::name_ = "LogicalGet";
template <>
std::string OperatorNode<LogicalProject>::name_ = "LogicalProject";
template <>
std::string OperatorNode<LogicalFilter>::name_ = "LogicalFilter";
template <>
std::string OperatorNode<LogicalInnerJoin>::name_ = "LogicalInnerJoin";
template <>
std::string OperatorNode<LogicalLeftJoin>::name_ = "LogicalLeftJoin";
template <>
std::string OperatorNode<LogicalRightJoin>::name_ = "LogicalRightJoin";
template <>
std::string OperatorNode<LogicalOuterJoin>::name_ = "LogicalOuterJoin";
template <>
std::string OperatorNode<LogicalAggregate>::name_ = "LogicalAggregate";
template <>
std::string OperatorNode<LogicalLimit>::name_ = "LogicalLimit";
template <>
std::string OperatorNode<PhysicalScan>::name_ = "PhysicalScan";
template <>
std::string OperatorNode<PhysicalComputeExprs>::name_ = "PhysicalComputeExprs";
template <>
std::string OperatorNode<PhysicalFilter>::name_ = "PhysicalFilter";
template <>
std::string OperatorNode<PhysicalInnerNLJoin>::name_ = "PhysicalInnerNLJoin";
template <>
std::string OperatorNode<PhysicalLeftNLJoin>::name_ = "PhysicalLeftNLJoin";
template <>
std::string OperatorNode<PhysicalRightNLJoin>::name_ = "PhysicalRightNLJoin";
template <>
std::string OperatorNode<PhysicalOuterNLJoin>::name_ = "PhysicalOuterNLJoin";
template <>
std::string OperatorNode<PhysicalInnerHashJoin>::name_ =
    "PhysicalInnerHashJoin";
template <>
std::string OperatorNode<PhysicalLeftHashJoin>::name_ = "PhysicalLeftHashJoin";
template <>
std::string OperatorNode<PhysicalRightHashJoin>::name_ =
    "PhysicalRightHashJoin";
template <>
std::string OperatorNode<PhysicalOuterHashJoin>::name_ =
    "PhysicalOuterHashJoin";

template <>
OpType OperatorNode<LeafOperator>::type_ = OpType::Leaf;
template <>
OpType OperatorNode<LogicalGet>::type_ = OpType::Get;
template <>
OpType OperatorNode<LogicalProject>::type_ = OpType::Project;
template <>
OpType OperatorNode<LogicalFilter>::type_ = OpType::LogicalFilter;
template <>
OpType OperatorNode<LogicalInnerJoin>::type_ = OpType::InnerJoin;
template <>
OpType OperatorNode<LogicalLeftJoin>::type_ = OpType::LeftJoin;
template <>
OpType OperatorNode<LogicalRightJoin>::type_ = OpType::RightJoin;
template <>
OpType OperatorNode<LogicalOuterJoin>::type_ = OpType::OuterJoin;
template <>
OpType OperatorNode<LogicalAggregate>::type_ = OpType::Aggregate;
template <>
OpType OperatorNode<LogicalLimit>::type_ = OpType::Limit;
template <>
OpType OperatorNode<PhysicalScan>::type_ = OpType::Scan;
template <>
OpType OperatorNode<PhysicalComputeExprs>::type_ = OpType::ComputeExprs;
template <>
OpType OperatorNode<PhysicalFilter>::type_ = OpType::Filter;
template <>
OpType OperatorNode<PhysicalInnerNLJoin>::type_ = OpType::InnerNLJoin;
template <>
OpType OperatorNode<PhysicalLeftNLJoin>::type_ = OpType::LeftNLJoin;
template <>
OpType OperatorNode<PhysicalRightNLJoin>::type_ = OpType::RightNLJoin;
template <>
OpType OperatorNode<PhysicalOuterNLJoin>::type_ = OpType::OuterNLJoin;
template <>
OpType OperatorNode<PhysicalInnerHashJoin>::type_ = OpType::InnerHashJoin;
template <>
OpType OperatorNode<PhysicalLeftHashJoin>::type_ = OpType::LeftHashJoin;
template <>
OpType OperatorNode<PhysicalRightHashJoin>::type_ = OpType::RightHashJoin;
template <>
OpType OperatorNode<PhysicalOuterHashJoin>::type_ = OpType::OuterHashJoin;

template <>
bool OperatorNode<LeafOperator>::IsLogical() const {
  return false;
}
template <>
bool OperatorNode<LogicalGet>::IsLogical() const {
  return true;
}
template <>
bool OperatorNode<LogicalProject>::IsLogical() const {
  return true;
}
template <>
bool OperatorNode<LogicalFilter>::IsLogical() const {
  return true;
}
template <>
bool OperatorNode<LogicalInnerJoin>::IsLogical() const {
  return true;
}
template <>
bool OperatorNode<LogicalLeftJoin>::IsLogical() const {
  return true;
}
template <>
bool OperatorNode<LogicalRightJoin>::IsLogical() const {
  return true;
}
template <>
bool OperatorNode<LogicalOuterJoin>::IsLogical() const {
  return true;
}
template <>
bool OperatorNode<LogicalAggregate>::IsLogical() const {
  return true;
}
template <>
bool OperatorNode<LogicalLimit>::IsLogical() const {
  return true;
}
template <>
bool OperatorNode<PhysicalScan>::IsLogical() const {
  return false;
}
template <>
bool OperatorNode<PhysicalComputeExprs>::IsLogical() const {
  return false;
}
template <>
bool OperatorNode<PhysicalFilter>::IsLogical() const {
  return false;
}
template <>
bool OperatorNode<PhysicalInnerNLJoin>::IsLogical() const {
  return false;
}
template <>
bool OperatorNode<PhysicalLeftNLJoin>::IsLogical() const {
  return false;
}
template <>
bool OperatorNode<PhysicalRightNLJoin>::IsLogical() const {
  return false;
}
template <>
bool OperatorNode<PhysicalOuterNLJoin>::IsLogical() const {
  return false;
}
template <>
bool OperatorNode<PhysicalInnerHashJoin>::IsLogical() const {
  return false;
}
template <>
bool OperatorNode<PhysicalLeftHashJoin>::IsLogical() const {
  return false;
}
template <>
bool OperatorNode<PhysicalRightHashJoin>::IsLogical() const {
  return false;
}
template <>
bool OperatorNode<PhysicalOuterHashJoin>::IsLogical() const {
  return false;
}

template <>
bool OperatorNode<LeafOperator>::IsPhysical() const {
  return false;
}
template <>
bool OperatorNode<LogicalGet>::IsPhysical() const {
  return false;
}
template <>
bool OperatorNode<LogicalProject>::IsPhysical() const {
  return false;
}
template <>
bool OperatorNode<LogicalFilter>::IsPhysical() const {
  return false;
}
template <>
bool OperatorNode<LogicalInnerJoin>::IsPhysical() const {
  return false;
}
template <>
bool OperatorNode<LogicalLeftJoin>::IsPhysical() const {
  return false;
}
template <>
bool OperatorNode<LogicalRightJoin>::IsPhysical() const {
  return false;
}
template <>
bool OperatorNode<LogicalOuterJoin>::IsPhysical() const {
  return false;
}
template <>
bool OperatorNode<LogicalAggregate>::IsPhysical() const {
  return false;
}
template <>
bool OperatorNode<LogicalLimit>::IsPhysical() const {
  return false;
}
template <>
bool OperatorNode<PhysicalScan>::IsPhysical() const {
  return true;
}
template <>
bool OperatorNode<PhysicalComputeExprs>::IsPhysical() const {
  return true;
}
template <>
bool OperatorNode<PhysicalFilter>::IsPhysical() const {
  return true;
}
template <>
bool OperatorNode<PhysicalInnerNLJoin>::IsPhysical() const {
  return true;
}
template <>
bool OperatorNode<PhysicalLeftNLJoin>::IsPhysical() const {
  return true;
}
template <>
bool OperatorNode<PhysicalRightNLJoin>::IsPhysical() const {
  return true;
}
template <>
bool OperatorNode<PhysicalOuterNLJoin>::IsPhysical() const {
  return true;
}
template <>
bool OperatorNode<PhysicalInnerHashJoin>::IsPhysical() const {
  return true;
}
template <>
bool OperatorNode<PhysicalLeftHashJoin>::IsPhysical() const {
  return true;
}
template <>
bool OperatorNode<PhysicalRightHashJoin>::IsPhysical() const {
  return true;
}
template <>
bool OperatorNode<PhysicalOuterHashJoin>::IsPhysical() const {
  return true;
}

} /* namespace optimizer */
} /* namespace peloton */
