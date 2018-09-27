<pre class='metadata'>
Title: The Pitchfork Layout (PFL)
Shortname: cxx-pfl
Level: 1
Status: DREAM
Group: wg21
URL: https://vector-of-bool.github.io/psl.html
Markup Shorthands: markdown yes
Editor: Colby Pike, vectorofbool@gmail.com, https://vector-of-bool.github.io/
Abstract: PFL a convention for laying out source, build, and resource files in a
Abstract: filesystem to aide in uniformity, tooling, understandability, and
Abstract: compartmentalization.
</pre>

# Introduction # {#intro}

The phrase "it doesn't matter, just be consistent" has really harmed the C++
community more than it has helped. "A foolish consistency," if you will.
Developers' desire to keep their codebase unique and "special" has held back
the development of tooling when every user insists that a tool support their
own custom style.

For some things, like where you put the opening brace, the difference might
truly be insignificant. For other things, this isn't the case.

One such thing is the layout of a project on a filesystem.

Incongruous layouts not only harms tooling, they create unnecessary disparity,
create friction during on-boarding of contributors, and present a huge burden to
beginners for a task that shouldn't even take more than a few seconds of
thought.

For virtually all projects, they are some (maybe improper) subset of a few key
components:

- Compilable source files
- Public headers
- Private headers
- Source files containing entry points (`main()` functions)
- Documentation files
- Tests
- Samples and examples
- External libraries which have been embedded within the project structure
- "Add-ons" to the source (e.g. language bindings, optional plugins, platform
    bindings)

In addition, it is very common to see projects which subdivide themselves into
submodules. These submodules may also have dependencies between
each-other. Submodules greatly increase the complexity potential, but aren't
completely out of the question. A project layout standard *must* be able to
handle subdivided projects.

## Background and Terminology ## {#intro.bg}

The ideas and concepts presented herein are not novel. Rather, they are built
upon the work of others. Some come from professional idea and experience, while
others come from community convention, which has gradually converged on to
prefer certain patterns while decrying others.

### Physical vs. Logical Layout ### {#intro.bg.phy_log}

This document builds heavily upon the work of John Lakos, who has spear-headed
much work on the aspects of *physical design*. Much work has been put into
evangelizing in the area of *logical design*, but *physical design* to this day
remains a topic that (in this author's opinion) is too often overlooked when
designing and implementing systems, as well as the teaching thereof.

John Lakos's 1996 book *Large-Scale C++ Software Design*, to this day, remains
as *the* book on *physical design* in C++.

So what's the difference between *physical* and *logical* design?

**Logical design** pertains closely to programming-language-level aspects of the
design of a system.

**Physical design** pertains to the aspects of design *outside* of what is
encoded by the language.

The source unit dealt with by the C and C++ standards is the *translation
unit*. This corresponds to a source file which has had all preprocessor
directives resolved. This is the closest that the language standards get to
discussing *physical design* of systems.

Physical design, despite its name, still lives within the digital space, of
course. It concerns the placement and naming of files and directories within a
filesystem, as well as how to coordinate communication between physical units
where the relationship between their filesystem locations is not deterministic.

This document does not address much in the way of *logical design*. Rather, it
is specifically written to address many common questions and resolve common
answers for *physical design*.

### The *Physical Component* ### {#intro.bg.component}

The most fundamental unit of *physical design* is the **Component**.

A *component* should correspond to exactly two files: A *header* and a *source*
file. For some *rare* cases it may be beneficial to use more than a single
source file in a single physical component, but it may be a sign that further
refactoring is needed.

Additionally, a component may have a test source file, but it is not considered
a part of the physical component.

### Logical/Physical Coherence ### {#intro.bg.coherence}

This document assumes that developers will work to maintain logical/physical
*coherence*. There must be a coherent relationship between the logical and
physical components of a system, and a single logical component must not be
spread across multiple physical components. Multiple logical components should
not appear within the same physical component.

Note: *Logical components* can include more than an individual class or
function. For example, it may include classes representing private data (for
the PIMPL pattern), or `friend` classes and functions. Blindly splitting every
class and function into individual physical components is not required nor
recommended.

Physical components must not form cyclic dependencies.

<div class="example">
**Example of friendship**
```c++
// list.hpp
namespace acme {

template <typename T>
class list;

namespace detail {

// Class implementing a "list iterator"
template <typename T>
class list_iterator {
public:
    // Public default constructor
    list_iterator() = default;

private:
    // A private constructor used to initialize the iterator to the proper state
    explicit list_iterator(list<T>&);

    // Permit our list class to construct us with the private constructor
    friend class list<T>;
};

}

template <typename T>
class list {
public:
    using iterator = detail::list_iterator<T>;

    // The iterator class is a friend, giving it access to our internals
    friend class detail::list_iterator<T>;
};

}
```

The `list.cpp` is empty other than a single include directive:

```c++
// list.cpp
#include "list.hpp"
```

The purpose of the otherwise empty `list.cpp` is to ensure that the `list.hpp`
file will compile in isolation, ensuring that the header has the necessary
`#include` directives and/or forward declarations.

Despite having two distinct language-level `class` templates, we have a single
*logical component*. We say that these two class templates are "co-located."

We also have two source files, a `list.hpp` and `list.cpp`, but they together
form a single *physical component*.
</div>

<div class="example">
**Example using PIMPL**
```c++
// connection.hpp
#include <memory>

namespace acme {

// Forward-declare the private member data type
namespace detail { class connection_ipml; }

// The wrapper class
class connection {
private:
    // The actual PIMPL
    std::unique_ptr<detail::connection_impl> _impl;

public:
    connection(const std::string& address);
    ~connection();
};

}
```

And a corresponding `connection.cpp`:

```c++
#include "connection.hpp"

namespace acme::detail {

class connection_impl {
    // Whatever we need for the connection...
};

}

namespace {

std::unique_ptr<acme::detail::connection_impl>
init_private_data(const std::string& address) {
    // Initialize and return the connection data...
}

}

acme::connection::~connection() = default;
acme::connection::connection(const std::string& address)
    : _impl{ init_private_data(address) }
{}
```

Again we have two different (non-`template`) classes, and a single logical
component. In the header, the implementation detail class is only *forward*
declared, not fully defined. In the `connection.cpp` we provide the definition
for the detail class.
</div>

### Packages, Projects, Modules, and Submodules ### {#into.bg.pkg}

The terms defined in prior sections were heavily borrowed from John Lakos's
work. His work also often uses the term *package* to refer to a collection of
source components. This would roughly correspond to the entire source
repository, including build files, support files, documentation, scripts, and
data.

Unfortunately, the term "package" has been heavily overloaded. These days,
"package" most often refers to a unit of distribution of software, rather than
the software itself. For this reason, this document prefers the term "project,"
and will use it instead of "package," although they mean the same thing.

In a similar vein, the term *module* has also been heavily overloaded. In the
wake of upcoming C++ module specifications and implementations, the word
"module" will be avoided as to avoid ambiguity and confusion as a "module"
corresponds with neither a *package*/*project* nor a *physical component*.

Still, a term is needed to refer to the subdivisions of a large project into
smaller elements. For example, *Qt* is an enormous framework of many
interconnected pieces. To refer to these pieces *Qt* uses the term "module",
which has already been excluded.

For lack of a better unqualified term, the best this author can find is a
qualified term: *Submodule*. This term will be used to denote separate sections
of a project which can be consumed on as-needed basis. See the
[[#submod]] for more information.

## Project Files ## {#intro.files}

PFL prescribes several files that should be present in the root of the project:

- A `README` file should be present. It should be easily readable in plaintext,
    but may use "enhanced" plaintext like Markdown or similar. It should
    contain a description of the contents of the directory and subdirectories.
- A `LICENSE` file must be present for projects that wish to redistribute
    themselves. It must be plaintext (ie. not enhanced with markup).

Tool-support files, such as `.gitignore` and `.clang-format`, may be present in
this directory.

Other files in the root directory must be pertinent to the build system of the
project. Other files should not appear in the root of the project.

## Project Directories ## {#intro.dirs}

PFL prescribes several directories that should appear at the root of the project
tree. Not all of the directories are required, but they have an assigned
purpose, and no other directory in the filesystem may assume the role of one of
these directories. That is, these directories must be the ones used if their
purpose is required.

Other directories should not appear at the root.

Note: If you have a need not fulfilled by a PFL directory listed below, that is
a bug in this specification, and I would love to hear from you! Before
reporting, double-check that what you need isn't listed below and in the
following sections.

: [[#tld.build|build/]]
:: A special directory that should not be considered part of the source of the
    project. Used for storing ephemeral build results. must not be checked into
    source control. If using source control, must be ignored using source
    control ignore-lists.

: [[#tld.src|src/]]
:: Main compilable source location. must be present for projects with compiled
    components that do not use submodules.

    In the presence of [[#tld.include|include/]], also contains private
    headers.

: [[#tld.include|include/]]
:: Directory for public headers. may be present. may be omitted for projects
    that do not distinguish between private/public headers. may be omitted for
    projects that use submodules.

: [[#tld.tests|tests/]]
:: Directory for tests.

: [[#tld.examples|examples/]]
:: Directory for samples and examples.

: [[#tld.external|external/]]
:: Directory for packages/projects to be used by the project, but not edited as
    part of the project.

: [[#tld.extras|extras/]]
:: Directory containing extra/optional submodules for the project.

: [[#tld.data|data/]]
:: Directory containing non-source code aspects of the project. This might
    include graphics and markup files.

: [[#tld.tools|tools/]]
:: Directory containing development utilities, such as build and refactoring
    scripts

: [[#tld.docs|docs/]]
:: Directory for project documentation.

: [[#tld.libs|libs/]]
:: Directory for main project submodules.

# Top-Level Directories # {#tld}

Pitchfork specifies several top-level directories. Other directories should not
be present in the root directory, except for what is required by other tooling.

## `build/` ## {#tld.build}

This directory is not required, but its name should be reserved.

The `build/` directory is special in that it must not be committed to a source
control system. A user downloading the codebase should not see a `build/`
directory present in the project root, but one may be created in the course of
working with the software. The `_build/` directory is also reserved.

Note: Some build systems may commandeer the `build/` directory for themselves.
In this case, the directory `_build/` should be used in place of `build/`.

The `build/` directory may be used for ephemeral build results. Other uses of
the directory are not permitted.

Creation of additional directories for build results in the root directory is
not permitted.

Note: Although multiple *root* directories are not allowed, the structure and
layout of the `build/` directory is not prescribed. Multiple subdirectories of
`build/` may be used to hold multiple build results of different configuration.

## `include/` ## {#tld.include}

Note: The `include/` and [[#tld.src|src/]] directories are very closely
related. Be sure to also read its section in addition to this one.

The purpose of the `include/` directory is to hold *public API* headers.

The `include/` directory should not be used if using
[[#src.header-placement.merged]].

See [[#src]].

## `src/` ## {#tld.src}

Note: The `src/` and [[#tld.include|include/]] directories are very closely
related. Be sure to also read its section in addition to this one.

The purpose and content of `src/` depends on whether the project authors choose
to follow [[#src.header-placement.merged]] or
[[#src.header-placement.separate]].

See [[#src]].

## `tests/` ## {#tld.tests}

This directory is not required.

The `tests/` directory is reserved for source files related to (non-unit) tests
for the project.

The structure and layout of this directory is not prescribed by this document.

A project which can be embedded in another project (such as via
[[#tld.external]]), must disable its `tests/` directory if it can detect that it
is being built as an embedded sub-project.

Project maintainers must provide a way for consumers to disable the compilation
and running of tests, especially for the purpose of embedding.

## `examples/` ## {#tld.examples}

This directory is not required.

The `examples/` directory is reserved for source files related to example and
sample usage of the project. The structure and layout of this directory is not
prescribed by this document.

Project maintainers must provide a way for consumers to disable the compilation
of examples and samples.

## `external/` ## {#tld.external}

This directory is not required.

The `external/` directory is reserved for embedding of external projects. Each
embedded project should occupy a single subdirectory of `external/`.

`external/` should not contain files other than those required by tooling.

This directory may be automatically populated, either partially or completely,
by tools (eg. `git` submodules) as part of a build process. In this case,
projects must declare the auto-populated subdirectories as ignored by relevant
source control systems.

Subdirectories of `external/` should not be modified as part of regular project
development. Subdirectories should remain as close to their upstream source as
possible.

## `data/` ## {#tld.data}

This directory is not required.

The `data/` directory is designated for holding project files which should be
included in revision control, but are not explicitly code. For example,
graphics and localization files are not code in the same sense as the rest of
the project, but are good candidates for inclusion in the `data/` directory.

The structure and layout of this directory is not prescribed by this document.

## `tools/` ## {#tld.tools}

This directory is not required.

The `tools/` directory is designated for holding extra scripts and tools related
to developing and contributing to the project. For example, turn-key build
scripts, linting scripts, code-generation scripts, test scripts, or other tools
that may be useful to a project develop.

The contents of this directory should not be relevant to a project consumer.

## `docs/` ## {#tld.docs}

This directory is not required.

The `docs/` directory is designated to contain project documentation. The
documentation process, tools, and layout is not prescribed by this document.

## `libs/` ## {#tld.libs}

The `libs/` directory must not be used unless the project wishes to subdivide
itself into [[#submod|submodules]]. Its presence excludes the [[#tld.src|src/]]
and [[#tld.include|include/]] directories.

See [[#submod]] and [[#submod.libs]].

## `extras/` ## {#tld.extras}

This directory is not required.

`extras/` is a [[#submod.root|submodule root]]. See [[#submod]] and
[[#submod.extras]].

# Library Source Layout # {#src}

A *library source tree* refers to the layout of source code files that comprise
a single library, which is a collection of code that is exposed to the library's
*consumer*.

## Header File Placement ## {#src.header-placement}

This document supports two different methods of placing headers in a single
library: *separate* and *merged*. These two methods are mutually exclusive
within a single library source tree.

### Separate Header Placement ### {#src.header-placement.separate}

In *separated* placement, there are two *source directories*, [[#tld.include]]
and [[#tld.src]]. The `include/` directory is designated to contain the *public*
headers of the library, while the *src/* directory is designated to contain
the compilable source code and *private* headers.

Note: Not all projects will necessarily have private headers.

In separate placement, a single physical component is split between the two
directories. The relative path to the parent directory of a compilable source
file in the `src/` directory must be equivalent to the relative path to the
parent directory of the header in the `include/` directory that corresponds to
the compilable source file.

<div class="example">
Given a physical component of a header file `meow.hpp` and a source file
`meow.cpp`, we might place the header at `include/cat/sounds/meow.hpp`. The
relative path from `include/` to the parent directory of `meow.hpp` is
`cat/sounds.`

Thus, we can compose the path to the compilable source file as joining the
source directory name `src`, the relative path `cat/sounds`, and the filename
`meow.cpp` to get the path `src/cat/sounds/meow.cpp`. The following layout
results:

```text
<root>/
    include/
        cat/
            sounds/
                meow.hpp
    src/
        cat/
            sounds/
                meow.cpp
```
</div>

Note: The purpose of the deterministic header/file path relationship is to aid
both tools and human viewers in understanding and manipulating the source
directory structure.

Note: The relative paths of these physical components is not arbitrary. See
[[#src.layout]].

Consumers of a library using separated header layout should be given the path to
the [[#tld.include]] directory as the sole include search directory for the
library's public interface. This prevents users from being able to `#include`
paths which exist only in the `src/` directory.

The library itself should be compiled with both its [[#tld.include]] and
[[#tld.src]] directories as include search directories. This ensures that the
library itself can access all files within both source directories.

### Merged Header Placement ### {#src.header-placement.merged}

In *merged* header placement, there is a single *source directory*,
[[#tld.src]].

Much like with separated placement, the relative path from the source directory
to the parent of directory of the files of a physical component must be the
same. This implies that the files of a physical component will always be
*sibling* files in the same directory.

<div class="example">
Given a physical component with header `hiss.hpp` and `hiss.cpp`, and a relative
path of `cat/sounds`, the resulting layout is defined:

```text
<root>/
    src/
        cat/
            sounds/
                hiss.hpp
                hiss.cpp
```
</div>

## Test Placement ## {#src.tests}

This document distinguishes between *unit* tests, and other tests. Unit tests
are tests that roughly correspond to a single single *unit* of the source code.
This may be a physical component, public API, or combination thereof. The
distinguishing of a unit test has implications on where it may be placed.

### Merged Test Placement ### {#src.tests.merged}

Optional but recommended is to use *merged* test placement. In this method,
a unit test should have exactly one compilable source file, and that filename
stem should be the same as the filename stem of the physical component under
test, with a `.test` appended to it. For example, a test for the physical
component comprised of `meow.hpp` and `meow.cpp` will be named `meow.test.cpp`.
This unit test source file should be placed in the same directory as a
compilable source file of the physical component under test. Therefore, when
the unit has a compilable source file, the unit test source file will appear as
a sibling of the compilable source file.

### Separate Test Placement ### {#src.tests.separate}

If not using merged tests, all tests should be placed within the
[[#tld.tests|tests/]] top-level directory. There are no mandates on the layout
within `tests/`.

## Source Directory Layout ## {#src.layout}

For the purposes of this section, the [[#tld.include|include/]] and
[[#tld.src|src/]] top-level directories are both included in the definition of
"source directories." They are root of the *library source tree*. They are
named as such because they contain the primary "source files" of the source
language (C and/or C++).

No non-source-code files will should be placed *or generated* in any
subdirectories of a source directory. That is, the root of a source directory
may contain non-source-code files, but no child directories should.

Conversely, no source-code files should be placed in the root of a source
directory. That is, all source files must have *qualified paths* relative to the
root of their source directory.

Header files and source files should correspond to a logical component of the
project. For example, a `geometry` library might contain a `circle` class along
a single header and (optional) source file to represent it. If no other
logical components appear in that header and/or source file, the logical
component can be said to be the "main component" of the corresponding source
component. The main logical component may be a `class`, function, or some
grouping thereof.

The layout of the source tree should closely correspond to the namespace
structure of the project.

In C, there is no language-level concept of a namespace, but there is the
convention of qualifying globally visible identifiers with a "pseudo" namespace.
For example, a `libfoo` might define a `foo_create()`, where the prefix `foo_`
acts as the "namespace" for the identifier. The namespace for these purposes can
be said to be `foo`.

In C++, which has a language-level `namespace`, the need to qualify identifiers
in this way is not necessary (when using C++ linkage). Instead, these qualifiers
are put in `namespace`s.

Given that each logical component has a namespace, we can associate that
namespace with the physical component in which it is defined. This namespace
can then be used to generate a qualified path by which the physical component
can be found. In this way, physical components can be considered
"content-addressable".

Source files should be placed in a directory relative to the source
directory where the relative path is composed by joining the elements of the
component namespace as intermediate directories. The stem of the source filename
should correspond to the name of the logical component which it declares or
defines.

<div class="example">
Given a logical component `geo::shapes::circle`, we can use the qualifying
namespace to generate the relative path `geo/shapes`, and the component's leaf
name as a basis for the filename of `circle`. Thus, the full path to the sources
defining the logical component are `geo/shapes/circle.hpp` and
`geo/shapes/circle.cpp`.

If using [[#src.header-placement.separate]], the path from the project or
submodule root to the header will be `include/geo/shapes/circle.hpp`, and the
path to the compiled source file will be `src/geo/shapes/circle.cpp`.

If using [[#src.header-placement.merged]], then the header will appear as a
sibling in `src/geo/shapes/circle.hpp`.

The unit test for this component will appear at
`src/geo/shapes/circle.test.cpp`.
</div>

In some cases, it may be advantageous to separate the compiled source file into
multiple compiled source files while maintaining a single header file. In this
case, the stem of the source file should begin the same as if it were not
subdivided, then qualified with a `.` separating the distinguishing
characteristic of the source file.

<div class="example">
Given a logical component `geo::shapes::circle`, and two *extremely complex*
member functions `circumference()` and `area()`, we might split the
implementations of these methods into separate translation units. The resulting
physical component will have the following appearance:

```
<root>
    src/geo/shapes/
        circle.hpp
        circle.cpp
        circle.circumference.cpp
        circle.area.cpp
        circle.test.cpp
```
</div>

# Submodules # {#submod}

Very large projects (eg. Qt, Boost, JUCE, LLVM) will benefit from the concept of
*submodules*.

: Submodule
:: A subdivision of a larger project which can be consumed as-needed. Contains
    its own source trees, tests, data, and documentation.

Note: Splitting a project into submodules should be considered *very carefully*.
It is an extremely heavy tool with subtleties that often trip people up. Very
few projects warrant subdividing themselves in this way.
Most projects will do just fine with multiple namespaces and directories within
their source tree. Don't reach for this tool when namespaces and subdirectories
will suite you just fine. Converting a project to/from a submodule layout is
a very cumbersome task.

The following rules must be taken into consideration when considering or using
submodules:

- **Submodules are not themselves standalone projects**.
- They should not pretend to be entire projects.
- They cannot be consumed independent of the rest of the project.
- They should not be versioned separately from the project.
- They *cannot* further subdivide into sub-submodules.

## Submodule Root ## {#submod.root}

: Submodule Root
:: A directory whose child directories are [[#submod.dir|submodules]].

Submodule roots include:

- [[#tld.extras]]
- [[#submod.libs]]

Each subdirectory of a submodule root must correspond to exactly one submodule.

## Submodule Directory ## {#submod.dir}

A submodule is represented as a subdirectory of the project which may contain
the following directories:

- [[#tld.src|src/]] for submodule sources
- [[#tld.include|include/]] for submodule includes (if splitting headers)
- [[#tld.tests|tests/]] for submodule tests
- [[#tld.data|data/]] for submodule data
- [[#tld.examples|examples/]] for examples
- [[#tld.docs|docs/]] for submodule documentation

Note: Most of the [[#tld|top-level directories]] are *absent* from this list.

Submodules directories should not contain other files or directories except
those required by tooling.

## `libs/` ## {#submod.libs}

The `libs/` directory is for *main* submodules. It is a
[[#submod.root|submodule root]].

When the `libs/` directory is present, the top-level `src/` and `include/`
directories must not be present. Instead of having a root source tree, a project
using `libs/` for submodules should instead refactor itself such that the
project has a common basis submodule upon which other submodules will may
depend.

The main difference between `libs/` and [[#tld.extras|extras/]] is that the
`libs/` submodules found in `libs/` should be built _by default_, although a
consumer may opt-out of the submodules on an as-needed basis.

## `extras/` ## {#submod.extras}

The `extras/` directory is designated for containing additional submodules for
the project which build upon the main component(s). This may include submodules
that are not part of the project's "default" build, or otherwise impose special
requirements to be used.

For example, the following might be candidates for `extra/` rather than regular
components:

1. "Language bindings" or extra libraries that provide integrations of the
    project with programming languages or runtimes different from its own.
2. "Platform bindings" or extra libraries (plugins) that integrate the project
    with a particular platform. For example, a windowing library that needs to
    understand how to talk with Windows, Quartz, X11, and Wayland would include
    its platform integration implementations in this directory.
3. "Contributed" submodules. Additional submodules that are contributed by the
    project's users and included in upstream, but are not officially supported
    by the project.
4. Optional submodules that require additional dependencies, or may be
    prohibitive to include for all users. For example, Qt's Webkit module is
    prohibitively time consuming to build, and it requires the presence of
    dependencies that are only required exactly for that one component.

# Build Systems # {#builds}

This document does not mandate any particular build system. The only
requirements is that the chosen build system support the layout herein defined.

# Libraries # {#libraries}

For library projects, a source tree should correspond to *exactly one* library.
That is, *at most one one* linkable result, and *exactly one* public include
directory. A single `#include` tree must not require linking more than one
library to access all symbols exported from that tree.

Note: Submodules, having their own source tree, may each contain a library that
can be linked and consumed independently.

A single source tree should not vary its public interface based on anything
other than the target platform. This has several big implications, including
(but not limited to) the following:

- A library should not offer the user controls for tweaking its public
    interface.
- A library should not change its public interface based on the
    presence/absence of external software.

# Open Questions # {#open-qs}
