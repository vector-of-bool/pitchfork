/**
 * Module for manipulating CMake projects in Pitchfork projects
 */ /** */

import * as path from 'path';

import {dirForNamespace} from './dirs';
import {NewProjectParams} from './new';
import {fs} from './pr';

const CML_HEAD
    = `\
cmake_minimum_required(VERSION 3.10)

list(APPEND CMAKE_MODULE_PATH "\${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(<<PROJECT_NAME>> VERSION 0.0.1 DESCRIPTION "A great new project")

`;

const CML_INCL_THIRD
    = `\
# Include third-party components we need for the build
add_subdirectory(third_party)

`;

const SRC_CML
    = `\
add_library(
  <<PROJECT_NAME>>
  <<SRC_PATH>>/<<PROJECT_NAME>>.hpp
  <<SRC_PATH>>/<<PROJECT_NAME>>.cpp
  )
add_library(<<ALIAS_TARGET>> ALIAS <<PROJECT_NAME>>)
`;

const SRC_CML_WITH_INCLUDE_DIR
    = `\
target_include_directories(
  <<PROJECT_NAME>>
  PRIVATE \${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC $<BUILD_INTERFACE:\${PROJECT_SOURCE_DIR}/include>
  )
`;

const SRC_CML_WITH_NO_INCLUDE_DIR
    = `\
target_include_directories(
  <<PROJECT_NAME>>
  PUBLIC $<BUILD_INTERFACE:\${CMAKE_CURRENT_SOURCE_DIR}>
  )
`;

const FIRST_CPP
    = `\
#include <iostream>

#include "./<<PROJECT_NAME>>.hpp"

int <<ROOT_NAMESPACE>>::<<PROJECT_NAME_IDENT>>() {
    std::cout << "Calculating the answer...\\n";
    return 42;
}
`;

const FIRST_HPP
    = `\
#ifndef <<NS_UPPER>>_HPP_INCLUDED
#define <<NS_UPPER>>_HPP_INCLUDED

namespace <<ROOT_NAMESPACE>> {

// Calculate the answer
int <<PROJECT_NAME_IDENT>>();

} // <<ROOT_NAMESPACE>>

#endif // <<NS_UPPER>>_HPP_INCLUDED
`;

const FIRST_TEST_CPP
    = `\
#include <iostream>

#include <<<SRC_PATH>>/<<PROJECT_NAME>>.hpp>

int main() {
    const auto value = <<ROOT_NAMESPACE>>::<<PROJECT_NAME_IDENT>>();
    if (value == 42) {
        std::cout << "We calculated the value correctly\\n";
        return 0;
    } else {
        std::cout << "The value was incorrect!\\n";
        return 1;
    }
}
`;

const TEST_CML
    = `\
add_executable(first-test my_test.cpp)
target_link_libraries(first-test PRIVATE <<ALIAS_TARGET>>)
add_test(NAME first-test COMMAND first-test)
`;

const CML_MIDDLE
    = `\
add_subdirectory(src)

option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS AND (PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
    enable_testing()
    add_subdirectory(tests)
endif()

`;

const EXAMPLE1_CPP
    = `\
#include <iostream>

#include <<<SRC_PATH>>/<<PROJECT_NAME>>.hpp>

int main() {
    std::cout << "I am an example executable\\n";
    std::cout << "Let's calculate the value...\\n";
    const auto value = <<ROOT_NAMESPACE>>::<<PROJECT_NAME_IDENT>>();
    std::cout << "The value we got is " << value << '\\n';
}
`;

const EXAMPLES_CML
    = `\
add_executable(example1 example1.cpp)

target_link_libraries(example1 PRIVATE <<ALIAS_TARGET>>)
`;

const CML_INCL_EXAMPLES
    = `\
option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES AND (PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
    add_subdirectory(examples)
endif()

`;

function expandTemplate(tmpl: string, params: NewProjectParams): string {
  return tmpl.replace(/<<PROJECT_NAME>>/g, params.name)
      .replace(/<<SRC_PATH>>/g, dirForNamespace(params.rootNamespace))
      .replace(/<<ROOT_NAMESPACE>>/g, params.rootNamespace)
      .replace(/<<NS_UPPER>>/g, params.rootNamespace.toUpperCase())
      .replace(/<<ALIAS_TARGET>>/g, `${params.rootNamespace}::${params.name}`)
      .replace(/<<PROJECT_NAME_IDENT>>/g, params.name.replace(/-/g, '_'));
}

export async function createCMakeFiles(prDir: string, params: NewProjectParams) {
  const cml = path.join(prDir, 'CMakeLists.txt');
  let acc = CML_HEAD;

  function expand(tmpl: string) { return expandTemplate(tmpl, params); }

  // Include third_party if needed
  if (params.generateThirdParty) {
    const thirdCML = path.join(prDir, 'third_party', 'CMakeLists.txt');
    await fs.writeFile(thirdCML, '\n');
    acc += CML_INCL_THIRD;
  }

  // Write the src/CMakeLists.txt
  let srcAcc = SRC_CML;
  if (params.separateHeaders) {
    srcAcc += SRC_CML_WITH_INCLUDE_DIR;
  } else {
    srcAcc += SRC_CML_WITH_NO_INCLUDE_DIR;
  }
  srcAcc = expand(srcAcc);
  await fs.writeFile(path.join(prDir, 'src/CMakeLists.txt'), srcAcc);

  const libDir = path.join(prDir, 'src', dirForNamespace(params.rootNamespace));
  await fs.writeFile(path.join(libDir, `${params.name}.hpp`), expand(FIRST_HPP));
  await fs.writeFile(path.join(libDir, `${params.name}.cpp`), expand(FIRST_CPP));

  // Write an initial test
  const testsDir = path.join(prDir, 'tests');
  await fs.writeFile(path.join(testsDir, 'my_test.cpp'), expand(FIRST_TEST_CPP));
  await fs.writeFile(path.join(testsDir, 'CMakeLists.txt'), expand(TEST_CML));

  // The middle of the top cml:
  acc += CML_MIDDLE;

  // Generate some example files
  if (params.generateExamples) {
    const examplesDir = path.join(prDir, 'examples');
    await fs.writeFile(path.join(examplesDir, 'example1.cpp'), expand(EXAMPLE1_CPP));
    await fs.writeFile(path.join(examplesDir, 'CMakeLists.txt'), expand(EXAMPLES_CML));
    acc += CML_INCL_EXAMPLES;
  }

  // Write the root CMakeLists.txt
  acc = expandTemplate(acc, params);
  await fs.writeFile(cml, acc);
}