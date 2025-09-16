//===- RockDialectParseTests.cpp - Tests for the Rock Dialect parser ----===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM
// Exceptions. See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "mlir/Dialect/Rock/IR/Rock.h"
#include "mlir/IR/AsmState.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/Parser/Parser.h"
#include "llvm/ADT/SmallVector.h"
#include "gtest/gtest.h"

using namespace mlir;
using namespace mlir::rock;

namespace {

// Test Fixture for Rock Dialect Parser Tests
class RockDialectParseTest : public ::testing::Test {
protected:
  RockDialectParseTest() : context() {
    context.getOrLoadDialect<RockDialect>();
  }
  
  MLIRContext context;
};

// Helper function to create a parser from a string
std::unique_ptr<AsmParser> createParser(MLIRContext &context, StringRef input) {
  auto memoryBuffer = llvm::MemoryBuffer::getMemBuffer(input);
  SourceMgr sourceMgr;
  sourceMgr.AddNewSourceBuffer(std::move(memoryBuffer), SMLoc());
  return parseSourceString(input, &context);
}

// Test the parseAndGather function with integer parsing
TEST_F(RockDialectParseTest, ParseAndGatherIntegers) {
  // Test parsing comma-separated integers with no delimiter
  std::string input = "1, 2, 3, 4";
  auto memoryBuffer = llvm::MemoryBuffer::getMemBuffer(input);
  SourceMgr sourceMgr;
  sourceMgr.AddNewSourceBuffer(std::move(memoryBuffer), SMLoc());
  
  Parser parser(sourceMgr, &context);
  SmallVector<int64_t> result;
  
  auto parseResult = parser.parseCommaSeparatedList(
    AsmParser::Delimiter::None,
    [&]() -> ParseResult {
      int64_t out;
      ParseResult res = parser.parseInteger(out);
      if (res.succeeded()) {
        result.push_back(out);
      }
      return res;
    }
  );
  
  EXPECT_TRUE(parseResult.succeeded());
  EXPECT_EQ(result.size(), 4u);
  EXPECT_EQ(result[0], 1);
  EXPECT_EQ(result[1], 2);
  EXPECT_EQ(result[2], 3);
  EXPECT_EQ(result[3], 4);
}

// Test parsing with square brackets delimiter
TEST_F(RockDialectParseTest, ParseAndGatherWithSquareBrackets) {
  std::string input = "[5, 10, 15]";
  auto memoryBuffer = llvm::MemoryBuffer::getMemBuffer(input);
  SourceMgr sourceMgr;
  sourceMgr.AddNewSourceBuffer(std::move(memoryBuffer), SMLoc());
  
  Parser parser(sourceMgr, &context);
  SmallVector<int64_t> result;
  
  auto parseResult = parser.parseCommaSeparatedList(
    AsmParser::Delimiter::Square,
    [&]() -> ParseResult {
      int64_t out;
      ParseResult res = parser.parseInteger(out);
      if (res.succeeded()) {
        result.push_back(out);
      }
      return res;
    }
  );
  
  EXPECT_TRUE(parseResult.succeeded());
  EXPECT_EQ(result.size(), 3u);
  EXPECT_EQ(result[0], 5);
  EXPECT_EQ(result[1], 10);
  EXPECT_EQ(result[2], 15);
}

// Test parsing strings
TEST_F(RockDialectParseTest, ParseAndGatherStrings) {
  std::string input = "[\"dim1\", \"dim2\", \"dim3\"]";
  auto memoryBuffer = llvm::MemoryBuffer::getMemBuffer(input);
  SourceMgr sourceMgr;
  sourceMgr.AddNewSourceBuffer(std::move(memoryBuffer), SMLoc());
  
  Parser parser(sourceMgr, &context);
  SmallVector<std::string> result;
  
  auto parseResult = parser.parseCommaSeparatedList(
    AsmParser::Delimiter::Square,
    [&]() -> ParseResult {
      std::string out;
      ParseResult res = parser.parseString(&out);
      if (res.succeeded()) {
        result.push_back(out);
      }
      return res;
    }
  );
  
  EXPECT_TRUE(parseResult.succeeded());
  EXPECT_EQ(result.size(), 3u);
  EXPECT_EQ(result[0], "dim1");
  EXPECT_EQ(result[1], "dim2");
  EXPECT_EQ(result[2], "dim3");
}

// Test empty list parsing
TEST_F(RockDialectParseTest, ParseAndGatherEmptyList) {
  std::string input = "[]";
  auto memoryBuffer = llvm::MemoryBuffer::getMemBuffer(input);
  SourceMgr sourceMgr;
  sourceMgr.AddNewSourceBuffer(std::move(memoryBuffer), SMLoc());
  
  Parser parser(sourceMgr, &context);
  SmallVector<int64_t> result;
  
  auto parseResult = parser.parseCommaSeparatedList(
    AsmParser::Delimiter::Square,
    [&]() -> ParseResult {
      int64_t out;
      ParseResult res = parser.parseInteger(out);
      if (res.succeeded()) {
        result.push_back(out);
      }
      return res;
    }
  );
  
  EXPECT_TRUE(parseResult.succeeded());
  EXPECT_EQ(result.size(), 0u);
}

// Test error handling when parsing fails
TEST_F(RockDialectParseTest, ParseAndGatherErrorHandling) {
  std::string input = "[1, invalid, 3]";
  auto memoryBuffer = llvm::MemoryBuffer::getMemBuffer(input);
  SourceMgr sourceMgr;
  sourceMgr.AddNewSourceBuffer(std::move(memoryBuffer), SMLoc());
  
  Parser parser(sourceMgr, &context);
  SmallVector<int64_t> result;
  
  auto parseResult = parser.parseCommaSeparatedList(
    AsmParser::Delimiter::Square,
    [&]() -> ParseResult {
      int64_t out;
      ParseResult res = parser.parseInteger(out);
      if (res.succeeded()) {
        result.push_back(out);
      }
      return res;
    }
  );
  
  EXPECT_TRUE(parseResult.failed());
  // Should have parsed the first element before failing
  EXPECT_EQ(result.size(), 1u);
  EXPECT_EQ(result[0], 1);
}

// Test single element parsing
TEST_F(RockDialectParseTest, ParseAndGatherSingleElement) {
  std::string input = "42";
  auto memoryBuffer = llvm::MemoryBuffer::getMemBuffer(input);
  SourceMgr sourceMgr;
  sourceMgr.AddNewSourceBuffer(std::move(memoryBuffer), SMLoc());
  
  Parser parser(sourceMgr, &context);
  SmallVector<int64_t> result;
  
  auto parseResult = parser.parseCommaSeparatedList(
    AsmParser::Delimiter::None,
    [&]() -> ParseResult {
      int64_t out;
      ParseResult res = parser.parseInteger(out);
      if (res.succeeded()) {
        result.push_back(out);
      }
      return res;
    }
  );
  
  EXPECT_TRUE(parseResult.succeeded());
  EXPECT_EQ(result.size(), 1u);
  EXPECT_EQ(result[0], 42);
}

} // namespace