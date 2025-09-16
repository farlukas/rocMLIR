add_rocmlir_unittest(MLIRRockDialectParseTests
  RockDialectParseTests.cpp
)

target_link_libraries(MLIRRockDialectParseTests
  PRIVATE
  MLIRRockDialect
  MLIRParser
)

add_rocmlir_unittest(MLIRRockTransformMapBuilderTests
  TransformMapBuilderTests.cpp
)

target_link_libraries(MLIRRockTransformMapBuilderTests
  PRIVATE
  MLIRRockOps
  MLIRRockUtility
)
